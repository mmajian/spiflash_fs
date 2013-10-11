/*
 * MINI FS on spi flash.
 *
 *
 *
 *
 *
 *
 */
#include "spiflash_fs.h"

linkhead_t*  read_link_head(spiflashaddr_t myfaddr, linkhead_t* mylinkhead)
{
	node_t node_tmp;

	if(myfaddr == NULL)
		return NULL;

	flash_data_read(myfaddr, sizeof(node_t),&node_tmp);
	mylinkhead->addr = myfaddr;
	mylinkhead->next = node_tmp.addr;
	mylinkhead->size = node_tmp.size;

	return mylinkhead;
}

node_t* malloc_node(node_t *mynode)
{
	linkhead_t	*linkhead_tmp;
	linkhead_t	linkhead_s_tmp;
	//updatelink_t	updatelink_tmp;
	linkhead_t updatelink_tmp;
	spiflashaddr_t	faddr_tmp;
	node_size_t		size_tmp;
	u8 ok = 0;

	if((mynode == NULL ) || (mynode->size == NULL))
			return FLASH_ERROR1;

	linkhead_tmp = read_link_head(rom_mesg_s.rhead, &linkhead_s_tmp);
	faddr_tmp = linkhead_tmp->next;

	if(mynode->size < SPIFLASH_IGNORE_SIZE)
	{
		mynode->size = SPIFLASH_IGNORE_SIZE;
	}
	/*
	 * Align to SPIFLASH_LINK_HEADSIZE
	 * Head struct on flash don't across sector.
	 */
	if(mynode->size | SPIFLASH_LINK_HEADSIZE_MASK)
	{
		mynode->size = (mynode->size + SPIFLASH_IGNORE_SIZE) | ~(SPIFLASH_LINK_HEADSIZE_MASK);
	}

	while(linkhead_tmp != NULL)
	{

		if(mynode->size <= linkhead_tmp->size)
		{
			ok = 1;
		}

		faddr_tmp = linkhead_tmp->addr;
		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_s_tmp);
	}

	if(ok == 0)
		return FLASH_ERROR2;

	size_tmp = linkhead_tmp->size - mynode->size;

	if(size_tmp  >= SPIFLASH_IGNORE_SIZE)
	{
		if(faddr_tmp == rom_mesg_s.rhead)
		{
			rom_mesg_s.rhead = linkhead_tmp->addr - mynode->size;

		}
		else
		{
			updatelink_tmp.addr = faddr_tmp;
			updatelink_tmp.next = faddr_tmp - mynode->size - SPIFLASH_LINK_HEADSIZE;
			update_link_head(&updatelink_tmp);
		}

		updatelink_tmp.addr = faddr_tmp;
		updatelink_tmp.next = linkhead_tmp->next;
		updatelink_tmp.size = size_tmp;
//		updatelink_tmp.date = NULL;

		update_link_head(&updatelink_tmp);

		mynode->addr = linkhead_tmp->addr;
		mynode->size = mynode->size;
	}
	else
	{
		if(faddr_tmp == rom_mesg_s.rhead)
		{
			rom_mesg_s.rhead = linkhead_tmp->addr - mynode->size - SPIFLASH_LINK_HEADSIZE;

		}
		else
		{
			updatelink_tmp.addr = faddr_tmp;
			updatelink_tmp.next = linkhead_tmp->next;
			update_link_head(&updatelink_tmp);
		}

		mynode->addr = linkhead_tmp->addr;
		mynode->size = linkhead_tmp->size;

	}

	return mynode;

}

