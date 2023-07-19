#include "fake_process.h"
#include "linked_list.h"
#pragma once

//ogni processo
typedef struct {
  ListItem list;
  int pid;
  ListHead events; //attività che svolge, CPU o IO
  float pred;
  float q_current;
} FakePCB;

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args, int multicore);

typedef struct FakeOS{
  //con aggiunta di ** diventa un array per gestire il multicore
  FakePCB** running; //processo che sta in stato running deve diventare multicore
  ListHead ready;  //processo in ready
  ListHead waiting; // processo in waiting
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;
  ListHead processes;
} FakeOS;

void FakeOS_init(FakeOS* os, int multicore);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
