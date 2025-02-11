#ifndef SHM_H
#define SHM_H

#define DEF_KEY_FILE "key"
#define BUF_SIZE 5


static struct sembuf mem_unlock[1] = {2, 1, 0};        // память

static struct sembuf mem_lock[1] = {2, -1, 0};         // память

static struct sembuf setFree[1] = {0, BUF_SIZE, 0};   // пустые

static struct sembuf releaseEmpty[1] = {0, 1, 0};     // пустые

static struct sembuf waitNotFull[1] = {0, -1, 0};      // пустые

static struct sembuf releaseFull[1] = {1, 1, 0};       // заполненные

static struct sembuf waitNotEmpty[1] = {1, -1, 0};     // заполненные



#endif /* SHM_H */

