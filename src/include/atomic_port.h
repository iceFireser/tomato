#ifndef __ATOMIC_PORT_H__
#define __ATOMIC_PORT_H__

#ifndef atomic_t
    #define atomic_t long
#endif

#define ATOMIC_INIT(x)         ((atomic_t)x)
#define atomic_inc(x)          __sync_add_and_fetch((x),1)  
#define atomic_dec(x)          __sync_sub_and_fetch((x),1)  
#define atomic_add(x,y)        __sync_add_and_fetch((x),(y))  
#define atomic_sub(x,y)        __sync_sub_and_fetch((x),(y))  
#define atomic_set(x, value)   __sync_lock_test_and_set(x, value)  
#define atomic_read(x)         __sync_fetch_and_xor(x, 0)   
#define atomic_add_return(x,y) __sync_add_and_fetch((y), (x))  
#define atomic_sub_return(x,y) __sync_sub_and_fetch((y), (x))


#endif
