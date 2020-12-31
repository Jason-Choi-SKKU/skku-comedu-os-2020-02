
circularQueue * makeQ(){
    circularQueue * tmpQ = (circularQueue *)malloc(sizeof(circularQueue));
    tmpQ->front = -1;
    tmpQ ->rear = -1;
    tmpQ -> max = PROCESS_NUM+1;
    tmpQ->num = 0;
    return tmpQ;
}

void addQ(process * p, circularQueue * q){
        q->rear = (q->rear + 1)%(q->max);
        q->num++;
        q->queue[q->rear] = p;
    
}

process * popQ(circularQueue * q){
    q->front = (q->front +1)%q->max;
    q->num--;
    process * tmp = q->queue[q->front];
    q->queue[q->front] = NULL;
    return tmp;
}

void traverseQ(circularQueue * q){
    for (int i=0; i < q->num; i++){
        printf(" %d",q->queue[(q->front +1 +i)%q->max]->pid);
    }
    printf("\n");
}

int isEmpty(circularQueue * q){
    if(q->front == q->rear){
        return 1;
    }
    else{
        return 0;
    }
}