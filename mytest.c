typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

u8 dev[8*1024*1024];

u32 spi_flash_read_data(u32 addr, u8* data, u32 size)
{
	unsigned int i;

	for(i = 0; i < size; i++)
		data[i]=dev[addr+i];

	return 0;
}

u32 spi_flash_write_scetor(u16 id, u8 *data)
{
	unsigned int i;

	for(i = 0 ; i< 4*1024; i++)
		dev[id * 0x1000 + i] = data[i];

	return 0;
}
u32 spi_flash_erase_sector(u16 setctor_id)
{
	unsigned int i;

	for(i = 0 ; i< 4*1024; i++)
		dev[setctor_id * 0x1000 + i] = 0xff;

	return 0;
}
void spi_flash_erase_all(void)
{
	unsigned int i;
	for(i = 0 ; i< 8*1024*1024; i++)
		dev[i]= 0xff;
	return 0;
}

int abc;
int abd = 1;
int abd_old = 0;
u8 tx_buf[4*1024];
u8 *tx_p;
u8 rx_data(void)
{
	if(abc == 0x20)
		abc = 0;
	return (0xab + abc);
}

u8 rx_data1(void)
{
	return (0xac);
}
void tx_data(u8 data)
{
	if(abd != abd_old)
	{
		abd_old = abd;
		tx_p = tx_buf;
	}

	if(abd == abd_old)
	{
		*(tx_p) = data;
		tx_p ++;
	}
	return 0;
}
u32 get_time()
{
	return 0;
}
void mem_set(u8 *des, u8 data, u32 size)
{
	u32 i;
	for(i = 0 ; i < size; i++)
		des[i] = data;

	return ;
}
void mem_cpy(u8 *des, u8 *src, u32 size)
{
	u32 i;
	for(i = 0 ; i < size; i++)
		des[i] = src[i];

	return ;
}
u8 mem_cmp(u8 *des, u8 *src, u32 size)
{
	u32 i;
	for(i = 0 ; i < size; i++)
		if(des[i] != src[i])
			return 1;
	return 0;
}
#define NULL 0
#include "libspiflash_fs.h"
extern sff_rtime_t *sff_time_write4read(u32 myt, sff_rtime_t *mysfft);
extern u32 sff_time_read2write(sff_rtime_t *mysfft);
void main()
{
	int i;
	u32 ret;
	u32 add;

#define TIME_TEST
#ifdef TIME_TEST
	sff_rtime_t mysfft;
	u32 t_tmp;

	mysfft.sec = 32;
	mysfft.min = 31;
	mysfft.hour = 22;
	mysfft.day = 21;
	mysfft.mon = 12;
	mysfft.years = 13;

	t_tmp = sff_time_read2write(&mysfft);

	printf("--time:%08x------------1--------\n", t_tmp);

	t_tmp = t_tmp + 1;

		printf("--time:%08x------------2--------\n", t_tmp);

	sff_time_write2read(t_tmp, &mysfft);

		printf("--time:------------3--------\n");
		printf("\tmysfft.sec:%d\n",mysfft.sec);
		printf("\tmysfft.min:%d\n",mysfft.min);
		printf("\tmysfft.hour:%d\n",mysfft.hour);
		printf("\tmysfft.day:%d\n",mysfft.day);
		printf("\tmysfft.mon:%d\n",mysfft.mon);
		printf("\tmysfft.years:%d\n",mysfft.years);
		printf("----------------------------\n");
		return;
#endif
	spiflash_fs_init();
		printf("--------------1--------\n");
	for(i = 0; ; i++)
	{
		printf("--------------2-:%08x-------\n",i);
		ret = spiflash_add_list(1014);
		if(ret == NULL)
		{
			printf("-------------full!!!!!-------\n");
			break;
		}
	}

		printf("--------------test--------\n");

	spiflash_final_update();
	//add = spiflash_remvoe_list(1014);

	//spiflash_del_list(0x7ffb40,1);
	spiflash_del_list(0x1000,1);
#if 0
	if(add == 0)
			printf("-------------success del list-------\n");
	else
			printf("-------------faild del list-------\n");

	for(add = 0x1818; add < 0x3000; add+=0x40c)
	{
		printf("-------------d 2-------\n");
		spiflash_del_list(add,1);
	}
		printf("-------------test 2-------\n");
#if 0
	for(i = 0; i< 0x1000 ; i++)
	{
		printf("-------------del :%08x--------\n",i);
		ret = spiflash_sequence_tx();
		if(ret == NULL)
		{
			printf("-------------empty!!!!!-------\n");
			break;
		}
	}
#else
		ret = spiflash_sequence_tx();
#endif
#endif
#if 1
	spiflash_final_update();
	for(i = 0; ; i++)
	{
		ret = spiflash_add_list(400);
		if(ret == NULL)
		{
			printf("-------------full!!!!!-------\n");
			break;
		}
		printf("--------------2-:%08x-------\n",i);
	}
#endif

	spiflash_final_update();
		printf("--------------3--------\n");

	return;
}
