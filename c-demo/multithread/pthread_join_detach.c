#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int g_count = 0;

void *thread_count(void *arg)
{
    //solution1: use detach
    //pthread_detach(pthread_self());
    g_count++;
    return NULL;
}

void *thread_show(void *arg)
{
    while (1) {
        printf("g_count: %d\n", g_count);
        sleep(1);
    }
    return NULL;
}


int main(int argc, char *argv[])
{
#define CHECK_RET(fun, ret) if (ret != 0) {strcpy(fun_name, #fun); goto done;}
    char fun_name[128] = {0};
    int ret;
    pthread_t tid;

    //solution3: set thread attr
    //pthread_attr_t attr;
    //pthread_attr_init(&attr);
    //pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    ret =  pthread_create(&tid, NULL, thread_show,  NULL);
    CHECK_RET(pthread_create, ret);
    while(1) {
        //solution3: use attr PTHREAD_CREATE_DETACHED
        ret = pthread_create(&tid, NULL, thread_count, NULL);
        CHECK_RET(pthread_create, ret);

        //solution2: use join
        //void *status = NULL;
        //ret = pthread_join(tid, &status); // Note: this is a block call.
        //CHECK_RET(pthred_join, ret);
    }

done:
    printf("%s ret: %d, g_count: %d\n", fun_name, ret, g_count);
    //pthread_attr_destroy(&attr); // solution3
    return -1;
#undef CHECK_RET
}

