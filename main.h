#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/signal.h>
#include <string.h>
#include <sys/ipc.h>

#define QUANTUM 1
#define PROCESS_NUM 10
#define BURSTSIZE 300
#define CALLBACK_TIME 2

#define WAITING_CPU 1
#define WATING_IO 2
#define BURST_CPU 3
#define BURST_IO 4
#define WORK_END 5


typedef struct _process{
    pid_t pid;
    int cpuBurst;
    int ioBurst;
    int status;
    int priority;
} process;

typedef struct _circularQueue{
    int front;
    int rear;
    int max;
    int num;
    process * queue[PROCESS_NUM];    
} circularQueue;

typedef struct _msgSet{
    long msgType;
    int mode;
    process * process;
} msgSet;


void child(process * pd);