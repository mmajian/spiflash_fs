#ifndef _SPIFLASH_FS_H_
#define _SPIFLASH_FS_H_

#define SPIFLASH_FS_CTOL_SADDR 0x0
#define SPIFLASH_FS_DATA_SADDR 0x1000
#define SPIFLASH_IGNORE_SIZE 0x100

typedef u32 spiflashaddr_t;
typedef u32 node_size_t;
typedef u16 node_len_t;
typedef u16 node_num_t;
typedef u16 node_id_t;

linkhead_t* read_link_head(spiflashaddr_t faddr);

u8 erase[256];

struct cache{
	node_id_t id;
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
	spiflashaddr_t pro;
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
	spiflashaddr_t pro;
	spiflashaddr_t tail;
	u16 num;
}rlink_s;

typedef struct node{
	spiflashaddr_t addr;
	node_size_t size;
}node_t;

typedef struct link{
	spiflashaddr_t addr;
	node_size_t size;
}linkhead_t;
//ram status
typedef struct updatelink{
	updatelink_t *next;

	spiflashaddr_t addr;
	spiflashaddr_t next;
	node_size_t size;
	node_len_t len;

}updatelink_t;

/*
 * updatelink[0]: point to the linkhead data of to update on current sector.
 * updatelink[1]: point to the linkhead data of to update on other sector.
 *
 */
updatelink_t *updatelink[2];
#endif
