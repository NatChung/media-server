// ITU-T H.222.0(10/2014)
// Information technology �C Generic coding of moving pictures and associated audio information: Systems
// 2.4.4.8 Program map table(p68)

#include "mpeg-ts-proto.h"
#include "mpeg-util.h"
#include <string.h>
#include <assert.h>

size_t pmt_read(const uint8_t* data, size_t bytes, pmt_t *pmt)
{
	int i = 0;
	uint32_t j = 0, k = 0, n = 0;

	uint32_t table_id = data[0];
	uint32_t section_syntax_indicator = (data[1] >> 7) & 0x01;
//	uint32_t zero = (data[1] >> 6) & 0x01;
//	uint32_t reserved = (data[1] >> 4) & 0x03;
	uint32_t section_length = ((data[1] & 0x0F) << 8) | data[2];
	uint32_t program_number = (data[3] << 8) | data[4];
//	uint32_t reserved2 = (data[5] >> 6) & 0x03;
	uint32_t version_number = (data[5] >> 1) & 0x1F;
//	uint32_t current_next_indicator = data[5] & 0x01;
	uint32_t sector_number = data[6];
	uint32_t last_sector_number = data[7];
//	uint32_t reserved3 = (data[8] >> 6) & 0x03;
	uint32_t PCR_PID = ((data[8] & 0x1F) << 8) | data[9];
//	uint32_t reserved4 = (data[10] >> 4) & 0x0F;
	uint32_t program_info_length = ((data[10] & 0x0F) << 8) | data[11];

//	printf("PMT: %0x %0x %0x %0x %0x %0x %0x %0x, %0x, %0x, %0x, %0x\n", (unsigned int)data[0], (unsigned int)data[1], (unsigned int)data[2], (unsigned int)data[3], (unsigned int)data[4], (unsigned int)data[5], (unsigned int)data[6],(unsigned int)data[7],(unsigned int)data[8],(unsigned int)data[9],(unsigned int)data[10],(unsigned int)data[11]);

	assert(PAT_TID_PMS == table_id);
	assert(1 == section_syntax_indicator);
	assert(0 == sector_number);
	assert(0 == last_sector_number);
	pmt->PCR_PID = PCR_PID;
	pmt->pn = program_number;
	pmt->ver = version_number;
	pmt->pminfo_len = program_info_length;

	i = 12;
	if(program_info_length)
	{
		// descriptor()
	}

	assert(bytes >= section_length + 3); // PMT = section_length + 3
	for(j = 0; j < section_length - 9 - pmt->pminfo_len - 4; j += 5) // 4:CRC, 9: follow section_length item
	{
		pmt->streams[n].avtype = data[i+j];
		pmt->streams[n].pid = ((data[i+j+1] & 0x1F) << 8) | data[i+j+2];
		pmt->streams[n].esinfo_len = ((data[i+j+3] & 0x0F) << 8) | data[i+j+4];
//		printf("PMT[%d]: sid: %0x, pid: %0x, eslen: %d\n", n, pmt->streams[n].avtype, pmt->streams[n].pid, pmt->streams[n].esinfo_len);

		for(k = 0; k < pmt->streams[n].esinfo_len; k++)
		{
			// descriptor
		}

		j += pmt->streams[n].esinfo_len;
		n++;
	}

	pmt->stream_count = n;

	i += j;
	//assert(i+4 == bytes);
	//crc = (data[i] << 24) | (data[i+1] << 16) | (data[i+2] << 8) | data[i+3];
	// assert(0 == crc32((unsigned int)(-1), data, section_length+3));
	return 0;
}

size_t pmt_write(const pmt_t *pmt, uint8_t *data)
{
	// 2.4.4.8 Program map table (p68)
	// Table 2-33

	uint32_t i = 0;
	uint32_t crc = 0;
	ptrdiff_t len = 0;
	uint8_t *p = NULL;

	data[0] = PAT_TID_PMS;	// program map table

	// skip section_length

	// program_number
	nbo_w16(data + 3, (uint16_t)pmt->pn);

	// reserved '11'
	// version_number 'xxxxx'
	// current_next_indicator '1'
	data[5] = (uint8_t)(0xC1 | (pmt->ver << 1));

	// section_number/last_section_number
	data[6] = 0x00;
	data[7] = 0x00;

	// reserved '111'
	// PCR_PID 13-bits 0x1FFF
	nbo_w16(data + 8, (uint16_t)(0xE000 | pmt->PCR_PID));

	// reserved '1111'
	// program_info_lengt 12-bits, the first two bits of which shall be '00'.
	assert(pmt->pminfo_len < 0x400);
	nbo_w16(data + 10, (uint16_t)(0xF000 | pmt->pminfo_len));
	if(pmt->pminfo_len > 0 && pmt->pminfo_len < 0x400)
	{
		// fill program info
		assert(pmt->pminfo);
		memcpy(data + 12, pmt->pminfo, pmt->pminfo_len);
	}

	// streams
	p = data + 12 + pmt->pminfo_len;
	for(i = 0; i < pmt->stream_count && p - data < 1021 - 4 - 5 - pmt->streams[i].esinfo_len; i++)
	{
		// stream_type
		*p = (uint8_t)pmt->streams[i].avtype;

		// reserved '111'
		// elementary_PID 13-bits
		nbo_w16(p + 1, 0xE000 | pmt->streams[i].pid);

		// reserved '1111'
		// ES_info_lengt 12-bits
		nbo_w16(p + 3, 0xF000 | pmt->streams[i].esinfo_len);

		// fill elementary stream info
		if(pmt->streams[i].esinfo_len > 0)
		{
			assert(pmt->streams[i].esinfo);
			memcpy(p + 5, pmt->streams[i].esinfo, pmt->streams[i].esinfo_len);
		}

		p += 5 + pmt->streams[i].esinfo_len;
	}

	// section_length
	len = p + 4 - (data + 3); // 4 bytes crc32
	assert(len <= 1021); // shall not exceed 1021 (0x3FD).
	assert(len <= TS_PACKET_SIZE - 7);
	// section_syntax_indicator '1'
	// '0'
	// reserved '11'
	nbo_w16(data + 1, (uint16_t)(0xb000 | len)); 

	// crc32
	crc = crc32(0xffffffff, data, (uint32_t)(p-data));
	//put32(p, crc);
	p[3] = (crc >> 24) & 0xFF;
	p[2] = (crc >> 16) & 0xFF;
	p[1] = (crc >> 8) & 0xFF;
	p[0] = crc & 0xFF;

	return (p - data) + 4; // total length
}
