#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Process {
    int Process_ID;
    int Arrival_Time;
    int Burst_Duration;
    int Priority;
    int TurnAroundTime;
    int WaitTime;
};
//**********************************************************
struct Gantt {
  int Process_ID;
  int Arrival_Time;
  int End_Time;
};

void File_Content(struct Process Processes[], int number);
void FCFS(struct Process Processes[], int number);
void SJFP(struct Process Processes[], int number);
void PRIORITY(struct Process Processes[], int number);
void RR(struct Process Processes[], int number, int quantum);
void Display_Output(struct Process Processes[], int number, struct Gantt Rows[], int count, char* filename);

int main(int argc, char **argv) {

    int number_of_pross = 0;

    if (argc != 2) {
        printf("Incorrect number of arguments.\n");
        return -1;
    }
    
    FILE* file1 = fopen(argv[1], "r");
    
    while ( !feof(file1) ) {
        char ch = fgetc(file1);
        if(ch == '\n') {
            number_of_pross++;
        }
    }
    
    struct Process Processes[number_of_pross];
    
    fseek(file1, 0, SEEK_SET);
    
    number_of_pross=0;
    
    while ( fscanf(file1, "%d,%d,%d,%d", &Processes[number_of_pross].Process_ID, &Processes[number_of_pross].Arrival_Time, &Processes[number_of_pross].Burst_Duration, &Processes[number_of_pross].Priority) != EOF ) {
        number_of_pross++;
    }
    
    fclose(file1);

    File_Content(Processes, number_of_pross);
    FCFS(Processes, number_of_pross);
    SJFP(Processes, number_of_pross);
    PRIORITY(Processes, number_of_pross);
    
    return 0;
}
//--------------------------------------------------------
void File_Content(struct Process Processes[], int number) {
    int i;
    printf("PROCESS ID\tARRIVAL TIME\tBURST DURATION\tPRIORITY\n");
    for (i=0; i < number; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\n", Processes[i].Process_ID, Processes[i].Arrival_Time, Processes[i].Burst_Duration, Processes[i].Priority);
    }
}
//---------------------------------------------------------------
void Display_Output(struct Process Processes[], int number, struct Gantt Rows[], int count, char *filename) {
    
    FILE* file;
    int i;
    file = fopen(filename, "w");
    fprintf(file, "PROCESS ID\tARRIVAL TIME\tEND TIME\n");

    for (i = 0; i < count; i++) {
        fprintf(file, "%10d%10d%10d\n", Rows[i].Process_ID, Rows[i].Arrival_Time, Rows[i].End_Time);
    }

    float avgWait = 0;
    float avgTurnAround = 0;
    for (i = 0; i < number; i++) {
        avgWait += Processes[i].WaitTime;
        avgTurnAround += Processes[i].TurnAroundTime;
    }
    fprintf(file, "Average Wait Time=%f\n", avgWait/number);
    fprintf(file, "TurnAround Time=%f\n", avgTurnAround/number);
    // Assumption: the value of count recieved is greater than exact number of rows in the Gantt chart by 1. Dont forget to adjust the value if yours is not so.
    fprintf(file, "Throughput=%f\n", ( (float)number/Rows[count-1].End_Time ));
    fclose(file);

}
//------------------------------------------------------------

typedef struct {
    int front, rear, size;
    unsigned capacity;
    int* array;
} Queue;

Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue)); 
    queue->capacity = capacity; 
    queue->front = queue->size = 0;  
    queue->rear = capacity - 1;  // This is important, see the enqueue 
    queue->array = (int*) malloc(queue->capacity * sizeof(int)); 
    return queue; 
}

int isFull(Queue* queue) {  return (queue->size == queue->capacity);  }

int isEmpty(Queue* queue) {  return (queue->size == 0); }

int size(Queue* queue) { return queue->size; }

void push(Queue* queue, int item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

void add(Queue* queue, int item, int index) {
    if (isFull(queue))
        return;
    for (int i = queue->rear; i >= index; --i) {
       queue->array[queue->front+i+1] = queue->array[queue->front+i];
    }
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->front+index] = item;
    queue->size = queue->size + 1;
}

