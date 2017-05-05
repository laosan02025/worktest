/***************************************************************************
  * Description
  *   This file implement red-black tree functions.
  *
  * History
  *   <table>
  *   \Author      Date         Change Description
  *   -----------  -----------  -------------------
  *   Charle Fu    2007.07.10   Initialize.
  *   </table>
  *
  * TODO
  *	
  ***************************************************************************/
#include <kv_type.h>
#include <libc/stdlib.h>
#include <libc/string.h>

#define RBT_DEBUG_LEVEL					0
#include <algo/rbtree.h>

#define isred(n) (n->red)
#define isblack(n) (!isred(n))
#define isleft(n) (n->left)
#define isright(n) (!isleft(n))
#define isint(n) (n->internal)
#define isext(n) (!isint(n))
#define ishead(n) (n->roothead & 2)
#define isroot(n) (n->roothead & 1)
#define getlext(n) ((struct __rbtree_node *)(n->key))
#define setlext(node, val) node->key = (KV_U32) (val)
#define getrext(n) ((struct __rbtree_node *)(n->val))
#define setrext(node, value) node->val = (KV_U32) (value)
#define setred(n) n->red = 1
#define setblack(n) n->red = 0
#define setleft(n) n->left = 1
#define setright(n) n->left = 0
#define sethead(n) (n->roothead |= 2)
#define setroot(n) (n->roothead |= 1)
#define setint(n) n->internal = 1
#define setext(n) n->internal = 0
#define setnormal(n) n->roothead = 0
#define sibling(n) ((isleft(n)) ? n->parent->blink : n->parent->flink)

static void insert(rbtree_node* item, rbtree_node* list)	/* Inserts to the end of a list */
{
	rbtree_node* last_node;

	last_node = list->blink;

	list->blink = item;
	last_node->flink = item;
	item->blink = last_node;
	item->flink = list;
}

static void delete_item(rbtree_node* item)		/* Deletes an arbitrary iterm */
{
	item->flink->blink = item->blink;
	item->blink->flink = item->flink;
}

static void single_rotate(rbtree_node* y, int l)
{
	int rl = 0, ir;
	rbtree_node *x, *yp;

	ir = isroot(y);
	yp = y->parent;
	if (!ir)
	{
		rl = isleft(y);
	}

	if (l)
	{
		x = y->flink;
		y->flink = x->blink;
		setleft(y->flink);
		y->flink->parent = y;
		x->blink = y;
		setright(y);  
	}
	else
	{
		x = y->blink;
		y->blink = x->flink;
		setright(y->blink);
		y->blink->parent = y;
		x->flink = y;
		setleft(y);  
	}

	x->parent = yp;
	y->parent = x;
	if (ir)
	{
		yp->parent = x;
		setnormal(y);
		setroot(x);
	}
	else
	{
		if (rl)
		{
			yp->flink = x;
			setleft(x);
		}
		else
		{
			yp->blink = x;
			setright(x);
		}
	}
}

static void recolor(rbtree_node* n)
{
	rbtree_node *p, *gp, *s;
	int done = 0;

	while (!done)
	{
		if (isroot(n))
		{
			setblack(n);
			return;
		}

		p = n->parent;

		if (isblack(p))	return;

		if (isroot(p))
		{
			setblack(p);
			return;
		}

		gp = p->parent;
		s = sibling(p);
		if (isred(s))
		{
			setblack(p);
			setred(gp);
			setblack(s);
			n = gp;
		}
		else
		{
			done = 1;
		}
	}
	/* p's sibling is black, p is red, gp is black */

	if ((isleft(n) == 0) == (isleft(p) == 0))
	{
		single_rotate(gp, isleft(n));
		setblack(p);
		setred(gp);
	}
	else
	{
		single_rotate(p, isleft(n));
		single_rotate(gp, isleft(n));
		setblack(n);
		setred(gp);
	}
}


static void mk_new_ext(rbtree_node* node, KV_U32 key, KV_U32 val)
{
	node->val = val;
	node->key = key;
	setext(node);
	setblack(node);
	setnormal(node);
}