DEFAULT_RETURN_T free_node(node_t *mynode)
{
	linkhead_t *linkhead_tmp;
	linkhead_t linkhead_s_tmp;
	linkhead_t updatelink_s_tmp;
	linkhead_t *linkheadpre_tmp;
	u8 ok = 0;
	node_size_t size_tmp;

	linkhead_tmp = read_link_head(rom_mesg_s.rhead, &linkhead_s_tmp);
	linkheadpre_tmp = linkhead_tmp;

	if((mynode == NULL )|| (mynode->size == NULL))
			return FLASH_ERROR3;

	while(linkhead_tmp != NULL)
	{
		if(mynode->addr < linkhead_tmp->addr)
		{
			ok = 1;
			break;
		}

		linkheadpre_tmp = linkhead_tmp;
		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_s_tmp);

	}

		if(ok == 0)
		{
			if(mynode->addr == (linkheadpre_tmp->size + linkheadpre_tmp->addr))
			{
				updatelink_s_tmp.addr = linkheadpre_tmp->addr;
				updatelink_s_tmp.size = linkheadpre_tmp->size + mynode->size;
				update_link_head(&updatelink_s_tmp);
			}
			else
			{
				updatelink_s_tmp.addr = mynode->addr;
				updatelink_s_tmp.size = mynode->size;
				update_link_head(&updatelink_s_tmp);

				updatelink_s_tmp.addr = linkheadpre_tmp->addr;
				updatelink_s_tmp.next = mynode->addr;
				update_link_head(&updatelink_s_tmp);
			}
		}
		else
		{
			if(linkheadpre_tmp == linkhead_tmp)
			{
				if(linkhead_tmp->addr == (mynode->size + mynode->addr))
				{
					updatelink_s_tmp.addr = mynode->addr;
					updatelink_s_tmp.size = linkhead_tmp->size + mynode->size;
					updatelink_s_tmp.next = linkhead_tmp->next;
					update_link_head(&updatelink_s_tmp);
				}
				else
				{
					updatelink_s_tmp.addr = mynode->addr;
					updatelink_s_tmp.size = mynode->size;
					updatelink_s_tmp.next = linkheadpre_tmp->addr;
					update_link_head(&updatelink_s_tmp);
				}

				rom_mesg_s.rhead =  mynode->addr;
			}
			else
			{
				if(linkhead_tmp->addr == (mynode->size + mynode->addr))
				{
					updatelink_s_tmp.addr = mynode->addr;
					updatelink_s_tmp.size = linkhead_tmp->size + mynode->size;
					updatelink_s_tmp.next = linkhead_tmp->next;
					update_link_head(&updatelink_s_tmp);
				}
				else
				{
					updatelink_s_tmp.addr = mynode->addr;
					updatelink_s_tmp.size = mynode->size;
					updatelink_s_tmp.next = linkheadpre_tmp->addr;
					update_link_head(&updatelink_s_tmp);
				}

				if(mynode->addr == (linkheadpre_tmp->size + linkheadpre_tmp->addr))
				{
					updatelink_s_tmp.addr = linkheadpre_tmp->addr;
					updatelink_s_tmp.size = linkheadpre_tmp->size + mynode->size;
					update_link_head(&updatelink_s_tmp);
				}
				else
				{
					updatelink_s_tmp.addr = mynode->addr;
					updatelink_s_tmp.size = mynode->size;
					update_link_head(&updatelink_s_tmp);

					updatelink_s_tmp.addr = linkheadpre_tmp->addr;
					updatelink_s_tmp.next = mynode->addr;
					update_link_head(&updatelink_s_tmp);
				}
			}
		}

		return FLASH_OK;
}

static inline node_id_t  get_cache_id(void)
{
		return cache.id;
}

DEFAULT_RETURN_T update_link_head(linkhead_t *myupdatelink)
{
	node_id_t id_tmp;
	linkhead_t *linkhead_tmp;
//	updatelink_t *updatelink_tmp;
	u8 i;


start:
	id_tmp = (myupdatelink->addr | (~SPIFLASH_SECTOR_SIZE_MASK)) >> SPIFLASH_SECTOR_OFFSET;

	if(get_cache_id() == NULL)
	{
		cache_load(id_tmp);
	}

	if(id_tmp == get_cache_id())
	{

		if(myupdatelink->next)
			cache_write(myupdatelink->addr + INROM_LIST_NEXT_OFFSET, 0x4, (u8 *)&myupdatelink->next);

		if(myupdatelink->size)
			cache_write(myupdatelink->addr + INROM_LIST_SIZE_OFFSET, 0x4, (u8 *)&myupdatelink->size);

	}
	else if((updatelink_num != 0)&&(updatelink_buf[i].id != id_tmp))
	{
			cache_store();
			goto start;
	}
	else
	{
		for(i=0; i < UPDATE_NUM; i++)
		{
			if(updatelink_buf[i].id==NULL)
			{
				updatelink_buf[i].id = id_tmp;
				updatelink_buf[i].element.addr = myupdatelink->addr;
				updatelink_buf[i].element.next = myupdatelink->next;
				updatelink_buf[i].element.size = myupdatelink->size;
				updatelink_num = i+1;
				break;
			}
		}
	}

	if(i == UPDATE_NUM)
		return FLASH_ERROR4;

	return FLASH_OK;
}

