#include "Hermes.hh"
#include <cstdio>

ForkShareMemory::ForkShareMemory() {
  if ((key_n = ftok(".", 's') < 0)) {
    perror("Fail to ftok\n");
    FAIT_EXIT
  }

  if ((shm_id = shmget(key_n, 1024, 0666 | IPC_CREAT)) == -1) {
    perror("shmget failed...\n");
    FAIT_EXIT
  }

  void *ret = shmat(shm_id, NULL, 0);
  if (ret == (void *)-1) {
    perror("shmat failed...\n");
    FAIT_EXIT
  } else {
    info = (shinfo *)ret;
  }

  info->flag = false;
  info->notgood = false;
  info->endCycles = 0;
  info->sel_pid = 0;
}

ForkShareMemory::~ForkShareMemory() {
  if (shmdt(info) == -1) {
    perror("detach error\n");
  }
  shmctl(shm_id, IPC_RMID, NULL);
}

void ForkShareMemory::shwait() {
  while (true) {
    if (info->flag && (getpid() == info->sel_pid)) {
      if (info->notgood)
        break;
      else
        exit(0);
    } else {
      sleep(2);
    }
  }
}

int Hermes::do_fork(uint64_t cycle) {
  // kill the oldest blocked checkpoint process
  if (maxSlotCnt == pidSlot.size()) {
    pid_t temp = pidSlot.front().pid;
    pidSlot.pop_front();
    kill(temp, SIGKILL);
    waitpid(temp, NULL, 0);
  }
  // fork a new checkpoint process and block it
  if ((pid = fork()) < 0) {
    printf("[%d]Error: could not fork process!\n", getpid());
    return FORK_ERROR;
  }
  // the original process
  else if (pid != 0) {
    pidSlot.emplace_back(pid, cycle, false);
    return FORK_OK;
  }
  // for the fork child
  waitProcess = 1;
  forkshm.shwait();
  // checkpoint process wakes up
  // start wave dumping
  // bool is_sel = forkshm.info->sel_pid == getpid();
  //(is_sel) ? WAIT_SEL : WAIT_EXIT;
  return WAIT_SEL;
}

int Hermes::wakeup_child(pid_t pid) {
  forkshm.info->endCycles = 0;
  forkshm.info->sel_pid = pid;
  forkshm.info->notgood = true;
  forkshm.info->flag = true;
  //   kill(pid , SIGUSR1);
  int status = -1;
  waitpid(pid, &status, 0);
  return 0;
}

bool Hermes::is_child() { return waitProcess; }

int Hermes::do_clear() {
  if (is_child())
    return 0;
  FORK_PRINTF("clear all fork point...\n")
  while (pidSlot.size()) {
    forkPoint t = pidSlot.back();
    if (!t.used) {
      kill(t.pid, SIGKILL);
      waitpid(t.pid, NULL, 0);
    }
    pidSlot.pop_back();
  }
  return 0;
}

void Hermes::wakeup(unsigned num) {

  pid_t sel_pid = -1;
  int counter = 1;
  bool used = false;
  for (auto it = pidSlot.begin(); it != pidSlot.end(); it++) {
    if (counter++ == num) {
      used = (*it).used;
      sel_pid = (*it).pid;
      (*it).used = true;
      break;
    }
  }
  if (used) {
    FORK_PRINTF("!!!pid %d is used \n", sel_pid);
  } else if (sel_pid == -1) {
    FORK_PRINTF("ERROR pid , select wrong\n");
  } else {
    FORK_PRINTF("now select pid %d is replaying \n", sel_pid);
    wakeup_child(sel_pid);
  }
}

void Hermes::dumpForkPoint() {
  int num;
  FORK_PRINTF("\n");
  FORK_PRINTF("----Dump ForkPoint---- \n");
  num = 0;
  for (auto i : pidSlot)
    FORK_PRINTF("(%d) pid : %d cycles : %d %s \n", ++num, i.pid, i.curCycle,
                i.used ? "(used)" : " ");
  FORK_PRINTF("----Dump ForkPoint---- \n");
}

void Hermes::process() {
  if (is_child())
    return;

  char cmd[2];
  int num;

  while (1) {
    dumpForkPoint();
    FORK_PRINTF("(hermes):");
    scanf("%s",cmd);

    if (cmd[0] == 'q') {
      do_clear();
      exit(0);
    } else if (cmd[0] >= '1' && cmd[0] <= '9') {
      num = cmd[0] - '0';
      wakeup(num);
    }else{
      FORK_PRINTF("Error %c",cmd[0]);
    }
  }
}