static void mk_new_int(rbtree_node* newnode, rbtree_node* l, rbtree_node* r, rbtree_node* p, int il)
{
	setint(newnode);
	setred(newnode);
	setnormal(newnode);
	newnode->flink = l;
	newnode->blink = r;
	newnode->parent = p;
	setlext(newnode, l);
	setrext(newnode, r);
	l->parent = newnode;
	r->parent = newnode;
	setleft(l);
	setright(r);
	if (ishead(p))
	{
		p->parent = newnode;
		setroot(newnode);
	}
	else if (il)
	{
		setleft(newnode);
		p->flink = newnode;
	}
	else
	{
		setright(newnode);
		p->blink = newnode;
	}
	recolor(newnode);

}  


static rbtree_node* lprev(rbtree_node* n)
{
	if (ishead(n)) return n;
	while (!isroot(n))
	{
		if (isright(n))	return n->parent;
		n = n->parent;
	}
	return n->parent;
}

static rbtree_node* rprev(rbtree_node* n)
{
	if (ishead(n)) return n;
	while (!isroot(n))
	{
		if (isleft(n)) return n->parent;
		n = n->parent;
	}
	return n->parent;
}

static rbtree_node* rbtree_insert_b(rbtree_node* n, KV_U32 key, KV_U32 val, mem_mgr_t* alloc)
{
	rbtree_node* newleft, *newright, *newnode, *p;  
	KV_U32 actual;

	if (ishead(n))
	{
		if (n->parent == n)
		{		  /* Tree is empty */
			newnode = (rbtree_node*) alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);

			if (newnode != NULL)
			{
				mk_new_ext(newnode, key, val);
				insert(newnode, n);
				n->parent = newnode;
				newnode->parent = n;
				setroot(newnode);
			}
			return newnode;
		}
		else
		{
			newright = (rbtree_node*) alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);

			if (newright != NULL)
			{
				newnode = (rbtree_node*) alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);
				if(newnode != NULL)
				{
					mk_new_ext(newright, key, val);
					insert(newright, n);
					newleft = newright->blink;
					setnormal(newleft);
					mk_new_int(newnode, newleft, newright, newleft->parent, isleft(newleft));
					p = rprev(newright);
					if (!ishead(p))	setlext(p, newright);
				}
				else
				{
					alloc->mem_free(alloc, (KV_U8*)newright, sizeof(rbtree_node));
					newright = NULL;
				}
			}
			return newright;
		}
	}
	else
	{
		newleft = (rbtree_node*) alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);

		if (newleft != NULL)
		{
			newnode = (rbtree_node*) alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);
			if(newnode != NULL)
			{
				mk_new_ext(newleft, key, val);
				insert(newleft, n);
				setnormal(n);
				mk_new_int(newnode, newleft, n, n->parent, isleft(n));
				p = lprev(newleft);
				if (!ishead(p))	setrext(p, newleft);
			}
			else
			{
				alloc->mem_free(alloc, (KV_U8*)newleft, sizeof(rbtree_node));
				newleft = NULL;
			}
		}
		return newleft;    
	}

}

