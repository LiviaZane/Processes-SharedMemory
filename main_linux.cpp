#include <iostream>                    // for cout
#include <unistd.h>                    // for fork()
#include <sys/types.h>                 // for pid_t type
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

bool trow_the_coin(){         // trow_the_coin function
    if (rand()%2+1 == 2)
        return true;          //will increment the counter
    return false;
}

int main(){

    srand(time(NULL));      // restarts the seeds

    //Create a shared memory object.
    shared_memory_object shdmem{open_or_create, "DataSharedMemory", read_write};
    shdmem.truncate(1);

    //Maped to curent process (parent process)
    mapped_region region{shdmem, read_write};

    //Write all the memory to 0
    int x = 0;
    std::memset(region.get_address(), x, region.get_size());

    //Pointer to shared memory
    int *mem1 = static_cast<int*>(region.get_address());

    // Shared memory for semaphor
    shared_memory_object shm (open_or_create, "SemaphorSharedMemory", read_write);
    shm.truncate(1);

    //Maped to parent process
    mapped_region region1(shm, read_write);

    //Pointer to shared memory semaphore
    int *semaphore = static_cast<int*>(region1.get_address());
    *semaphore = 1;

    //Starts 1 child process and continue the main
    pid_t pid1 = fork();

    int cp = 0, cc = 0;       //counters for parent and child

    //Trow the coin until shared memory counter reach 1000
    while (*mem1 < 1000) {
        if (pid1 > 0) {          // parent process
            cp++;   // increment the counter for parent process

            //Read the counter value from shared memory
            mem1 = static_cast<int*>(region.get_address());

            //Read the semaphore from shared memory
            semaphore = static_cast<int*>(region1.get_address());

            while(!(*semaphore));  //wait if flag_mem = 0
            x = *mem1;
            if(trow_the_coin()){
                x++;
                *mem1 = x;
                std::cout << "Parent process counter = " << cp << ", SharedMemory = " << *mem1 << std::endl;
            }
            *semaphore = 0;  //set the semaphore to child process
        } else {            // child process
            cc++;   // increment the counter for child process

            //Mapped all shared memory to child process
            mapped_region region2{shdmem, read_write};

            //Read the counter value from shared memory
            int *mem2 = static_cast<int*>(region2.get_address());

            //Mapped semaphore shared memory to child process
            mapped_region region3(shm, read_write);

            //Read the semaphore from shared memory
            int *semaphore2 = static_cast<int*>(region3.get_address());

            while(*semaphore2);  //wait if semaphor = 1
            x = *mem2;
            if(trow_the_coin()){
                x++;
                *mem2 = x;
                std::cout << "Child process counter = " << cc << ", SharedMemory = " << *mem2 << std::endl;
            }
            *semaphore2 = 1;  // set the semaphore for parent process
         }
    }

    //Delete shared memory
    shared_memory_object::remove("DataSharedMemory");
    shared_memory_object::remove("SemaphoreSharedMemory");

    return 0;
}
