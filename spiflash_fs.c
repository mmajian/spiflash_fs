node_t *malloc_node(node_t *mynode)
{
	linkhead_t linkhead_tmp;
	u8 ok = 0;
	node_size_t size_tmp;
	updatelink_t *updatelink_tmp;

	if((mynode == NULL )|| (mynode->size == NULL))
			return ERROR1;

	linkhead_tmp = read_link_head(rlink_s->head, &linkhead_tmp);
	rlink_s->pro = linkhead_tmp->next;

	if(mynode->size < SPIFLASH_IGNORE_SIZE)
	{
		mynode->size = SPIFLASH_IGNORE_SIZE;
	}
	if(mynode->size | SPIFLASH_LINK_HEADSIZE_MASK)
	{
		mynode->size = (mynode->size + SPIFLASH_IGNORE_SIZE) | ~(SPIFLASH_LINK_HEADSIZE_MASK)
	}

	while(linkhead_tmp != NULL)
	{

		if(mynode->size <= linkhead->size)
		{
			ok = 1;
		}

		rlink_s->pro = linkhead_tmp->addr;
		read_link_head(linkhead_tmp->next, &linkhead_tmp);
	}

	if(ok == 0)
		return ERROR2;

	size_tmp = linkhead->size - mynode->size;

	if(size_tmp  >= SPIFLASH_IGNORE_SIZE)
	{
		if(rlink_s->pro == rlink_s->head)
		{
			rlink_s->head = linkhead_tmp->addr - mynode->size;

		}
		else
		{
			updatelink_tmp->addr = rlink_s->pro;
			updatelink_tmp->next = rlink_s->pro - mynode->size - SPIFLASH_LINK_HEADSIZE;
			update_link_haead(&updatelink_tmp,1);
		}

		updatelink_tmp->addr = rlink_s->pro;
		updatelink_tmp->next = linkhead_tmp->next;
		updatelink_tmp->size = size_tmp;
		updatelink_tmp->date = NULL;

		update_link_haead(&updatelink_tmp,0);

		mynode->addr = linkhead_tmp->addr;
		mynode->size = mynode->size;
	}
	else
	{
		if(rlink_s->pro == rlink_s->head)
		{
			rlink_s->head = linkhead_tmp->addr - mynode->size - SPIFLASH_LINK_HEADSIZE;

		}
		else
		{
			updatelink_tmp->addr = rlink_s->pro;
			updatelink_tmp->next = linkhead_tmp->next;
			update_link_haead(&updatelink_tmp,1);
		}

		mynode->addr = linkhead_tmp->addr;
		mynode->size = linkhead_tmp->size;

	}

	return mynode;

}

u8 free_node(node_t *mynode)
{
	linkhead_t *linkhead_tmp;
	linkhead_t *linkheadpre_tmp;
	u8 ok = 0;
	node_size_t size_tmp;
	linkhead_t updatelink_tmp_tmp;

	linkhead_tmp = read_link_head(rlink_s->head, &linkhead_tmp);
	linkheadpre_tmp = linkhead_tmp;

	if((mynode == NULL )|| (mynode->size == NULL))
			return ERROR1;

	while(linkhead_tmp != NULL)
	{
		if(mynode->addr < linkhead_tmp->addr)
		{
			ok = 1;
			break;
		}

		linkheadpre_tmp = linkhead_tmp;
		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_tmp);

	}

		if(ok == 0)
		{
			if(mynode->addr == (linkheadpre_tmp->size + linkheadpre_tmp->addr))
			{
				updatelink_tmp->addr = linkheadpre_tmp->addr;
				updatelink_tmp->size = linkheadpre_tmp->size + mynode->size;
				update_link_haead(&updatelink_tmp,1);
			}
			else
			{
				updatelink_tmp->addr = mynode->addr;
				updatelink_tmp->size = mynode->size;
				update_link_haead(&updatelink_tmp,1);

				updatelink_tmp->addr = linkheadpre_tmp->addr;
				updatelink_tmp->next = mynode->addr;
				update_link_haead(&updatelink_tmp,1);
			}
		}
		else
		{
			if(linkheadpre_tmp == linkhead_tmp)
			{
				if(linkhead_tmp->addr == (mynode->size + mynode->addr))
				{
					updatelink_tmp->addr = mynode->addr;
					updatelink_tmp->size = linkhead_tmp->size + mynode->size;
					updatelink_tmp->next = linkhead_tmp->next;
					update_link_haead(&updatelink_tmp,1);
				}
				else
				{
					updatelink_tmp->addr = mynode->addr;
					updatelink_tmp->size = mynode->size;
					updatelink_tmp->next = linkheadpre_tmp->addr;
					update_link_haead(&updatelink_tmp,1);
				}

				rlink_s->head =  mynode->addr;
			}
			else
			{
				if(linkhead_tmp->addr == (mynode->size + mynode->addr))
				{
					updatelink_tmp->addr = mynode->addr;
					updatelink_tmp->size = linkhead_tmp->size + mynode->size;
					updatelink_tmp->next = linkhead_tmp->next;
					update_link_haead(&updatelink_tmp,1);
				}
				else
				{
					updatelink_tmp->addr = mynode->addr;
					updatelink_tmp->size = mynode->size;
					updatelink_tmp->next = linkheadpre_tmp->addr;
					update_link_haead(&updatelink_tmp,1);
				}

				if(mynode->addr == (linkheadpre_tmp->size + linkheadpre_tmp->addr))
				{
					updatelink_tmp->addr = linkheadpre_tmp->addr;
					updatelink_tmp->size = linkheadpre_tmp->size + mynode->size;
					update_link_haead(&updatelink_tmp,1);
				}
				else
				{
					updatelink_tmp->addr = mynode->addr;
					updatelink_tmp->size = mynode->size;
					update_link_haead(&updatelink_tmp,1);

					updatelink_tmp->addr = linkheadpre_tmp->addr;
					updatelink_tmp->next = mynode->addr;
					update_link_haead(&updatelink_tmp,1);
				}
			}
		}

		return 0;
}
inline node_id_t  get_cache_id(void)
{
		return cache->id;
}
void update_link_haead(linkhead_t *myupdatelink,1);
{
	node_id_t id_tmp;
	node_offset_t offset_tmp;

	id_tmp = (myupdatelink->addr | (~SPIFLASH_SECTOR_SIZE_MASK)) >> SPIFLASH_SECTOR_OFFSET;
	if(id_tmp == get_cache_id())
	{
		offset_tmp = myupdatelink->addr | SPIFLASH_SECTOR_SIZE_MASK;
		if(myupdatelink->next)
			cache_write(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
		if(myupdatelink->size)
			cache_write(node_offset_t myaddr, node_len_t mylen, u8 *mydata);
	}
}