static int compare_local(KV_U32 key1, KV_U32 key2)
{
	if (key1 > key2)
	{
		return 1;
	}
	else if (key1 == key2)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


rbtree rbtree_create(mem_mgr_t* alloc)
{
	rbtree_node* head = NULL;
	KV_U32 actual = 0;

	head = (rbtree_node*)alloc->mem_alloc(alloc, sizeof(struct __rbtree_node), &actual);

	if (head != NULL)
	{
		head->flink = head;
		head->blink = head;
		head->parent = head;
		head->key = 0;
		sethead(head);
	}

	return head;
}


rbtree_node* rbtree_insert(rbtree tree, KV_U32 key, KV_U32 val, 
						   int (*compare)(KV_U32 key1, KV_U32 key2), mem_mgr_t* alloc)
{
	KV_S32 find = 0;

	rbtree_node* n = rbtree_find_greater(tree,key,compare,&find);

	return rbtree_insert_b(n, key, val, alloc);
}

rbtree_node* rbtree_insert_unique(rbtree tree, KV_U32 key, KV_U32 val, 
								  int (*compare)(KV_U32 key1, KV_U32 key2), mem_mgr_t* alloc, rbtree_node** exist)
{
	KV_S32 find = 0;

	rbtree_node* n = rbtree_find_greater(tree,key,compare,&find);

	if (!find)
	{
		if(exist != NULL) {
			*exist = NULL;
		}
		return rbtree_insert_b(n,key,val, alloc);
	}
	else
	{
		if(exist != NULL) {
			*exist = n;
		}
		return NULL;
	}
}

rbtree_node* rbtree_find_greater(rbtree tree, KV_U32 key, int (*compare)(KV_U32 key1, KV_U32 key2), KV_S32* found)
{
	int cmp = 0;
	rbtree_node* n = tree;

	*found = 0;
	ASSERT(n != NULL && ishead(n));

	if (n->parent == n)	return n;
	if (compare == NULL)
	{
		compare = compare_local;
	}
	cmp = (*compare)(key, n->blink->key);
	if (cmp == 0)
	{
		*found = 1;
		return n->blink; 
	}
	if (cmp > 0) return n;
	else n = n->parent;

	while (1)
	{
		if (isext(n)) return n;
		cmp = (*compare)(key, getlext(n)->key);
		if (cmp == 0)
		{
			*found = 1;
			return getlext(n);
		}
		if (cmp < 0) n = n->flink ;
		else n = n->blink;
	}
}

rbtree_node* rbtree_find(rbtree tree, KV_U32 key, int (*compare)(KV_U32 key1, KV_U32 key2))
{
	KV_S32 fnd;
	rbtree_node* j;

	j = rbtree_find_greater(tree, key, compare, &fnd);
	if (fnd != 0)
		return j;
	else
		return rbtree_nil(tree);
}

void rbtree_delete_node(rbtree_node* node, mem_mgr_t* alloc)
{
	rbtree_node *s, *p, *gp, *n;
	unsigned char ir;

	ASSERT(node != NULL && !isint(node) && !ishead(node));

	delete_item(node); /* Delete it from the list */
	p = node->parent;  /* The only node */
	if (isroot(node))
	{
		p->parent = p;
		alloc->mem_free(alloc, (KV_U8*)node, sizeof(rbtree_node));
		node = NULL;
		return;
	}

	s = sibling(node);	  /* The only node after deletion */
	if (isroot(p))
	{
		s->parent = p->parent;
		s->parent->parent = s;
		setroot(s);
		alloc->mem_free(alloc, (KV_U8*)p, sizeof(rbtree_node));
		p = NULL;
		alloc->mem_free(alloc, (KV_U8*)node, sizeof(rbtree_node));
		node = NULL;
		return;
	}

	gp = p->parent;	 /* Set parent to sibling */
	s->parent = gp;
	if (isleft(p))
	{
		gp->flink = s;
		setleft(s);
	}
	else
	{
		gp->blink = s;
		setright(s);
	}
	ir = isred(p);
	alloc->mem_free(alloc, (KV_U8*)p, sizeof(rbtree_node));
	p = NULL;
	alloc->mem_free(alloc, (KV_U8*)node, sizeof(rbtree_node));
	node = NULL;

	if (isext(s))
	{	   /* Update proper rext and lext values */
		p = lprev(s); 
		if (!ishead(p))	setrext(p, s);
		p = rprev(s);
		if (!ishead(p))	setlext(p, s);
	}
	else if (isblack(s))
	{
		ASSERT(0); /*Should not be this*/
	}
	else
	{
		p = lprev(s);
		if (!ishead(p))	setrext(p, s->flink);
		p = rprev(s);
		if (!ishead(p))	setlext(p, s->blink);
		setblack(s);
		return;
	}

	if (ir)	return;

	/* Recolor */

	n = s;
	p = n->parent;
	s = sibling(n);
	while (isblack(p) && isblack(s) && isint(s) && 
		   isblack(s->flink) && isblack(s->blink))
	{
		setred(s);
		n = p;
		if (isroot(n)) return;
		p = n->parent;
		s = sibling(n);
	}

	if (isblack(p) && isred(s))
	{  /* Rotation 2.3b */
		single_rotate(p, isright(n));
		setred(p);
		setblack(s);
		s = sibling(n);
	}

	{ 
		rbtree_node *x, *z; 
		unsigned char il;

		if (isext(s))
		{
			ASSERT(0);/*sibling not internal*/
		}

		il = isleft(n);
		x = il ? s->flink : s->blink ;
		z = sibling(x);

		if (isred(z))
		{  /* Rotation 2.3f */
			single_rotate(p, !il);
			setblack(z);
			if (isred(p)) setred(s);
			else setblack(s);
			setblack(p);
		}
		else if (isblack(x))
		{	/* Recoloring only (2.3c) */
			if (isred(s) || isblack(p))
			{
				ASSERT(0);/*2.3c not quite right*/
			}
			setblack(p);
			setred(s);
			return;
		}
		else if (isred(p))
		{ /* 2.3d */
			single_rotate(s, il);
			single_rotate(p, !il);
			setblack(x);
			setred(s);
			return;
		}
		else
		{  /* 2.3e */
			single_rotate(s, il);
			single_rotate(p, !il);
			setblack(x);
			return;
		}
	}
}

void rbtree_free(rbtree tree, mem_mgr_t* alloc)
{
	rbtree_node* n = tree;

	ASSERT(n != NULL && ishead(n));

	while (rbtree_first(n) != rbtree_nil(n))
	{
		rbtree_delete_node(rbtree_first(n), alloc);
	}
}

void rbtree_destroy(rbtree tree, mem_mgr_t* alloc)
{
	rbtree_free(tree, alloc);
	alloc->mem_free(alloc, (KV_U8*)tree, sizeof(rbtree_node));
	tree = NULL;
}

/*KV_U32 rbtree_val(rbtree_node* node)
{
	ASSERT(node != NULL);
	return node->val;
}*/

KV_U32 rbtree_nblack(rbtree_node* node)
{
	int nb = 0;

	ASSERT(node != NULL && !ishead(node) && !isint(node));

	while (!ishead(node))
	{
		if (isblack(node)) nb++;
		node = node->parent;
	}
	return nb;
}

KV_U32 rbtree_plength(rbtree_node* node)
{
	int pl = 0;

	ASSERT(node != NULL && !ishead(node) && !isint(node));

	while (!ishead(node))
	{
		pl++;
		node = node->parent;
	}
	return pl;
}

#if (RBT_DEBUG_LEVEL>0)

void rbtree_print_tree(rbtree t, int level)
{
	int i;
	if (ishead(t) && t->parent == t)
	{
		RBT_PRINTF("tree 0x%x is empty\n", t);
	}
	else if (ishead(t))
	{
		RBT_PRINTF("Head: 0x%x.  Root = 0x%x\n", t, t->parent);
		rbtree_print_tree(t->parent, 0);
	}
	else
	{
		if (isext(t))
		{
			for (i = 0; i < level; i++)	putchar(' ');
			RBT_PRINTF("Ext node 0x%x: %c,%c: p=0x%x, k=%d\n", 
					   t, isred(t)?'R':'B', isleft(t)?'l':'r', t->parent, t->key);
		}
		else
		{
			rbtree_print_tree(t->flink, level+2);
			rbtree_print_tree(t->blink, level+2);
			for (i = 0; i < level; i++)	putchar(' ');
			RBT_PRINTF("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%d,%d)\n", 
					   t, isred(t)?'R':'B', isleft(t)?'l':'r', t->flink, 
					   t->blink, 
					   t->parent, getlext(t)->key, getrext(t)->key);
		}
	}
}
#endif

rbtree_node* rbtree_find_value(rbtree_node* n, KV_U32 value)
{
	rbtree_node* ptr = n;
	KV_U32 next = 0;

	do
	{
		if (rbtree_val(ptr) == value)
		{
			return ptr;
		}

		if (next == 0)
		{
			ptr = rbtree_prev(ptr);
			if (ptr->key != n->key)
			{
				ptr = rbtree_next(n);
				next = 1;
			}
		}
		else
		{
			ptr = rbtree_next(ptr);
			if (ptr->key != n->key)
			{
				return NULL;
			}
		}

	} while (1);
}
