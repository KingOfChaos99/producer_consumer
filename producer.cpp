/*
 * This program represents the Producer process in the problem.
 * It generates items and places them onto a shared "table" in shared memory.
 * The table can only hold two items at a time.
 * Synchronization is managed using POSIX semaphores to ensure:
 * - The producer waits if the table is full.
 * - The consumer waits if the table is empty.
 * - Mutex ensures only one process accesses shared memory at a time.
 * Compile: g++ producer.cpp -o producer -pthread -lrt
 * Run: ./producer &
 */

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>    // For sem_open, sem_wait, etc.
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace std;

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

// Producer thread function
void* producer_thread(void* arg) {
    int item = 1;

    while (true) {
        sem_wait(empty_sem); // wait for empty slot
        sem_wait(mutex_sem); // lock

        if (table_ptr->count < TABLE_SIZE) {
            table_ptr->items[table_ptr->count++] = item;
            cout << "Produced item: " << item << endl;
            item++;
        }

        sem_post(mutex_sem); // unlock
        sem_post(full_sem);  // signal one full slot
        sleep(1);
    }
    return nullptr;
}

int main() {
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); return 1; }
    if (ftruncate(shm_fd, sizeof(Table)) == -1) { perror("ftruncate"); return 1; }

    table_ptr = (Table*) mmap(0, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table_ptr == MAP_FAILED) { perror("mmap"); return 1; }

    // Initialize table
    table_ptr->count = 0;
    memset(table_ptr->items, 0, sizeof(table_ptr->items));

    // Create semaphores
    empty_sem = sem_open(SEM_EMPTY, O_CREAT, 0666, TABLE_SIZE);
    full_sem  = sem_open(SEM_FULL,  O_CREAT, 0666, 0);
    mutex_sem = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    if (empty_sem == SEM_FAILED || full_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("sem_open"); return 1;
    }

    cout << "Producer started." << endl;

    // Start producer thread
    pthread_t prod_thread;
    if (pthread_create(&prod_thread, nullptr, producer_thread, nullptr) != 0) {
        cerr << "Failed to create thread" << endl;
        return 1;
    }

    pthread_join(prod_thread, nullptr); // wait for thread (runs infinitely)
    return 0;
}
