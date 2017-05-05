/***************************************************************************
  * Description
  *   This file defines red-black tree functions.
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
#ifndef __RBTREE_H__
#define __RBTREE_H__

#include <kv_type.h>
#include <util/mem_mgr.h>

#if 1
	#include <libc/stdio.h>
	#define RBTREE_PRINTF					LOG_PRINTF
#else
	#define RBTREE_PRINTF(x...)				do{}while(0)	
#endif




#define rbtree_first(n) (n->flink)
#define rbtree_last(n) (n->blink)
#define rbtree_next(n) (n->flink)
#define rbtree_prev(n) (n->blink)
#define rbtree_empty(t) (t->flink == t)
#define rbtree_nil(t) (t)
#define rbtree_val(n) (n->val)

 
#define rbtree_traverse(ptr, lst) \
  for(ptr = rbtree_first(lst); ptr != rbtree_nil(lst); ptr = rbtree_next(ptr))
 
#define rbtree_rtraverse(ptr, lst) \
  for(ptr = rbtree_last(lst); ptr != rbtree_nil(lst); ptr = rbtree_prev(ptr))

typedef struct __rbtree_node
{
	unsigned char red;
	unsigned char internal;
	unsigned char left;
	unsigned char roothead;	 /* (bit 1 is root, bit 2 is head) */
	struct __rbtree_node *flink;
	struct __rbtree_node *blink;
	struct __rbtree_node *parent;
	KV_U32 key;
	KV_U32 val;
} rbtree_node, *rbtree;

typedef KV_U8* (*node_alloc_t)(KV_U32 priv, KV_U32 size);
typedef void (*node_free_t)(KV_U32 priv, KV_U8* buffer, KV_U32 size);

typedef struct rbtree_node_alloc
{
	node_alloc_t	node_alloc;
	node_free_t  node_free;
	KV_U32 priv;
} rbtree_node_alloc_t;




/**
 * Create a red black tree
 * 
 * @return rbtree
 */
rbtree rbtree_create(mem_mgr_t* alloc);

/** 
 * 
 * 
 * @param tree
 */
void rbtree_destroy(rbtree tree, mem_mgr_t* alloc);

/**
 * 
 * @param tree
 * @param key
 * @param val
 * @param less
 * 
 * @return rbtree_node*
 */
rbtree_node* rbtree_insert(rbtree tree, KV_U32 key, KV_U32 val, 
						   int (*compare)(KV_U32 key1, KV_U32 key2), mem_mgr_t* alloc);

/** 
 * 
 * 
 * @param tree
 * @param key
 * @param val
 * @param compare
 * 
 * @return rbtree_node*
 */
rbtree_node* rbtree_insert_unique(rbtree tree, KV_U32 key, KV_U32 val, 
								  int (*compare)(KV_U32 key1, KV_U32 key2), mem_mgr_t* alloc, rbtree_node** exist);

/**
 * 
 * @param tree
 * @param key
 * @param less
 * @param found
 * 
 * @return rbtree_node*
 */
rbtree_node* rbtree_find_greater(rbtree tree, KV_U32 key, int (*compare)(KV_U32 key1, KV_U32 key2), KV_S32* found);

/**
 * 
 * @param tree
 * @param key
 * @param less
 * 
 * @return rbtree_node*
 */
rbtree_node* rbtree_find(rbtree tree, KV_U32 key, int (*compare)(KV_U32 key1, KV_U32 key2));

/**
 * 
 * @param node
 */
void rbtree_delete_node(rbtree_node* node, mem_mgr_t* alloc);

/**
 * 
 * @param tree
 */
void rbtree_free(rbtree tree, mem_mgr_t* alloc);

/**
 * 
 * @param node
 * 
 * @return KV_U32
 */
/*KV_U32 rbtree_val(rbtree_node* node);*/

/**
 * 
 * @param node
 * 
 * @return KV_U32
 */
KV_U32 rbtree_nblack(rbtree_node* node);

/**
 * 
 * @param node
 * 
 * @return KV_U32
 */
KV_U32 rbtree_plength(rbtree_node* node);

/** 
 * 
 * 
 * @param n
 * @param value
 * 
 * @return rbtree_node*
 */
rbtree_node* rbtree_find_value(rbtree_node* n, KV_U32 value);


#endif /*__RBTREE_H__*/
