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
	node_id_t node_id;
	u8 i;

	if(myfaddr == NULL)
		return NULL;
	node_id = myfaddr >> SPIFLASH_SECTOR_OFFSET;

	if((updatelink_num != 0) && (updatelink_buf[0].id == node_id))
	{
		for(i=0; i < updatelink_num; i++)
		{
			if(updatelink_buf[i].element.addr == myfaddr)
			{
				mylinkhead->next = updatelink_buf[i].element.next;
				mylinkhead->size = updatelink_buf[i].element.size;
				goto find;
			}
		}
	}

	flash_data_read(myfaddr, sizeof(node_t),&node_tmp);
	mylinkhead->next = node_tmp.addr;
	mylinkhead->size = node_tmp.size;
find:
	mylinkhead->addr = myfaddr;
	return mylinkhead;
}

node_t* malloc_node(node_t *mynode)
{
	linkhead_t	*linkhead_tmp;
	linkhead_t	linkhead_s_tmp;
	spiflashaddr_t	faddrpre_tmp;
	//updatelink_t	updatelink_tmp;
	linkhead_t updatelink_tmp;
	spiflashaddr_t	f;
	node_size_t		size_tmp;
	u8 ok = 0;

	if((mynode == NULL ) || (mynode->size == NULL))
			return NULL;

	linkhead_tmp = read_link_head(rom_mesg_s.rhead, &linkhead_s_tmp);

	if(mynode->size < SPIFLASH_IGNORE_SIZE)
	{
		mynode->size = SPIFLASH_IGNORE_SIZE;
	}
	/*
	 * Align to SPIFLASH_LINK_HEADSIZE
	 * Head struct on flash don't across two sector.
	 */
	/*
	if(mynode->size & SPIFLASH_LINK_HEADSIZE_MASK)
	{
		mynode->size = (mynode->size + SPIFLASH_IGNORE_SIZE) & ~(SPIFLASH_LINK_HEADSIZE_MASK);
	}
	*/

	faddrpre_tmp = NULL;
	while(linkhead_tmp != NULL)
	{

//		faddr_tmp = linkhead_tmp->addr;
		if(mynode->size <= linkhead_tmp->size)
		{
			ok = 1;
			break;
		}

		faddrpre_tmp = linkhead_tmp->addr;
		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_s_tmp);
	}

	if(ok == 0)
	{
		return NULL;
	}

	/*
	 * Align to SPIFLASH_LINK_HEADSIZE
	 * Head struct on flash don't across two sector.
	 */
	f = SPIFLASH_SECTOR_SIZE - ((linkhead_tmp->addr + mynode->size) & SPIFLASH_SECTOR_SIZE_MASK);
	if(f < SPIFLASH_LINK_HEADSIZE)
		mynode->size = mynode->size + f;


	size_tmp = linkhead_tmp->size - mynode->size;

	if(size_tmp  >= SPIFLASH_IGNORE_SIZE)
	{
		//if(linkhead_tmp->addr == rom_mesg_s.rhead)
		if(faddrpre_tmp == NULL)
		{
			rom_mesg_s.rhead = linkhead_tmp->addr + mynode->size;
			if(rom_mesg_s.rnum == 1)
				rom_mesg_s.rtail = rom_mesg_s.rhead;


			rom_mesg_s.dirty = 1;

		}
		else
		{
			if(linkhead_tmp->addr == rom_mesg_s.rtail)
				rom_mesg_s.rtail = linkhead_tmp->addr;

			updatelink_tmp.addr = faddrpre_tmp;
			updatelink_tmp.next = linkhead_tmp->addr + mynode->size;
			update_link_head(&updatelink_tmp);
		}

		updatelink_tmp.addr = linkhead_tmp->addr + mynode->size;

		if(linkhead_tmp->next == NULL)
			updatelink_tmp.next = NEXT_NULL;
		else
			updatelink_tmp.next = linkhead_tmp->next;

		updatelink_tmp.size = size_tmp;
//		updatelink_tmp.date = NULL;

		update_link_head(&updatelink_tmp);

		mynode->addr = linkhead_tmp->addr;
		mynode->size = mynode->size;
	}
	else
	{
		rom_mesg_s.rnum --;
		rom_mesg_s.dirty = 1;

		if(!rom_mesg_s.rnum)
		{
			rom_mesg_s.rhead = NULL;
			rom_mesg_s.rtail = NULL;
		}
		else
		{

		//	if(linkhead_tmp->addr == rom_mesg_s.rhead)
			if(faddrpre_tmp == NULL)
			{
				rom_mesg_s.rhead = linkhead_tmp->addr + mynode->size;

			}
			else
			{
				if(linkhead_tmp->addr == rom_mesg_s.rtail)
					rom_mesg_s.rtail = faddrpre_tmp;

				updatelink_tmp.addr = faddrpre_tmp;
				updatelink_tmp.next = linkhead_tmp->next;
				update_link_head(&updatelink_tmp);
			}
		}

		mynode->addr = linkhead_tmp->addr;
		mynode->size = linkhead_tmp->size;

	}

	rom_mesg_s.vnum ++;
	rom_mesg_s.dirty = 1;

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
	id_tmp = (myupdatelink->addr & (~SPIFLASH_SECTOR_SIZE_MASK)) >> SPIFLASH_SECTOR_OFFSET;

	if(get_cache_id() == NULL)
	{
		cache_load(id_tmp);
	}

	if(id_tmp == get_cache_id())
	{

		if(myupdatelink->next)
		{
			if(myupdatelink->next == NEXT_NULL)
				myupdatelink->next = NULL;
			cache_write(myupdatelink->addr + INROM_LIST_NEXT_OFFSET, 0x4, (u8 *)&myupdatelink->next);
		}

		if(myupdatelink->size)
			cache_write(myupdatelink->addr + INROM_LIST_SIZE_OFFSET, 0x4, (u8 *)&myupdatelink->size);

	}
	else if((updatelink_num != 0)&&(updatelink_buf[0].id != id_tmp))
	{
			cache_store();
			cache_load(updatelink_buf[0].id);
			goto start;
	}
	else
	{
			if(updatelink_num > UPDATE_NUM)
			{
				printf("-------------ERROR--------------------------------------\n");
				return FLASH_ERROR4;
			}
			if(( updatelink_num == 0 )|| (updatelink_buf[updatelink_num-1].id == id_tmp))
			{
				updatelink_buf[updatelink_num].id = id_tmp;
				updatelink_buf[updatelink_num].element.addr = myupdatelink->addr;
				updatelink_buf[updatelink_num].element.next = myupdatelink->next;
				updatelink_buf[updatelink_num].element.size = myupdatelink->size;
				updatelink_num ++;
			}
	}
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
		for(i=0; (i < updatelink_num) && (updatelink_buf[i].id != NULL); i++)
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
	u16 index_tmp;
	u16 offset_tmp;
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
	index_tmp = cache.id >> (SPIFLASH_BLOCK_OFFSET - SPIFLASH_SECTOR_OFFSET );
	offset_tmp = cache.id & 0xf ;

	if(offset_tmp >= 8)
	{
		index_tmp ++;
		offset_tmp = offset_tmp & 0x7;
	}
	if((erase[index_tmp] & (1<<offset_tmp)) == 0)
	{
		spi_flash_erase_sector(cache.id);
	}

	if(cache.status != 3)
	{
		erase[index_tmp] = erase[index_tmp] & ~(1<<offset_tmp);
		rom_mesg_s.dirty = 1;
	}
	else
		rom_mesg_s.dirty = 0;


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

	if(cache.id == id_tmp)
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

	if(cache.status != 3)
		cache.status = 1;

	return FLASH_OK;

}

