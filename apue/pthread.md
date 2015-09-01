###difference between return and exit
- return is an instruction of the language that returns from a function call
- exit is a system call that terminates the current process
- in main function return a integer and call exit with parameter integer is equivalent.
- atexit can register functions that will called by exit(return in main has same effect) in stack order.

###pthread
- create a pthread, put tid into tidp

        int pthread_create(pthread_t *restrict tidp,
            const pthread_attr_t *restrict attr,
            void *(*start_rtn)(void *), void *restrict arg);
- thread terminates itself  

        void pthread_exit(void *rval_ptr);

- another thread in process wait for thread with tid "thread" terminate, and acquire terminating state

        void pthread_join(pthread_t thread, void **rval_ptr);

- just like exit system call, we can use a pair of functions to register and run cleanup functions

        void pthread_cleanup_push(void (*rtn)(void *), void *arg);
        void pthread_cleanup_pop(int execute);
- mutex

        pthread_mutex_init
        pthread_mutex_destroy
        pthread_mutex_lock | pthread_mutex_trylock
        pthread_mutex_unlock
- rwlock rwlock has three mode: lock in read mode, lock in write mode, unlock. Only one thread can possesses write lock, however read lock can possessed by multi threads.

        pthread_rwlock_init
        pthread_rwlock_destroy
        pthread_rwlock_rdlock | pthread_rwlock_tryrdlock
        pthread_rwlock_wrlock | pthread_rwlock_trywrlock
        pthread_rwlock_unlock

- condition

        pthread_cond_init
        pthread_cond_destroy

        // the following four function should cooperated with mutex. like this
        // in one thread:
        // pthread_mutex_lock(mutex);
        // while(condition not satisfy) {
        //     pthread_cond_wait(cond, mutex);
        // }
        // pthread_mutex_unlock(mutex);
        // in another thread:
        // pthread_mutex_lock(mutex);
        // some code to change condition
        // pthread_mutex_unlock(mutex);
        // pthread_cond_signal(...);
        pthread_cond_wait(cond, mutex);
        pthread_cond_timedwait(cond, mutex, timeout);
        pthread_cond_signal // wake one thread
        pthread_cond_broadcast // wake all threads
        // as why need lock while call cond_wait, for one reason to protect
        // conditional variable operated by cond_wait before block happened,
        // another reason is to prevent signal happens between while
        // directive's condition judge and call to pthread_cond_wait, such
        // race condition will make the signal miss.
- pthread_spin_lock 自旋锁，在进入阻塞队列之前先跑几个循环，然后再去尝试获取锁，直到自旋的次数超过阈值，才进入阻塞队列，此时才切换状态