DEFAULT_RETURN_T cache_load(node_id_t myid)
{
//	updatelink_t *updatelink_tmp;
//	updatelink_t updatelink_s_tmp;
	u8 i;

	if(cache.status != 0)
	{
		return FLASH_ERROR3;
	}

	spi_flash_read_data((myid << SPIFLASH_SECTOR_OFFSET), cache.cache, SPIFLASH_SECTOR_SIZE);

	cache.id = myid;

	if(updatelink_buf[0].id == myid)
	{
		for(i=0; (i < UPDATE_NUM) && (updatelink_buf[i].id != NULL); i++)
		{
			update_link_head(&updatelink_buf[i].element);
		}

		updatelink_buf[0].id = NULL;
		updatelink_num = 0;
		cache.status = 1;

	}

	return FLASH_OK;
}

DEFAULT_RETURN_T cache_store(void)
{
	u8 index_tmp;
	u8 offset_tmp;
	u8 mask_tmp;

	if(cache.status == 3)
	{
			cache.id = 0;
			goto mesg_store;
	}

	if(cache.id == NULL)
	{
		return FLASH_ERROR5;
	}

	if(cache.status == 0)
	{
		return FLASH_STATUS1;
	}

mesg_store:
	index_tmp = cache.id >> (SPIFLASH_BLOCK_OFFSET - SPIFLASH_SECTOR_OFFSET);
	offset_tmp = cache.id & 0xf ;

	if(erase[index_tmp] & (1<<offset_tmp))
	{
		spi_flash_erase_sector(cache.id);
	}

	spi_flash_write_scetor(cache.id,  cache.cache);
	cache.status = 0;

	return FLASH_OK;
}
/*
 * Determine maunaly before call this func:
 *			1.mylen < (SPIFLASH_SECTOR_SIZE - offset_tmp)
 *
 * If mydate is NULL, func return date addr on cache.cache.
 */
DEFAULT_RETURN_T cache_read(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata)
{
	node_offset_t offset_tmp;
	node_id_t id_tmp;

	id_tmp = myaddr >> SPIFLASH_SECTOR_OFFSET;

	if(cache.id != id_tmp)
		return FLASH_ERROR6;

	offset_tmp = myaddr & SPIFLASH_SECTOR_SIZE_MASK;

	if(mydata != NULL)
	{
		mem_cpy(mydata, &(cache.cache[offset_tmp]), mylen);
		return FLASH_OK;
	}
	else
	{
		return &cache.cache[offset_tmp];
	}


}

DEFAULT_RETURN_T flash_data_read(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata)
{
	node_offset_t offset_tmp;
	node_id_t id_tmp;

	offset_tmp = myaddr & SPIFLASH_SECTOR_SIZE_MASK;
	id_tmp = myaddr >> SPIFLASH_SECTOR_OFFSET;

	if(cache.id != id_tmp)
	{
		if(mylen <= (SPIFLASH_SECTOR_SIZE - offset_tmp))
		{
			cache_read(myaddr, mylen, mydata);
			return FLASH_STATUS2;
		}
	}
	else
	{
		spi_flash_read_data(myaddr,  mydata, mylen);
			return FLASH_STATUS3;
	}

}

DEFAULT_RETURN_T cache_write(spiflashaddr_t myaddr, node_len_t mylen, u8 *mydata)
{
	node_offset_t offset_tmp;
	node_id_t id_tmp;

	id_tmp = myaddr >> SPIFLASH_SECTOR_OFFSET;

	if(cache.id != id_tmp)
		return FLASH_ERROR7;

	offset_tmp = myaddr & SPIFLASH_SECTOR_SIZE_MASK;

	if(mylen > (SPIFLASH_SECTOR_SIZE - offset_tmp))
	{
		return FLASH_ERROR8;
	}

	mem_cpy(&(cache.cache[offset_tmp]), mydata, mylen);
	cache.status = 1;

	return FLASH_OK;

}

