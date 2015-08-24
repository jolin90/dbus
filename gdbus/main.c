#include<glib.h>
static void test_function(gpointer data, gpointer user_data)
{
        int i;
        i = GPOINTER_TO_INT(data);
        g_print("test %d\n",i);
}

int main()
{
        GThreadPool *pool = NULL;
        GError *error = NULL;
        int i,gthreadcount;
        GMutex *mutex;
        if ( g_thread_supported () )
                printf("support g_thread\n");
        g_thread_init (NULL);
        mutex = g_mutex_new();
        pool = g_thread_pool_new(test_function,NULL,-1,FALSE,&error);
        if(pool == NULL) {
                g_print("can not create thread");
        }
        gthreadcount = g_thread_pool_get_num_threads(pool);
        g_print("%d\n",gthreadcount);

        g_mutex_lock(mutex);
        for(i = 0; i < 10 ; i++)
        {
                g_thread_pool_push(pool, (gpointer *)i , NULL);
        }
        g_mutex_unlock(mutex);
}