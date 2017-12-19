#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

//using namespace std;

#define COUNT_SEC 10
#define COUNT_USEC 0

/*Initialize timer*/
static void Init_timer(struct itimerval *tick)
{
    int ret;
    memset((void*)tick, 0, sizeof(struct itimerval));

    /*initialize it_value*/
    tick->it_value.tv_sec = COUNT_SEC;
    tick->it_value.tv_usec = COUNT_USEC;

    /*initialize it_interval*/
    tick->it_interval.tv_sec = COUNT_SEC;
    tick->it_interval.tv_usec = COUNT_USEC;

    return;
}

/*Callback function when time's up*/
static void Timeout_callback(int sig)
{
    printf("timeout callback function\n");
    return;
}

int main(void)
{
    int ret = 0;
    struct itimerval tick;

    signal(SIGALRM, Timeout_callback);

    Init_timer(&tick);

    ret = setitimer(ITIMER_REAL, &tick, NULL);
    if(ret < 0)
    {
        printf("setitimer failed!\n");
        return -1;
    }

    while(1);

    return 0;
}
