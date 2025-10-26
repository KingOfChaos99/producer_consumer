# Producer-Consumer Problem

## Project Description
This project implements the Producer-Consumer problem using C++ on Linux. 
- The producer generates items and puts them on a shared table.
- The consumer removes items from the table.
- The table can hold up to 2 items at a single time.
- Synchronization is done with semaphores
- Shared memory stores the table.
- Each program uses threads for its logic.

---

## Files
- `producer.cpp` — Producer program
- `consumer.cpp` — Consumer program
- `cleanup.cpp` — Optional: removes shared memory and semaphores
- `README.md` — This file

---

## How to Compile
```bash
g++ producer.cpp -o producer -pthread -lrt
g++ consumer.cpp -o consumer -pthread -lrt
g++ cleanup.cpp -o cleanup -pthread -lrt   # optional

## how to run
##see process ids
./producer & ./consumer &
##to see the jobs running, run the two files in seperate terminals
./cleanup

##example output

Producer started.
Produced item: 1
Produced item: 2
Consumed item: 1
Produced item: 3
Consumed item: 2
Consumed item: 3
Produced item: 4