DEFAULT_RETURN_T update_rom_mesg(rom_mesg_t *mymesg)
{
	if(cache.status != 3)
		return FLASH_ERROR9;

	if(mymesg->dirty == 0)
		return FLASH_STATUS4 ;

	cache.id = 0;

	cache_write(INROM_MAGIC_OFFSET, sizeof(rom_mesg_t), (u8 *)&mymesg->magic);
	cache_write(INROM_ERASE_OFFSET, INROM_ERASE_SIZE, erase);

	return FLASH_OK;
	/*
	cache_write(INROM_MAGIC_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	cache_write(INROM_VHEAD_OFFSET, 0x4, (u8 *)&mymesg->v);
	cache_write(INROM_VTAIL_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	cache_write(INROM_VNUM_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	cache_write(INROM_RHEAD_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	cache_write(INROM_RTAIL_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	cache_write(INROM_RNUM_OFFSET, 0x4, (u8 *)&mymesg->vhead);
	*/
}
void spiflash_fs_first_init(void)
{
	u8 i;

	spi_flash_erase_all();

	for(i = 0; i < 16 ; i++)
	{
		erase[i] = 0xff;
	}

	rom_mesg_s.dirty = 1;
	rom_mesg_s.magic = INROM_MAGIC_VALUE;
	rom_mesg_s.vhead = 0;
	rom_mesg_s.vtail = 0;
	rom_mesg_s.vnum = 0;
	rom_mesg_s.rhead = SPIFLASH_FS_DATA_SADDR;
	rom_mesg_s.rtail = SPIFLASH_FS_DATA_SADDR;
	rom_mesg_s.rnum = 1;

	cache.status = 3;
	update_rom_mesg(&rom_mesg_s);
	cache_store();

	return FLASH_OK;
}

DEFAULT_RETURN_T spiflash_fs_init(void)
{
	u8 c[INROM_INFOR_SIZE];
	u8 *p;
	u8 i;
	p = c;
	u32 magic_tmp;


	spi_flash_read_data(INROM_MAGIC_OFFSET,  &magic_tmp, INROM_MAGIC_SIZE);
	if(magic_tmp != INROM_MAGIC_VALUE)
	{
		spiflash_fs_first_init();

		spi_flash_read_data(INROM_MAGIC_OFFSET,  &magic_tmp, INROM_MAGIC_SIZE);
		if(magic_tmp != INROM_MAGIC_VALUE)
			return FLASH_ERROR8;
	}
	/* init V&R links */
	spi_flash_read_data(0,  &c, INROM_INFOR_SIZE);

	rom_mesg_s.vhead = *(spiflashaddr_t *)(p+INROM_VHEAD_OFFSET);
	rom_mesg_s.vtail = *(spiflashaddr_t *)(p+INROM_VTAIL_OFFSET);
	rom_mesg_s.vnum = *(spiflashaddr_t *)(p+INROM_VNUM_OFFSET);

	rom_mesg_s.rhead = *(spiflashaddr_t *)(p+INROM_RHEAD_OFFSET);
	rom_mesg_s.rtail = *(spiflashaddr_t *)(p+INROM_RTAIL_OFFSET);
	rom_mesg_s.rnum = *(spiflashaddr_t *)(p+INROM_RNUM_OFFSET);

	/* init cache */
	cache.id = NULL;
	cache.status = NULL;

	/* init erase */
	spi_flash_read_data(INROM_ERASE_OFFSET,  erase, INROM_ERASE_SIZE);

	/* init updatelink */
	for(i = 0; i < UPDATE_NUM; i++)
	{
		updatelink_buf[i].id = NULL;
	}
	updatelink_num = 0;

	return FLASH_OK;
}
/*
 * this func is API, for store list.
 *
 */
