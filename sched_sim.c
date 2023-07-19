#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;
int core;

typedef struct {
  int quantum; //quanto di tempo
} SchedRRArgs;


//algoritmo round robin
void schedRR(FakeOS* os, void* args_, int NUM_CORES) {
  SchedRRArgs* args = (SchedRRArgs*)args_;

  // Cerca il primo processo nella coda dei processi pronti per ogni core
  // Se non ci sono processi, ritorna
  for (int core = 0; core < NUM_CORES; core++) {
    if (!os->ready.first)
      break;

    if(os->running[core] == NULL)
    {
      FakePCB* pcb=(FakePCB*) List_popFront(&os->ready);
      os->running[core]=pcb;
      
      assert(pcb->events.first);
      ProcessEvent* e = (ProcessEvent*)pcb->events.first;
      assert(e->type==CPU);

      // look at the first event
      // if duration>quantum
      // push front in the list of event a CPU event of duration quantum
      // alter the duration of the old event subtracting quantum
      if (e->duration>args->quantum) {
        ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
        qe->list.prev=qe->list.next=0;
        qe->type=CPU;
        qe->duration=args->quantum;
        e->duration-=args->quantum;
        List_pushFront(&pcb->events, (ListItem*)qe);
      }
    }
  }
};

int main(int argc, char** argv) {
  FakeOS_init(&os, 4);
  SchedRRArgs srr_args;
  srr_args.quantum=5;
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR;
  
  for (int i=1; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("num processes in queue %d\n", os.processes.size);
  
  while(1){
    core = 0;
    for (int i = 0; i < 4; i++) {
      if (os.running[i] != NULL) {
        core = 1;  // se almeno un elemento non è NULL allora setto a uno
      }
    }

    if(core
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
      FakeOS_simStep(&os);
    }
    else{
      break;
    }
  }
}
