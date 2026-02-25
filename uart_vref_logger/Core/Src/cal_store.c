/*
 * cal_sstore.c
 *
 *  Created on: 2026. 2. 26.
 *      Author: Sunbi
 */

#include "cal_store.h"

// === 내부 상수 ===
#define FLASH_PAGE_SIZE_BYTES (1024u) // STM32F103C8/B 계열 페이지 크기
#define CAL_MAGIC (0xCA1Bu)
#define CAL_VERSION (0x0001u)

// 저장 가능한 보정값 범위 (VREFINT mV 추정값 범위)
#define CAL_MV_MIN (800u)
#define CAL_MV_MAX (1600u)

// === 내부 헬퍼 ===
static uint32_t cal_store_flash_last_page_addr_internal(void) {
	//STM32F1 Flash Size Data Register (KB)
	uint16_t size_kb = *(volatile uint16_t *)0x1FFFF7E0u;
	uint32_t flash_end = 0x08000000u + ((uint32_t)size_kb * 1024u);
	return flash_end - FLASH_PAGE_SIZE_BYTES;
}

// === 공개 함수 ===
uint32_t cal_store_get_last_page_addr(void) {
	return cal_store_flash_last_page_addr_internal();
}

uint16_t cal_store_crc16(uint16_t magic, uint16_t version, uint16_t cal_mv){
	// XOR 기반 무결성 체크(checksum)
	return (uint16_t)(magic ^ version ^ cal_mv ^ 0x5A5Au);
}

uint8_t cal_store_is_record_valid(const cal_rec_t *rec) {
	if (rec == NULL) return 0;

	if(rec->magic != CAL_MAGIC) return 0;
	if(rec->version != CAL_VERSION) return 0;

	uint16_t crc_calc = cal_store_crc16(rec->magic, rec->version, rec->vrefint_mv_cal);
	if(rec->crc != crc_calc) return 0;

	return 1;
}

void cal_store_read_raw(cal_rec_t *out_rec) {
	if(out_rec == NULL) return;

	uint32_t addr = cal_store_flash_last_page_addr_internal();
	const cal_rec_t *rec = (const cal_rec_t *)addr;

	*out_rec = *rec;
}

uint8_t cal_store_load(uint32_t *out_cal_mv) {
	if(out_cal_mv == NULL) return 0;

	uint32_t addr = cal_store_flash_last_page_addr_internal();
	const cal_rec_t *rec = (const cal_rec_t *)addr;

	if(!cal_store_is_record_valid(rec)) {
		return 0;
	}

	*out_cal_mv = (uint32_t)rec->vrefint_mv_cal;
	return 1;
}

cal_store_status_t cal_store_save(uint32_t cal_mv) {
	if(cal_mv < CAL_MV_MIN || cal_mv > CAL_MV_MAX) {
		return CAL_STORE_ERR_RANGE;
	}

	uint32_t addr = cal_store_flash_last_page_addr_internal();

	//기록할 레코드 준비
	cal_rec_t rec;
	rec.magic = CAL_MAGIC;
	rec.version = CAL_VERSION;
	rec.vrefint_mv_cal = (uint16_t)cal_mv;
	rec.crc = cal_store_crc16(rec.magic, rec.version, rec.vrefint_mv_cal);

	// Flash Unlock
	HAL_FLASH_Unlock();

	// 마지막 페이지 erase
	FLASH_EraseInitTypeDef erase = {0};
	uint32_t page_error = 0;

	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.PageAddress = addr;
	erase.NbPages = 1;

	if(HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) {
		HAL_FLASH_Lock();
		return CAL_STORE_ERR_FLASH_ERASE;
	}

	// halfword(16-bit) 단위 기록
	const uint16_t *p = (const uint16_t *)&rec;
	for(uint32_t i = 0; i < (sizeof(cal_rec_t) / 2u); i++) {
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + (i * 2u), p[i]) != HAL_OK) {
			HAL_FLASH_Lock();
			return CAL_STORE_ERR_FLASH_PROG;
		}
	}

	HAL_FLASH_Lock();

	// 기록 검증
	{
		const cal_rec_t *vr = (const cal_rec_t *)addr;
		if((vr->magic != rec.magic) ||
			(vr->version != rec.version) ||
			(vr->vrefint_mv_cal != rec.vrefint_mv_cal) ||
			(vr->crc != rec.crc)) {
			return CAL_STORE_ERR_VERIFY;
	}

	}
	return CAL_STORE_OK;
}

uint8_t cal_store_erase(void) {
	uint32_t addr = cal_store_flash_last_page_addr_internal();

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef erase = {0};
	uint32_t page_error = 0;

	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.PageAddress = addr;
	erase.NbPages = 1;

	if(HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) {
		HAL_FLASH_Lock();
		return 0;
	}

	HAL_FLASH_Lock();
	return 1;
}