DEFAULT_RETURN_T spiflash_add_list(node_size_t mylen)
{
	node_t	node_tmp;
	node_t	*node_p_tmp;
	node_id_t	id_my;
	node_offset_t	offset_tmp;
	u16 i;
	u8 *p;
	node_size_t s0, s1;
	linkhead_t lh_tmp;

	node_tmp.size = mylen;
	node_p_tmp = malloc_node(&node_tmp);

	if((DEFAULT_RETURN_T)node_p_tmp & FLASH_RETURN_SIGN)
		return (DEFAULT_RETURN_T)node_p_tmp;

	id_my = node_tmp.addr >> SPIFLASH_SECTOR_OFFSET;
	offset_tmp = node_tmp.addr & SPIFLASH_SECTOR_SIZE_MASK;

	if((SPIFLASH_SECTOR_SIZE - offset_tmp) < INROM_LIST_SIZE)
		return FLASH_ERRORb;

	if((offset_tmp + mylen + INROM_LIST_SIZE)& ~SPIFLASH_SECTOR_SIZE_MASK)
	{
		s0 = SPIFLASH_SECTOR_SIZE - offset_tmp - INROM_LIST_SIZE;
		s1 = (offset_tmp + mylen + INROM_LIST_SIZE) & SPIFLASH_SECTOR_SIZE_MASK;
	}
	else
	{
		s0 = mylen;
		s1 = 0;
	}

	if(id_my != cache.id)
	{
		switch(cache.status){
		case 1:
			cache_store();
			break;
		case 0:
			break;
		case 2:
		case 3:
			return FLASH_ERRORa;
			break;
		}

		cache_load(id_my);
	}

	/* add link list to vlinks */
	p = &cache.cache[offset_tmp];

	*(u32 *)(p + INROM_LIST_NEXT_OFFSET) = NULL;
	*(u32 *)(p + INROM_LIST_SIZE_OFFSET) = node_tmp.size;
	*(u32 *)(p + INROM_LIST_DATALEN_OFFSET) = mylen;
	*(u32 *)(p + INROM_LIST_DATE_OFFSET) = get_time();


	p = &cache.cache[offset_tmp + INROM_lIST_DATA_OFFSET];

	for(i = 0; i < s0; i++,p++)
	{
		*p = rx_data();
	}
	cache.status = 1;

	if(s1 != 0)
	{
		cache_store();
		cache_load(id_my+1);
		p = &cache.cache[0];

		for(i = 0; i < s1; i++,p++)
		{
			*p = rx_data();
		}
		cache.status = 1;
	}

	lh_tmp.addr = rom_mesg_s.vtail;
	lh_tmp.next = node_tmp.addr;
	lh_tmp.size = NULL;

	update_link_head(&lh_tmp);

	rom_mesg_s.vtail = node_tmp.addr;
	if(rom_mesg_s.vhead == NULL)
		rom_mesg_s.vhead = rom_mesg_s.vtail;

	rom_mesg_s.dirty = 1;

	return FLASH_OK;
}
/*
DEFAULT_RETURN_T spiflash_tx_list(spiflashaddr_t myaddr)
{

}
*/
/*
 * this func is API, for tx&del list when mode = 0;
 * mode:
 *     0: from vhead del one list, and tx data.
 *     1: del one list for myaddr, and don't data.
 *
 *
 */
