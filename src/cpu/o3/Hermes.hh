#ifndef __HERMES_HH
#define __HERMES_HH

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/prctl.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CHILDS 8

typedef struct shinfo {
  bool flag;
  bool notgood;
  uint64_t endCycles;
  pid_t sel_pid;
} shinfo;

class ForkShareMemory {
private:
  key_t key_n;
  int shm_id;

public:
  shinfo *info;

  ForkShareMemory();
  ~ForkShareMemory();

  void shwait();
};

#define FAIT_EXIT exit(1);
const int FORK_OK = 0;
const int FORK_ERROR = 1;
const int WAIT_SEL = 2;
const int WAIT_EXIT = 3;

typedef struct forkPoint {
  pid_t pid;
  unsigned curCycle;
  bool used;
  forkPoint(pid_t _pid, unsigned _curCycle, bool _used)
      : pid(_pid), curCycle(_curCycle), used(_used) {}
} forkPoint;

class Hermes {
  pid_t pid = -1;
  unsigned maxSlotCnt;

  int waitProcess = 0;
  std::list<forkPoint> pidSlot;
  ForkShareMemory forkshm;

public:
  int last;
  Hermes(unsigned maxslot) : maxSlotCnt(maxslot), last(0) {}
  ~Hermes() {
    if (pidSlot.size())
      do_clear();
  }

  int do_fork(uint64_t cycle);
  void process();
  void showHelp();
  void dumpForkPoint();
  void wakeup(unsigned num);
  int wakeup_child(pid_t pid);
  bool is_child();
  int do_clear();
  uint64_t get_end_cycles() { return forkshm.info->endCycles; }
};

#define FORK_PRINTF(format, args...)               \
  do {                                             \
    printf("[pid(%d)] ->" format, getpid(), ##args); \
    fflush(stdout);                                \
  } while (0);

#endif