DEFAULT_RETURN_T update_rom_mesg(rom_mesg_t *mymesg)
{
	u32 i;

	if(cache.status != 3)
		return FLASH_ERROR9;

	if(mymesg->dirty == 0)
		return FLASH_STATUS4 ;

	cache.id = 0;

	mem_set(cache.cache,0xff,SPIFLASH_SECTOR_SIZE);
	cache_write(INROM_MAGIC_OFFSET, sizeof(rom_mesg_t)-4, (u8 *)mymesg);
	cache_write(INROM_ERASE_OFFSET, INROM_ERASE_SIZE, erase);

	mymesg->dirty == 0;
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
	u32 i;
	linkhead_t linkhead_s_tmp;

	spi_flash_erase_all();

	for(i = 0; i < INROM_ERASE_SIZE ; i++)
	{
		erase[i] = 0xff;
	}

	erase[0] = 0xfe;

	linkhead_s_tmp.addr = SPIFLASH_FS_DATA_SADDR;
	linkhead_s_tmp.next = NEXT_NULL;
	//linkhead_s_tmp.size = 0x800000 - 0x1000 - INROM_LIST_SIZE;
	linkhead_s_tmp.size = SPIFLASH_SIZE - 0x1000;
	update_link_head(&linkhead_s_tmp);
	cache_store();

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

	/* init cache */
	cache.id = NULL;
	cache.status = NULL;

	/* init updatelink */
	for(i = 0; i < UPDATE_NUM; i++)
	{
		updatelink_buf[i].id = NULL;
	}
	updatelink_num = 0;

	/* init erase */
	spi_flash_read_data(INROM_ERASE_OFFSET,  erase, INROM_ERASE_SIZE);

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



	return FLASH_OK;
}
/*
 * this func is API, for store list.
 *
 */
extern int abc;
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

	node_tmp.size = mylen + INROM_LIST_SIZE;
	node_p_tmp = malloc_node(&node_tmp);

	if( !node_p_tmp)
		return NULL;

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


	p = &cache.cache[offset_tmp + INROM_LIST_DATA_OFFSET];

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

	abc ++;/* temp code for test, after dele*/

	if(!rom_mesg_s.vhead)
	{
		rom_mesg_s.vtail = node_tmp.addr;
		rom_mesg_s.vhead = node_tmp.addr;
		//rom_mesg_s.dirty = 1;
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


		p = &cache.cache[offset_tmp + INROM_LIST_DATA_OFFSET];

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

	rom_mesg_s.vnum --;
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
			flash_data_read(linkhead_tmp->addr + INROM_LIST_DATA_OFFSET,mylen, &cache.cache[0x800]);
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

void spiflash_final_update(void)
{
	if(cache.status == 1)
		cache_store();
	if(updatelink_num != 0)
	{
		cache_load(updatelink_buf[0].id);
		cache_store();
	}
	if(rom_mesg_s.dirty == 1)
	{
		cache.status = 3;
		update_rom_mesg(&rom_mesg_s);
		cache_store();
	}
}
