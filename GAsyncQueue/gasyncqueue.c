#include <stdio.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

GAsyncQueue* asyncQueue;

int g_async_queue_push_msg()
{
    char *buff = (char *)malloc(255);

    strncpy(buff, "12345", strlen("12345"));

    g_async_queue_push(asyncQueue, (void*)buff);

    printf("\033[31m jolin log   : \033[0m" "%s %s %d %p\n", __FILE__, __func__, __LINE__, buff);

    return 0;
}

int g_async_queue_pop_msg()
{
    unsigned char *buff;

    buff = (unsigned char *)g_async_queue_pop(asyncQueue);

    printf("\033[31m jolin log   : \033[0m" "%s %s %d %p\n", __FILE__, __func__, __LINE__, buff);
    free(buff);

    return 0;
}

int main(int argc, char *argv[])
{
    asyncQueue = g_async_queue_new();

    g_async_queue_push_msg();
    g_async_queue_pop_msg();

    return 0;
}