int pop(Queue* queue) { 
    if (isEmpty(queue))
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

int front(Queue* queue) { 
    if (isEmpty(queue))
        return -1;
    return queue->array[queue->front];
}

int back(Queue* queue) {
    if (isEmpty(queue))
        return -1;
    return queue->array[queue->rear];
}

int get(Queue* queue, int index) {
    if (isEmpty(queue))
        return -1;
    return queue->array[queue->front+index];
}

int turnAroundTime(int curTime, struct Process curProc) {
    return curTime - curProc.Arrival_Time;
}

int waitTime(struct Process curProc, int rowArrivalTime) {
    return rowArrivalTime - curProc.Arrival_Time - (curProc.TurnAroundTime - curProc.WaitTime);
}

int burstTimeSpent(int curTime, struct Process curProc, int rowArrivalTime) {
    return (curTime - rowArrivalTime) + (curProc.TurnAroundTime - curProc.WaitTime);
}

int burstTimeRemaining(int curTime, struct Process curProc, int rowArrivalTime) {
    return curProc.Burst_Duration - burstTimeSpent(curTime, curProc, rowArrivalTime);
}

/*
 Note: Use function Display_Output to display the result. The Processes is the
 updated Processes with their respective waittime, turnAround time.
 Use the array of Gantt Struct to mimic the behaviour of Gantt Chart. Rows is the base name of the same array.
 number is the number of processes which if not changed during processing, is taken care. count is the number of rows in the Gantt chart. 
 
 Number of Processes will not exceed 1000. Create an array of Gantt chart which can accomodate at least 1000 process.
 */
// -------------FCFS----------------------------------------
void FCFS(struct Process Processes[], int number) {
    //---------Start Processing
    
    if (number == 0)
        return;
    
    // Create an array of Gantt chart which can accomodate at least 1000 processes.
    // Use the array of Gantt Struct to mimic the behaviour of Gantt Chart. "Rows" is the base name of the same array.
    struct Gantt Rows[number];
    
    // You don't care about "priority"
    // 1 is the highest priority, 50 is the lowest.
    
    Queue* queue = createQueue(1000);
    
    int count = 0;
    
    Rows[count].Arrival_Time = 0;
    
    int curTime = 0;
    while (count < number) {
        
        for (int i = 0; i < number; ++i) {
            if (Processes[i].Arrival_Time == curTime) {
                push(queue, i);
            }
        }
        
        int curProc = front(queue);
        if (curProc == -1) {
            ++curTime;
            continue;
        }
        
        int burstTimeSpent = curTime - Rows[count].Arrival_Time;
        
        if (burstTimeSpent == Processes[curProc].Burst_Duration) {
            
            Processes[curProc].WaitTime = Rows[count].Arrival_Time - Processes[curProc].Arrival_Time;
            Processes[curProc].TurnAroundTime = curTime - Processes[curProc].Arrival_Time;
            
            Rows[count].Process_ID = Processes[curProc].Process_ID;
            Rows[count].End_Time = curTime;
            ++count;
            Rows[count].Arrival_Time = curTime;
            
            pop(queue);
            
        }
        
        ++curTime;
    }
    
    //---------End of Processing
    // Hint: Call function with arguments shown Display_Output(Processes,number,Rows,count,"FCFS");
    // "Processes" is the updated Processes with their respective "WaitTime" and "TurnAroundTime".
    // WaitTime = totalTimeWaiting;
    // TurnAroundTime = completionTime - submissionTime;
    // "number" is the number of processes, which if not changed during processing is taken care.
    // "count" is the number of rows in the Gantt chart. 
    Display_Output(Processes, number, Rows, count, "FCFS");
}
//---------------------SJFP---------------------------------------
void SJFP(struct Process Processes[], int number) {
    //---------Start Processing
    // It's preemptive.
    
    if (number == 0)
        return;
    
    struct Gantt Rows[1100];
    Queue* queue = createQueue(2000);
    int count = 0;
    Rows[count].Arrival_Time = 0;
    
    for (int i = 0; i < number; ++i) {
        Processes[i].WaitTime = 0;
        Processes[i].TurnAroundTime = 0;
    }
    
    int curTime = 0;
    while (true) {
        
        for (int i = 0; i < number; ++i) {
            if (Processes[i].Arrival_Time == curTime) {
                // If the queue is empty just add it to the end.
                if (size(queue) == 0) {
                    push(queue, i);
                }
                // If the process has less time remaining than the first thing in the queue, then switch out the head of the queue and insert the new process.
                else if (Processes[i].Burst_Duration < burstTimeRemaining(curTime, Processes[front(queue)], Rows[count].Arrival_Time)) {
                    
                    if (curTime != Rows[count].Arrival_Time) {
                        Processes[front(queue)].WaitTime = waitTime( Processes[front(queue)],  Rows[count].Arrival_Time );
                        Processes[front(queue)].TurnAroundTime = turnAroundTime( curTime, Processes[front(queue)] );
                        
                        Rows[count].Process_ID = Processes[front(queue)].Process_ID;
                        Rows[count].End_Time = curTime;
                        ++count;
                        Rows[count].Arrival_Time = curTime;
                    }
                    
                    add(queue, i, 0);
                }
                // Else then add it somewhere in the queue
                else {
                    int s = size(queue);
                    if (s == 1) {
                        push(queue, i);
                    } else {
                        for (int j = 1; j < s; ++j) {
                            if (Processes[i].Burst_Duration < burstTimeRemaining(curTime, Processes[get(queue, j)], curTime)) {
                                add(queue, i, j);
                                break;
                            } else if (j == (s-1)) {
                                push(queue, i);
                            }
                        }
                    }
                }
            }
        }
        
        int curProc = front(queue);
        
        if (burstTimeSpent(curTime, Processes[curProc], Rows[count].Arrival_Time) == Processes[curProc].Burst_Duration) {
            
            Processes[curProc].WaitTime = waitTime(Processes[curProc], Rows[count].Arrival_Time);
            Processes[curProc].TurnAroundTime = turnAroundTime(curTime, Processes[curProc]);
            
            Rows[count].Process_ID = Processes[curProc].Process_ID;
            Rows[count].End_Time = curTime;
            ++count;
            Rows[count].Arrival_Time = curTime;
            
            pop(queue);
            
        }
        
        if (curProc == -1) {
            break;
        }
        
        ++curTime;
    }
    
    
    //---------End of Processing
    Display_Output(Processes, number, Rows, count, "SJFP");
}
//------------------PRIORITY-------------------------------------
void PRIORITY(struct Process Processes[], int number) {
    //---------Start Processing
    
    // It's preemptive.
    // 1 is the highest priority, 50 is the lowest.
    
    if (number == 0)
        return;
    
    struct Gantt Rows[1100];
    Queue* queue = createQueue(2000);
    int count = 0;
    Rows[count].Arrival_Time = 0;
    
    for (int i = 0; i < number; ++i) {
        Processes[i].WaitTime = 0;
        Processes[i].TurnAroundTime = 0;
    }
    
    int curTime = 0;
    while (true) {
        
        for (int i = 0; i < number; ++i) {
            if (Processes[i].Arrival_Time == curTime) {
                // If the queue is empty just add it to the end.
                if (size(queue) == 0) {
                    push(queue, i);
                }
                // If the process has less time remaining than the first thing in the queue, then switch out the head of the queue and insert the new process.
                else if (Processes[i].Priority < Processes[front(queue)].Priority) {
                    
                    if (curTime != Rows[count].Arrival_Time) {
                        Processes[front(queue)].WaitTime = waitTime( Processes[front(queue)],  Rows[count].Arrival_Time );
                        Processes[front(queue)].TurnAroundTime = turnAroundTime( curTime, Processes[front(queue)] );
                        
                        Rows[count].Process_ID = Processes[front(queue)].Process_ID;
                        Rows[count].End_Time = curTime;
                        ++count;
                        Rows[count].Arrival_Time = curTime;
                    }
                    
                    add(queue, i, 0);
                }
                // Else then add it somewhere in the queue
                else {
                    int s = size(queue);
                    if (s == 1) {
                        push(queue, i);
                    } else {
                        for (int j = 1; j < s; ++j) {
                            if (Processes[i].Priority < Processes[get(queue, j)].Priority) {
                                add(queue, i, j);
                                break;
                            } else if (j == (s-1)) {
                                push(queue, i);
                            }
                        }
                    }
                }
            }
        }
        
        int curProc = front(queue);
        
        if (burstTimeSpent(curTime, Processes[curProc], Rows[count].Arrival_Time) == Processes[curProc].Burst_Duration) {
            
            Processes[curProc].WaitTime = waitTime(Processes[curProc], Rows[count].Arrival_Time);
            Processes[curProc].TurnAroundTime = turnAroundTime(curTime, Processes[curProc]);
            
            Rows[count].Process_ID = Processes[curProc].Process_ID;
            Rows[count].End_Time = curTime;
            ++count;
            Rows[count].Arrival_Time = curTime;
            
            pop(queue);
            
        }
        
        if (curProc == -1) {
            break;
        }
        
        ++curTime;
    }
    
    //---------End of Processing
    Display_Output(Processes, number, Rows, count, "Priority");
}



