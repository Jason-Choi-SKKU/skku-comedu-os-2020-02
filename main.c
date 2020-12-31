#include "main.h"
#include "queue.c"
#include "process.c"

pid_t pids[PROCESS_NUM];
process * processData[PROCESS_NUM];
struct sigaction sa;
struct itimerval timerMain, timerLast;
clock_t tickfront, tickrear;
int msgid;


void child(process * pd){
    msgSet tmpMsg;
    pid_t childPID = getpid();
    while(1){
        if(msgrcv(msgid, &tmpMsg, sizeof(msgSet), childPID, 0) != -1){
            printf("\n\n===============================================================");
            printf("\n                     Process %d Fetched!",childPID);
            printf("\n---------------------------------------------------------------");
            printf("\n                         Remain Burst\n");
            printf("\n              CPU : %d                 IO : %d",pd->cpuBurst, pd->ioBurst);
            printf("\n===============================================================");
            
            if(tmpMsg.mode == WAITING_CPU){
                if(pd->cpuBurst > QUANTUM) pd->cpuBurst -= QUANTUM;
                else pd->cpuBurst = 0;
            }
            else if(tmpMsg.mode  == WATING_IO){
                if(pd->ioBurst > QUANTUM) pd->ioBurst -= QUANTUM;
                else pd->ioBurst = 0;
            }
            
            
        }
    }
}

void burst(process * runningProcess, circularQueue * waitQ, circularQueue * runQ, int count){
    printf("\n\n===============================================================");
    if(runningProcess->status == WATING_IO){
        printf("\n             BURSTING IO || QUAMTUM NUMBER %d",count);
    }
    else if(runningProcess->status == WAITING_CPU){
        printf("\n             BURSTING CPU || QUAMTUM NUMBER %d",count);
    }
    
    printf("\n---------------------------------------------------------------");
    printf("\n                         Queue Status\n");
    printf("\n runQ(%d)  :",runQ->num);
    traverseQ(runQ);
    printf(" waitQ(%d) :",waitQ->num);
    traverseQ(waitQ);
    printf("===============================================================");
    msgSet workMsg;
    workMsg.msgType = runningProcess->pid;
    workMsg.process = runningProcess;
    workMsg.mode = runningProcess->status;

    if(runningProcess->status == WATING_IO){
        if(runningProcess->ioBurst < QUANTUM){
            tickrear = tickfront + runningProcess->ioBurst;
            runningProcess->ioBurst = 0;
        }
        else{
            runningProcess->ioBurst -= QUANTUM;
        }
    }
    else if(runningProcess->status == WAITING_CPU){
        if(runningProcess->cpuBurst < QUANTUM){
            tickrear = tickfront + runningProcess->cpuBurst;
            runningProcess->cpuBurst = 0;
        }
        else{
            runningProcess->cpuBurst -= QUANTUM;
        }
    }
      
    if(msgsnd(msgid, &workMsg, sizeof(msgSet), 0) == -1){
        printf("\nScheduling message send fail\n");
    }
}

int scheduling(){
    static int count = 0;
    static circularQueue * waitQ;
    static circularQueue * runQ;
    process * runningProcess;
    srand(time(NULL));
    if(count == 0){
        printf("\n=== Scheduling Start ===\n\n");
        waitQ = makeQ();
        runQ = makeQ();
        for(int i=0; i<PROCESS_NUM; i++){
            addQ(makeP(pids[i]),runQ);
        }

    }
    else{
        if(isEmpty(waitQ)){
            runningProcess = popQ(runQ);
            burst(runningProcess, waitQ, runQ, count);
            int tmpRand = rand();
            if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst != 0){
                if(tmpRand%2 == 0){
                    runningProcess->status = WAITING_CPU;
                    addQ(runningProcess,runQ);
                }
                else{
                    runningProcess->status = WATING_IO;
                    addQ(runningProcess, waitQ);
                }
            }
            else if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst != 0){
                runningProcess->status = WATING_IO;
                addQ(runningProcess, waitQ);
            }
            else if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst == 0){
                runningProcess->status = WAITING_CPU;
                addQ(runningProcess, runQ);
            }
            else if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst == 0){
                
            }
        }
        else if(isEmpty(runQ)){
            runningProcess = popQ(waitQ);
            burst(runningProcess, waitQ, runQ, count);
            int tmpRand = rand();
            if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst != 0){
                if(tmpRand%2 == 0){
                    runningProcess->status = WAITING_CPU;
                    addQ(runningProcess,runQ);
                }
                else{
                    runningProcess->status = WATING_IO;
                    addQ(runningProcess, waitQ);
                }
            }
            else if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst != 0){
                runningProcess->status = WATING_IO;
                addQ(runningProcess, waitQ);
            }
            else if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst == 0){
                runningProcess->status = WAITING_CPU;
                addQ(runningProcess, runQ);
            }
            else if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst == 0){
                
            }
        }
        else{
            int tmpRand1 = rand();
            if(tmpRand1%2 == 0){
                runningProcess = popQ(runQ);
                runningProcess->status = WAITING_CPU;
            }
            else{
                runningProcess = popQ(waitQ);
                runningProcess->status = WATING_IO;
            }
            burst(runningProcess, waitQ, runQ, count);
            
            if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst != 0){
                runningProcess->status = WATING_IO;
                addQ(runningProcess, waitQ);
            }
            else if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst == 0){
                runningProcess->status = WAITING_CPU;
                addQ(runningProcess, runQ);
            }
            else if(runningProcess->cpuBurst != 0 && runningProcess->ioBurst != 0){
                int tmpRand2 = rand();
                if(tmpRand2%2 == 0){
                    runningProcess->status = WAITING_CPU;
                    addQ(runningProcess, runQ);
                }
                else{
                    runningProcess->status = WATING_IO;
                    addQ(runningProcess, waitQ);
                }
            }
            else if(runningProcess->cpuBurst == 0 && runningProcess->ioBurst == 0){
                
            }
        }
        
    }
    count++;
    if(isEmpty(runQ) && isEmpty(waitQ)){
        return 1;
    }
    else{
        return 0;
    }
}


int main(){
    key_t key = ftok("~/os", 65);
    msgid = msgget(key, IPC_CREAT | 0666);
    if(msgid >0){
        printf("Message Queue Created! %d\n",msgid);
    }
    else{
        printf("Message Queue Create Failed :(\n");
    }
    int i=0;
    srand(time(NULL));
    for (i=0; i<PROCESS_NUM; i++){
        pids[i] = fork();
        if(pids[i] <0){
            printf("fork ERROR!\n");
        }
        processData[i] = makeP(pids[i]);
        if(pids[i] == 0){
            child(processData[i]);
            exit(0);
        }
    }
    

    tickfront = clock();
    int end = 0;
    while(!end){
        tickrear = clock();
        if((tickrear-tickfront)>=QUANTUM*1000){
            tickfront = tickrear;
            
            end = scheduling();
        }
        usleep(CALLBACK_TIME);
    }
    printf("\n\nALL PROCESSES ENDED!\n\n");
    return 0;

}