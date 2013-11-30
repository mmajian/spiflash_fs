#ifndef _LIBSPIFLASH_FS_H_
#define _LIBSPIFLASH_FS_H_

#define SEC_SIZE	6
#define MIN_SIZE	6
#define HOUR_SIZE	5
#define DAY_SIZE	5
#define MON_SIZE	4
#define YEARS_SIZE	6



typedef struct spiflash_fs_rtime{
	u8 sec;
	u8 min;
	u8 hour;
	u8 day;
	u8 mon;
	u8 years;
}sff_rtime_t;

typedef union spiflash_fs_wtime{
	u32 date;
	struct{
		u32 sec:SEC_SIZE;
		u32 min:MIN_SIZE;
		u32 hour:HOUR_SIZE;
		u32 day:DAY_SIZE;
		u32 mon:MON_SIZE;
		u32 years:YEARS_SIZE;
	}bit;
}sff_wtime_t;

#endif