DEFAULT_RETURN_T spiflash_del_list(spiflashaddr_t myaddr, u8 mode)
{
	node_t	node_tmp;
	node_t	*node_p_tmp;
	node_id_t	id_my;
	linkhead_t*   lk_p_tmp;
	linkhead_t   lk_tmp;
	linkhead_t   linkhead_s_tmp, linkheadpre_tmp;
	linkhead_t   *linkhead_tmp;
	u32 dsize_tmp;
	node_offset_t	offset_tmp;
	node_size_t s0, s1;
	u8 *p;
	u16 i;

	if(mode == 0)
		myaddr = rom_mesg_s.vhead;

	read_link_head(myaddr, &lk_tmp);
	flash_data_read(myaddr + INROM_LIST_DATALEN_OFFSET, 4, &dsize_tmp);

	id_my = lk_tmp.addr >> SPIFLASH_SECTOR_OFFSET;
	offset_tmp = lk_tmp.addr & SPIFLASH_SECTOR_SIZE_MASK;

	if(mode == 0)
	{
		if((offset_tmp + dsize_tmp + INROM_LIST_SIZE)& ~SPIFLASH_SECTOR_SIZE_MASK)
		{
			s0 = SPIFLASH_SECTOR_SIZE - offset_tmp - INROM_LIST_SIZE;
			s1 = (offset_tmp + dsize_tmp + INROM_LIST_SIZE) & SPIFLASH_SECTOR_SIZE_MASK;
		}
		else
		{
			s0 = dsize_tmp;
			s1 = 0;
		}

		if(id_my != cache.id)
		{
			switch(cache.status){
			case 1:
				cache_store();
				break;
			case 0:
				break;
			case 2:
			case 3:
				return FLASH_ERRORa;
				break;
			}

			cache_load(id_my);
		}


		p = &cache.cache[offset_tmp + INROM_lIST_DATA_OFFSET];

		for(i = 0; i < s0; i++,p++)
		{
			tx_data(*p);
		}

		if(s1 != 0)
		{
			cache_store();
			cache_load(id_my+1);
			p = &cache.cache[0];

			for(i = 0; i < s1; i++,p++)
			{
				tx_data(*p);
			}
		}
	}

	if(myaddr == rom_mesg_s.vhead )
	{
		rom_mesg_s.vhead = lk_tmp.next;
		rom_mesg_s.dirty = 1;
	}
	else
	{
		linkhead_tmp = read_link_head(rom_mesg_s.vhead, &linkhead_s_tmp);
		while(linkhead_tmp != NULL)
		{
			if(myaddr == linkhead_s_tmp.next)
			{
				break;
			}

			linkheadpre_tmp.addr = linkhead_s_tmp.addr;
			linkheadpre_tmp.next = linkhead_s_tmp.next;
			linkheadpre_tmp.size = linkhead_s_tmp.size;
			linkhead_tmp = read_link_head(linkhead_s_tmp.next, &linkhead_s_tmp);

		}

		linkhead_s_tmp.addr = linkheadpre_tmp.addr;
		linkhead_s_tmp.next = lk_tmp.next;
		linkhead_s_tmp.size = NULL;

		update_link_head(&linkhead_s_tmp);

	}

	if(myaddr == rom_mesg_s.vtail )
	{
		rom_mesg_s.vtail = linkheadpre_tmp.addr;
		rom_mesg_s.dirty = 1;
	}

	node_tmp.addr = myaddr;
	node_tmp.size = lk_tmp.size;
	free_node(&node_tmp);

	return FLASH_OK;
}

spiflashaddr_t * spiflash_find_list(node_len_t mylen)
{
	u16 i, nf = 1;
	u8 *p;
	linkhead_t *linkhead_tmp;
	linkhead_t linkhead_s_tmp, linkheadpre_s_tmp;
	spiflashaddr_t	faddr_tmp;
	node_len_t len_tmp;

	if(mylen > 0x800)
		return FLASH_ERRORc;

	if(cache.status == 1)
		cache_store();

	p = cache.cache[0];
	cache.status = 2;

	/* load data to cache */
	for(i = 0; i < mylen; i++)
	{
	  *p = rx_data();
	}

	linkhead_tmp = read_link_head(rom_mesg_s.vhead, &linkhead_s_tmp);
	while(linkhead_tmp != NULL)
	{
		flash_data_read(linkhead_tmp->addr + INROM_LIST_DATALEN_OFFSET,2, &len_tmp);
		if(len_tmp == mylen)
		{
			flash_data_read(linkhead_tmp->addr + INROM_lIST_DATA_OFFSET,mylen, &cache.cache[0x800]);
			nf = mem_cmp(&cache.cache[0],&cache.cache[0x800],mylen);
			if( nf == 1 )
				continue;
			else
				break;
		}

		linkheadpre_s_tmp.addr = linkhead_tmp->addr;
		linkheadpre_s_tmp.next = linkhead_tmp->next;
		linkheadpre_s_tmp.size = linkhead_tmp->size;

		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_s_tmp);

	}
	if(nf == 0)
		return linkhead_tmp->addr;
	else
		return 0;
}

/*
 * this func is API, for find&del list;
 */

DEFAULT_RETURN_T spiflash_remvoe_list(node_len_t mylen)
{
	spiflashaddr_t	faddr_tmp;

	faddr_tmp = spiflash_find_list(mylen);

	if((faddr_tmp == NULL) || (faddr_tmp == FLASH_RETURN_SIGN))
	{
		return 1;
	}
	spiflash_del_list(faddr_tmp,1);

		return 0;
}

