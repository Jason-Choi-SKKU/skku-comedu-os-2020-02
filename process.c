process * makeP(pid_t tmppid){
    process * tmpP = (process *)malloc(sizeof(process));
    tmpP -> pid = tmppid;
    tmpP->cpuBurst = (rand() % BURSTSIZE) + BURSTSIZE;
    tmpP->ioBurst = (rand() % BURSTSIZE) + BURSTSIZE;
    tmpP->status = WAITING_CPU;
    tmpP->priority = rand()%3;
    return tmpP;
}

