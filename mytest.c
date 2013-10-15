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
void main()
{
	int i;
	u32 ret;
	spiflash_fs_init();
		printf("--------------1--------\n");
	for(i = 0; ; i++)
	{
		ret = spiflash_add_list(1024);
		if(ret == NULL)
		{
			printf("-------------full!!!!!-------\n");
			break;
		}
		printf("--------------2-:%08x-------\n",i);
	}

	spiflash_final_update();

	for(i = 0; ; i++)
	{
		if(i == 0x1f7d)
			printf("--------------3--------\n");
		ret = spiflash_sequence_tx();
		if(ret == NULL)
		{
			printf("-------------empty!!!!!-------\n");
			break;
		}
		printf("-------------del :%08x--------\n",i);
	}
	spiflash_final_update();
		printf("--------------3--------\n");
	return;
}
