#ifndef _SPIFLASH_FS_H_
#define _SPIFLASH_FS_H_
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#define NULL 0

extern u32 spi_flash_read_data(u32 addr, u8* data, u32 size);
extern u32 spi_flash_write_scetor(u16 id, u8 *data);
extern u32 spi_flash_erase_sector(u16 setctor_id);
extern void spi_flash_erase_all(void);
extern u8 rx_data(void);
extern void tx_data(u8 data);
extern u32 get_time();


#define FLASH_RETURN_SIGN (0x80000000)
#define FLASH_ERROR_SIGN (0x80000000)
#define DEFAULT_RETURN_T u32
#define FLASH_OK (FLASH_ERROR_SIGN | 0x0)
#define FLASH_ERROR1 (FLASH_ERROR_SIGN | 0x1)
#define FLASH_ERROR2 (FLASH_ERROR_SIGN | 0x2)
#define FLASH_ERROR3 (FLASH_ERROR_SIGN | 0x3)
#define FLASH_ERROR4 (FLASH_ERROR_SIGN | 0x4)
#define FLASH_ERROR5 (FLASH_ERROR_SIGN | 0x5)
#define FLASH_ERROR6 (FLASH_ERROR_SIGN | 0x6)
#define FLASH_ERROR7 (FLASH_ERROR_SIGN | 0x7)
#define FLASH_ERROR8 (FLASH_ERROR_SIGN | 0x8)
#define FLASH_ERROR9 (FLASH_ERROR_SIGN | 0x9)
#define FLASH_ERRORa (FLASH_ERROR_SIGN | 0xa)
#define FLASH_ERRORb (FLASH_ERROR_SIGN | 0xb)
#define FLASH_ERRORc (FLASH_ERROR_SIGN | 0xc)

#define FLASH_STATUS1 (FLASH_ERROR_SIGN | 0x100)
#define FLASH_STATUS2 (FLASH_ERROR_SIGN | 0x200)
#define FLASH_STATUS3 (FLASH_ERROR_SIGN | 0x300)
#define FLASH_STATUS4 (FLASH_ERROR_SIGN | 0x400)
#define FLASH_STATUS5 (FLASH_ERROR_SIGN | 0x500)
#define FLASH_STATUS6 (FLASH_ERROR_SIGN | 0x600)
#define FLASH_STATUS7 (FLASH_ERROR_SIGN | 0x700)
#define FLASH_STATUS8 (FLASH_ERROR_SIGN | 0x800)
#define FLASH_STATUS9 (FLASH_ERROR_SIGN | 0x900)
#define FLASH_STATUSa (FLASH_ERROR_SIGN | 0xa00)


#define SPIFLASH_FS_CTOL_SADDR 0x0
#define SPIFLASH_FS_DATA_SADDR 0x1000
#define SPIFLASH_IGNORE_SIZE 0x100
#define SPIFLASH_SECTOR_SIZE 0x1000
#define SPIFLASH_SECTOR_OFFSET 12
#define SPIFLASH_BLOCK_OFFSET 16
#define SPIFLASH_SECTOR_SIZE_MASK (SPIFLASH_SECTOR_SIZE-1)
#define SPIFLASH_LINK_HEADSIZE 0x10
#define SPIFLASH_LINK_HEADSIZE_MASK (SPIFLASH_LINK_HEADSIZE-1)


typedef u32 spiflashaddr_t;
typedef u32 node_size_t;
typedef u16 node_len_t;
typedef u32 node_num_t;
typedef u16 node_id_t;
typedef u16 node_offset_t;

/*
cache_load(id_tmp);
cache_write(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
cache_read(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
cache_store(void);
*/

//u8 erase[256];/*TODO block */
/*
 * erase:
 *		1: erased,can used;
 *		0: non't  erased, can't used;
 */
u8 erase[16];	/* block number */

struct cache{
	node_id_t id;
	/*
	 *status: 
	 *		"0": isn't dirty data;
	 *		"1": dirty data, cache is noncohernet with flash, must write back.
	 *		"2": big data compare mode, cache.cache two part of size 2k.
	 *		"3": write head information mode.
	 */
	u8  status;
	u32 cache[512];
}cache;
/*
typedef struct vlink{
	spiflashaddr_t next;
	u32 date;
	node_size_t size;
	node_len_t len;
	u8 *data;
}* vlink_t;
*/

#define INROM_INFOR_SIZE 0x20

#define INROM_MAGIC_VALUE	0x12345678
#define INROM_MAGIC_SIZE	0x4

#define INROM_MAGIC_OFFSET	0x0
#define INROM_VHEAD_OFFSET	0x4
#define INROM_VTAIL_OFFSET	0x8
#define INROM_VNUM_OFFSET	0xc
#define INROM_RHEAD_OFFSET	0x10
#define INROM_RTAIL_OFFSET	0x14
#define INROM_RNUM_OFFSET	0x18

#define INROM_ERASE_OFFSET	0x100
#define INROM_ERASE_SIZE	0x10

#define INROM_LIST_NEXT_OFFSET	0x0
#define INROM_LIST_SIZE_OFFSET	0x4
#define INROM_LIST_DATALEN_OFFSET	0x8
#define INROM_LIST_DATE_OFFSET	0xc
#define INROM_LIST_SIZE		0x10
#define INROM_lIST_DATA_OFFSET	0x10
/* map:
 * |--------------------------------------------------------
 * |
 * |--------------------------------------------------------
 * |
 * |--------------------------------------------------------
 */
typedef struct rom_mesg{
	u32 dirty;
	u32 magic;
	spiflashaddr_t vhead;
	spiflashaddr_t vtail;
	node_num_t vnum;
	spiflashaddr_t rhead;
	spiflashaddr_t rtail;
	node_num_t rnum;
}rom_mesg_t;

rom_mesg_t rom_mesg_s;
/*
struct vlink_s{
	spiflashaddr_t head;
	spiflashaddr_t tail;
	node_num_t num;
}vlink_s;

typedef struct rlink{
	spiflashaddr_t next;
	node_len_t len;
	u32 date;
	node_size_t size;
	u8 *data;
}* rlink_t;
struct rlink_s{
	spiflashaddr_t head;
	spiflashaddr_t tail;
	node_num_t num;
}rlink_s;
*/

//ram struct
typedef struct node{  //for malloc&free
	spiflashaddr_t addr;
	node_size_t size;
}node_t;

typedef struct link{
	spiflashaddr_t	addr;
	spiflashaddr_t	next;
	node_size_t		size;
}linkhead_t;

DEFAULT_RETURN_T update_link_head(linkhead_t *myupdatelink);
DEFAULT_RETURN_T cache_load(node_id_t myid);
DEFAULT_RETURN_T cache_store(void);
DEFAULT_RETURN_T cache_read(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata);
DEFAULT_RETURN_T flash_data_read(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata);
DEFAULT_RETURN_T cache_write(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata);
/*
typedef struct updatelink{
	linkhead_t	*next;
	linkhead_t	element;
}updatelink_t;

struct updatelink_head{
	//linkhead_t	*next;
	node_id_t	id;
	updatelink_t	ele;
}updatelink_head;
*/
/*
 * updatelink: point to the linkhead data of to update on other sector.
 *
 */
#define UPDATE_NUM 3
u8 updatelink_num;
typedef struct updatelink{
/*
 * id:
 *   NULL:	don't used.
 *   other:	used.
 */
	node_id_t	id;
	linkhead_t	element;
}updatelink_t;

updatelink_t updatelink_buf[UPDATE_NUM];
#endif
