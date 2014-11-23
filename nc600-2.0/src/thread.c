//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   thread.c
/// @brief  线程库函数
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



#include "thread.h"
#include <stdio.h>
#include "log.h"

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
int trd_create(TRD_t *trd_id, void *(*func)(void *), void *arg)
{
    int ret = -1;

    ret = pthread_create(trd_id, NULL, func, arg);
	

    if (ret != 0)
    {
        perror("trd_create: pthread_create");
        return -1;
    }

    ret = pthread_detach(*trd_id);

    if (ret != 0)
    {
        perror("trd_create: pthread_detach");
        return -2;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////
///
///     互斥量初始化
///     @param *lock
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
inline void trd_lock_init(LOCK_t *lock)
{
    pthread_mutex_init(lock, NULL);
}


