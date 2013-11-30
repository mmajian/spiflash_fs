
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#include "libspiflash_fs.h"
u32 sff_time_read2write(sff_rtime_t *mysfft)
{
	sff_wtime_t t_tmp;

	t_tmp.bit.sec =  mysfft->sec & ((1<<SEC_SIZE)-1);
	t_tmp.bit.min =  mysfft->min & ((1<<MIN_SIZE)-1);
	t_tmp.bit.hour = mysfft->hour & ((1<<HOUR_SIZE)-1);
	t_tmp.bit.day =  mysfft->day & ((1<<DAY_SIZE)-1);
	t_tmp.bit.mon =  mysfft->mon & ((1<<MON_SIZE)-1);
	t_tmp.bit.years =  mysfft->years & ((1<<YEARS_SIZE)-1);

	return t_tmp.date;
}

sff_rtime_t *sff_time_write2read(u32 myt, sff_rtime_t *mysfft)
{
	sff_wtime_t t_tmp;

	t_tmp.date = myt;

	mysfft->sec = t_tmp.bit.sec;
	mysfft->min = t_tmp.bit.min;
	mysfft->hour = t_tmp.bit.hour;
	mysfft->day = t_tmp.bit.day;
	mysfft->mon = t_tmp.bit.mon;
	mysfft->years = t_tmp.bit.years;

	return mysfft;
}
