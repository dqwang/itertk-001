//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   thread.h
/// @brief  线程库函数头文件
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010－04－24
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef	_TRD_H_
#define	_TRD_H_
#include <pthread.h>
#include <semaphore.h>


typedef pthread_t		TRD_t;
typedef sem_t			SEM_t;
typedef pthread_mutex_t	LOCK_t;
// for thread safe
#define TS_LOCK(lock)	pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)lock );	pthread_mutex_lock( lock )
#define TS_UNLOCK(lock)	pthread_cleanup_pop( 1 )

//////////////////////////////////////////////////////////////////////////
///
///     创建并分离线程
///     @param trd_id
///     @param *func    目标线程
///     @param *arg     函数参数
///     @return 0-成功<0 失败
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern int trd_create(TRD_t *trd_id, void *(*func)(void *), void *arg);

//////////////////////////////////////////////////////////////////////////
///
///     互斥量初始化
///     @param *lock
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
inline void trd_lock_init(LOCK_t *lock);

#endif


