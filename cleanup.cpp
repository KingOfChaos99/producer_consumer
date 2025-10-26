/*
 * Utility program to remove shared memory and semaphores from the system.
 * This avoids file exists errors when re-running producer/consumer
 * Compile: g++ cleanup.cpp -o cleanup -pthread -lrt
 * Run: ./cleanup
 */

#include <iostream>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

int main() {
    // Remove shared memory
    if (shm_unlink("/my_shared_memory") == 0)
        cout << "Removed shared memory: /my_shared_memory" << endl;
    else
        perror("shm_unlink");

    // Remove all semaphores
    if (sem_unlink("/empty") == 0)
        cout << "Removed semaphore: /empty" << endl;
    else
        perror("sem_unlink /empty");

    if (sem_unlink("/full") == 0)
        cout << "Removed semaphore: /full" << endl;
    else
        perror("sem_unlink /full");

    if (sem_unlink("/mutex") == 0)
        cout << "Removed semaphore: /mutex" << endl;
    else
        perror("sem_unlink /mutex");

    cout << "Cleanup complete." << endl;
    return 0;
}
