/**************************************************************************
  * Description
  *   This file declare read write lock structure and operating functions.
  *
  * Remarks
  *   The rw lock is a simple combination of a mutex and a semaphore.
  *
  * Bugs
  *   \None.
  *
  * TODO
  *     * In i386 arch, optimization using assemble language will enhance performance.
  *     *
  * History
  *   <table>
  *   \Author      Date         Change Description
  *   -----------  -----------  -------------------
  *   Zhengdao Li  2007.01.24   Initialize.
  *   </table>
  *
  **************************************************************************/
#ifndef __RW_SEMA_H__
#define __RW_SEMA_H__
#include <sys_define.h>
#include <osal/osal.h>

enum {
	RW_LOCK_MAX_WAIT_THREAD			= 16, 	/* maximum waiting thread number */
};

struct rw_lock {
	OSAL_MUTEX_ID barrier;				/* barrier to operate on counting semaphore */
	OSAL_SEMA_ID count;					/* counting semaphore */
};

/*************************************************************************
  * Description:
  *   enter read condition.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *************************************************************************/
static __inline__ void down_read(struct rw_lock *rw)
{
	OsalMutexLock(rw->barrier);
	osal_sema_wait(rw->count, OSAL_WAIT_FOREVER);
	OsalMutexUnlock(rw->barrier);
}

/*************************************************************************
  * Description:
  *   leave read condition.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *************************************************************************/
static __inline__ void up_read(struct rw_lock *rw)
{
	osal_sema_post(rw->count);
}

/*************************************************************************
  * Description:
  *   enter write condition.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *************************************************************************/
static __inline__ void down_write(struct rw_lock *rw)
{
	KV_S32 count = RW_LOCK_MAX_WAIT_THREAD;
	
	OsalMutexLock(rw->barrier);
	while (count--) {
		osal_sema_wait(rw->count, OSAL_WAIT_FOREVER);
	}
	OsalMutexUnlock(rw->barrier);
}

/*************************************************************************
  * Description:
  *   leave write condition.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *   
  * Return Value List:
  *   OSAL_E_OK :   success.
  *   other :  failure.                          
  *************************************************************************/
static __inline__ void up_write(struct rw_lock *rw)
{
	KV_S32 count = RW_LOCK_MAX_WAIT_THREAD;
	while(count--) {
		osal_sema_post(rw->count);
	}
}

/*************************************************************************
  * Description:
  *   create a read write lock.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *   
  * Return Value List:
  *   OSAL_E_OK :   success.
  *   other :  failure.                          
  *************************************************************************/
static __inline__ KV_S32 rw_lock_create(struct rw_lock *rw)
{
	OsalMutexCreate(&rw->barrier);
	return osal_sema_create(&rw->count, RW_LOCK_MAX_WAIT_THREAD);
}

/*************************************************************************
  * Description:
  *   destroy a read write lock.
  *   
  * Parameters:
  *   rw :  the read write lock.
  *   
  * Return Value List:
  *   OSAL_E_OK :   success.
  *   other :  failure.                          
  *************************************************************************/
static __inline__ KV_S32 rw_lock_destroy(struct rw_lock *rw)
{
	OsalMutexDestroy(rw->barrier);
	return osal_sema_destroy(rw->count);
}

#endif /* __RW_SEMA_H__ */

