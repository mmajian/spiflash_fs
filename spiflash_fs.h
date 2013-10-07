#ifndef _SPIFLASH_FS_H_
#define _SPIFLASH_FS_H_

extern spi_flash_read_data(addr,  data, size);


#define SPIFLASH_FS_CTOL_SADDR 0x0
#define SPIFLASH_FS_DATA_SADDR 0x1000
#define SPIFLASH_IGNORE_SIZE 0x100
#define SPIFLASH_SECTOR_SIZE 0x1000
#define SPIFLASH_SECTOR_OFFSET 12
#define SPIFLASH_SECTOR_SIZE_MASK (SPIFLASH_SECTOR_SIZE-1)
#define SPIFLASH_LINK_HEADSIZE 0x10
#define SPIFLASH_LINK_HEADSIZE_MASK (SPIFLASH_LINK_HEADSIZE-1)

typedef u32 spiflashaddr_t;
typedef u32 node_size_t;
typedef u16 node_len_t;
typedef u16 node_num_t;
typedef u16 node_id_t;
typedef u16 node_offset_t;

linkhead_t*  read_link_head(spiflashaddr_t myfaddr, linkhead_t* mylinkhead);


cache_load(id_tmp);
cache_write(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
cache_read(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
cache_store(id_tmp);

u8 erase[256];/*TODO block */

struct cache{
	node_id_t id;
/*
 *status: 
 *		"0": dirty data;
 *		"1": dirty data, cache is noncohernet with flash, must write back.
 */
	u8  status; 
	u32 cache[512];
}cache;

typedef struct vlink{
	spiflashaddr_t next;
	u32 date;
	node_size_t size;
	node_len_t len;
	u8 *data;
}* vlink_t;

struct vlink_s{
	spiflashaddr_t head;
	spiflashaddr_t tail;
	node_num_t num;
}vlink_s;

typedef struct rlink{
	spiflashaddr_t next;
	u32 date;
	node_size_t size;
	node_len_t len;
	u8 *data;
}* rlink_t;

struct rlink_s{
	spiflashaddr_t head;
	spiflashaddr_t tail;
	u16 num;
}rlink_s;

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

typedef struct updatelink{
	linkhead_t	*next;
	linkhead_t	element;
}updatelink_t;

struct updatelink_head{
	//linkhead_t	*next;
	node_id_t	id;
	updatelink_t	ele;
}updatelink_head;
/*
 * updatelink[0]: point to the linkhead data of to update on current sector.
 * updatelink[1]: point to the linkhead data of to update on other sector.
 *
 */
#endif
