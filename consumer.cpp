/*
 * This program represents the Consumer process in the problem.
 * It removes items from the shared table stored in shared memory.
 * Synchronization is managed using POSIX semaphores:
 * - The consumer waits if there are no items.
 * - Mutex ensures only one process accesses shared memory at a time.
 * Compile: g++ consumer.cpp -o consumer -pthread -lrt
 * Run: ./consumer &
 */

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>

using namespace std;

// Same constants as in producer
const char* SHM_NAME = "/my_shared_memory";
const char* SEM_EMPTY = "/empty";
const char* SEM_FULL  = "/full";
const char* SEM_MUTEX = "/mutex";
const int TABLE_SIZE = 2;

struct Table {
    int items[TABLE_SIZE];
    int count;
};

// Shared memory pointer and semaphores
Table* table_ptr;
sem_t *empty_sem, *full_sem, *mutex_sem;

// Consumer thread function
void* consumer_thread(void* arg) {
    while (true) {
        sem_wait(full_sem);  // wait for available item
        sem_wait(mutex_sem); // lock

        if (table_ptr->count > 0) {
            int item = table_ptr->items[--table_ptr->count];
            cout << "Consumed item: " << item << endl;
        }

        sem_post(mutex_sem); // unlock
        sem_post(empty_sem); // signal empty slot
        sleep(1);
    }
    return nullptr;
}

int main() {
    // Open existing shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); return 1; }

    table_ptr = (Table*) mmap(0, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table_ptr == MAP_FAILED) { perror("mmap"); return 1; }

    // Open existing semaphores
    empty_sem = sem_open(SEM_EMPTY, 0);
    full_sem  = sem_open(SEM_FULL, 0);
    mutex_sem = sem_open(SEM_MUTEX, 0);
    if (empty_sem == SEM_FAILED || full_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("sem_open"); return 1;
    }

    cout << "Consumer started." << endl;

    // Start consumer thread
    pthread_t cons_thread;
    if (pthread_create(&cons_thread, nullptr, consumer_thread, nullptr) != 0) {
        cerr << "Failed to create thread" << endl;
        return 1;
    }

    pthread_join(cons_thread, nullptr); // wait for thread (runs forever)
    return 0;
}
