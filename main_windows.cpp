#include <windows.h>      // for CreateProcess
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>       // for cout
#include <tchar.h>        // fot TCHAR Type
#include <thread>         // for this_thread::sleep_for

using namespace std;

bool trow_the_coin(){         // trow_the_coin function
    if (rand()%2+1 == 2)
        return true;          //will increment the counter
    return false;
}

int _tmain(int argc, TCHAR* argv[]){

    srand(time(NULL));      // restarts the seeds for rand

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //Create a native windows shared memory object for semaphore
    boost::interprocess::windows_shared_memory shsem (boost::interprocess::create_only, "SemaphoreSharedMemory",
                                                      boost::interprocess::read_write, sizeof(int));
    boost::interprocess::mapped_region region_sem(shsem, boost::interprocess::read_write);
    int *semaphore = static_cast<int*>(region_sem.get_address());
    *semaphore = 1;
    //Create a native windows shared memory object for data (shared memory)
    boost::interprocess::windows_shared_memory shm (boost::interprocess::create_only, "DataSharedMemory",
                                                    boost::interprocess::read_write, sizeof(int));
    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
    int* mem = static_cast<int*>(region.get_address());
    *mem = 0;
    int cp = 0, cc = 0; //counters for parent/child processes
    int i = 0;          // temporary value for increment *mem
    //Throw the coin until SharedMemory reach 1000
    while(*mem < 1000){
        while(!(*semaphore));  //wait if semaphore = 0
        cp++;
        if(trow_the_coin()){
            i = *mem;
            i++;
            *mem = i;
            cout << "Proces parinte. Pasul = " << cp << ", SharedMemory = " << *mem <<endl;
        }
        *semaphore = 0;  //set the semaphore to child
        // Start the second process
        if (CreateProcess(NULL, TEXT("cmd"), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            boost::interprocess::windows_shared_memory shsem (boost::interprocess::open_only, "SemaphoreSharedMemory", boost::interprocess::read_write);
            boost::interprocess::mapped_region region_sem1(shsem, boost::interprocess::read_write);
            int *semaphore1 = static_cast<int*>(region_sem1.get_address());
            boost::interprocess::windows_shared_memory shm (boost::interprocess::open_only, "DataSharedMemory", boost::interprocess::read_write);
            boost::interprocess::mapped_region region1(shm, boost::interprocess::read_write);
            int *mem1 = static_cast<int*>(region1.get_address());
            while(*semaphore1);  //wait if semaphore = 1
            cc++;
            if(trow_the_coin()){
                i = *mem1;
                i++;
                *mem1 = i;
                cout << "Proces copil. Pasul = " << cc << ", SharedMemory = " << *mem1 <<endl;
            }
            *semaphore1 = 1;  // set the semaphore for parent
        }
        else {
            cout << "Eroare creare proces!" << endl;
            return 0;
        }
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;

}
