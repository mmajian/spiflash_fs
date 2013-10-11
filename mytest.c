typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
u32 spi_flash_read_data(u32 addr, u8* data, u32 size)
{
	return 0;
}

u32 spi_flash_write_scetor(u16 id, u8 *data)
{
	return 0;
}
u32 spi_flash_erase_sector(u16 setctor_id)
{
	return 0;
}
void spi_flash_erase_all(void)
{
	return 0;
}
u8 rx_data(void)
{
	return 0;
}
void tx_data(u8 data)
{
	return 0;
}
u32 get_time()
{
	return 0;
}
void mem_cmp(u8 *src,u8 *des, u32 size)
{
	return ;
}
u8 mem_cpy(u8 *src, u8 *des, u32 size)
{
	return 0;
}
void main()
{
	return;
}
