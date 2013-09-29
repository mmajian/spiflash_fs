spiflashaddr_t malloc_node(*node_t)
{
	linkhead_t linkhead_tmp; 
	u8 ok = 0;
	node_size_t size_tmp;

	if((node_t == NULL )|| (node_t->size == NULL))
			return ERROR1;

	linkhead_tmp = read_link_head(rlink_s->head, &linkhead_tmp);
	rlink_s->pro = linkhead_tmp->addr;

	while(linkhead_tmp != NULL)
	{

		if(node_t->size <= linkhead->size)
		{
			ok = 1;
		}

		rlink_s->pro = linkhead_tmp->addr;
		linkhead_tmp = read_link_head(linkhead_tmp->next, &linkhead_tmp);
	}

	if(ok == 0)
		return ERROR2;

	size_tmp = linkhead->size - node_t->size;

	if(size_tmp  >= SPIFLASH_IGNORE_SIZE)
	{
		if(rlink_s->pro == rlink_s->head)
		{
			rlink_s->head = linkhead_tmp->addr - node_t->size;
		}

		node_t->addr = linkhead_tmp->addr;
		node_t->size = linkhead_tmp->size;
	}
	else
	{
	}

}
