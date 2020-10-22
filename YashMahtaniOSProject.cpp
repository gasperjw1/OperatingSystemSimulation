//YASH MAHTANI

#include <iostream>
#include <vector>
#include <string>

enum procType{ //Enum for type of process
    NOTHING = -1,
    COMMON = 0,
    REALTIME = 1,
};

enum status{ //Enum for the status of the process
    EMPTY = -1,
    WAITING = 0,
    RUNNING = 1,
    WAITING_FOR_HDD = 3,
    USING_HDD = 4,
};

struct Process{ //Struct for the Process
    long long int pid;
    long long int size;
    procType type;
    long long int beginningPositionInMemory;
    long long int endPositionInMemory;
    status state;
};

struct HardDisk{ //Struct for the Hard Disks
    int number;
    std::vector<Process> hddProcesses;
};

int main(){
    long long int amountOfRAM = 0;
    int amountOfHardDisks = 0;
    long long int pidIncrement = 1;
    long long int remainingRAM = 0;

    std::cout << "How much RAM memory is there on the simulated computer? ";
    std::cin >> amountOfRAM;
    remainingRAM = amountOfRAM;
    std::cout << std::endl; 

    std::cout << "How many hard disks does the simulated computer have? ";
    std::cin >> amountOfHardDisks;
    std::cout << std::endl;

    std::vector<HardDisk> hdds;
    for(int i = 0; i < amountOfHardDisks; i++){
        HardDisk tempHDD;
        tempHDD.number = i;
        hdds.push_back(tempHDD);
        //std::cout << "Hard Disk Number: ";
        //std::cout << hdds[i].number << std::endl;
    }

    std::string instruction;
    long long int size;
    std::vector<Process> memory;
    std::vector<Process> commonProcessesQueue;
    std::vector<Process> realTimeProcessesQueue;
    //Process currentRunningProcess = {0,0,NOTHING};
    long long int currentRunningProcessPID = 0;
    procType currentRunningProcessType = NOTHING;

    while(true){
        std::cin >> instruction;
        //std::cout << "The instruction is: " << instruction << std::endl;
        //std::cout << "Current running process is PID:" << currentRunningProcessPID << std::endl;

        if(instruction.substr(0,1) == "A" && instruction.length() == 1){ //Check if we are adding a COMMON process
            std::cin >> size;

            if(size > remainingRAM){ //Checks if the process is too big for the RAM
                std::cout << "THAT PROCESS CANNOT BE HANDLED BY THIS TINY MACHINE" << std::endl;
            }
            else{ //If not too big, then look for a spot in memory to place the process
                bool lookingForAvailableSlot = true;

                if(memory.empty() || memory[0].beginningPositionInMemory >= size){ //Checks if there is free space in the start
                    Process temp = {pidIncrement,size,COMMON,0,size-1,WAITING};
                    pidIncrement++;

                    if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                        currentRunningProcessPID = temp.pid;
                        currentRunningProcessType = COMMON;
                        temp.state = RUNNING;
                    }
                    else{
                        commonProcessesQueue.push_back(temp); //Else put process in Ready Queue
                    }
                
                    memory.insert(memory.begin(),temp);

                    //memory[position+1].endPositionInMemory
                }
                else{ //Look for a free spot in memory in between other processes
                    int position = 0;
                    while(lookingForAvailableSlot){ //Loop to keep checking memory for a spot for the process
                        long long int begin = memory[position].endPositionInMemory;

                        if(memory.size() > position + 1){ //A check to see if we are looking for holes
                            long long int end = memory[position+1].beginningPositionInMemory;
                            if(end - begin >= size){ //If there is a hole
                                Process temp = {pidIncrement,size,COMMON,begin+1,begin+size,WAITING};
                                pidIncrement++;

                                if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                                    currentRunningProcessPID = temp.pid;
                                    currentRunningProcessType = COMMON;
                                    temp.state = RUNNING;
                                }
                                else{ //Else put it in the Ready Queue
                                    commonProcessesQueue.push_back(temp);
                                }

                                memory.insert(memory.begin() + position + 1,temp);
                                remainingRAM -= size;
                                lookingForAvailableSlot = false;
                            }
                            position++;
                        }
                        else if(memory.size() == position + 1){ //Checks if there is space for the process after all the processes
                            if(amountOfRAM - begin >= size){ //If there is space in the end, place the process there
                                Process temp = {pidIncrement,size,COMMON,begin+1,begin+size,WAITING};
                                pidIncrement++;

                                if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                                    currentRunningProcessPID = temp.pid;
                                    currentRunningProcessType = COMMON;
                                    temp.state = RUNNING;
                                }
                                else{ //Else put it in the ready Queue
                                    commonProcessesQueue.push_back(temp);
                                }

                                memory.insert(memory.begin() + position + 1,temp);
                                remainingRAM -= size;
                                lookingForAvailableSlot = false;
                            }
                        }
                        else{ //Just in case
                            std::cout << "Not enough space" << std::endl;
                        }
                    }
                }
            }
            
        }
        else if(instruction.substr(0,2) == "AR"){ //Checks if we are adding a Real-Time process
            std::cin >> size;

            if(size > remainingRAM){ //Checks if the process is too big
                std::cout << "THAT PROCESS CANNOT BE HANDLED BY THIS TINY MACHINE" << std::endl;
            }
            else{ //Else, look for a spot to place the process
                bool lookingForAvailableSlot = true;

                if(memory.empty() || memory[0].beginningPositionInMemory >= size){ //Free space in the start
                    Process temp = {pidIncrement,size,REALTIME,0,size-1,WAITING};
                    pidIncrement++;

                    if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                        currentRunningProcessPID = temp.pid;
                        currentRunningProcessType = REALTIME;
                        temp.state = RUNNING;
                    }
                    else if(currentRunningProcessType == COMMON){ //Pre-empt check if there is a common process in CPU
                        for(int i = 0; i < memory.size(); i++){
                            //std::cout << "I am here with PID: " << memory[i].pid;
                            //std::cout << " and a state of: " << memory[i].state << std::endl;
                            if(memory[i].state == RUNNING){
                                //std::cout << "I am inside!" << std::endl;
                                memory[i].state = WAITING;
                                Process temp2 = memory[i];
                                commonProcessesQueue.insert(commonProcessesQueue.begin(),temp2);
                            }
                        }
                        currentRunningProcessType = REALTIME;
                        currentRunningProcessPID = temp.pid;
                        //temp.state = RUNNING;
                    }
                    else{
                        realTimeProcessesQueue.push_back(temp); //Put process in Ready Queue
                    }

                    memory.insert(memory.begin(),temp);
                }
                else{ //Look for a free spot in memory in between other processes
                    int position = 0;
                    while(lookingForAvailableSlot){ //Loop to keep looking for a spot in memory to place process
                        long long int begin = memory[position].endPositionInMemory;

                        if(memory.size() > position + 1){ //Check to see if there is a hole
                            long long int end = memory[position+1].beginningPositionInMemory;
                            if(end - begin >= size){ //Put process in a hole if the hole is big enough to fit the process
                                Process temp = {pidIncrement,size,REALTIME,begin+1,begin+size,WAITING};
                                pidIncrement++;
                                memory.insert(memory.begin() + position + 1,temp);

                                if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                                    currentRunningProcessPID = temp.pid;
                                    currentRunningProcessType = REALTIME;
                                    temp.state = RUNNING;
                                }
                                else if(currentRunningProcessType == COMMON){ //Pre-empt check if there is a common process in CPU
                                    for(int i = 0; i < memory.size(); i++){
                                        //std::cout << "I am here with PID: " << memory[i].pid;
                                        //std::cout << " and a state of: " << memory[i].state << std::endl;
                                        if(memory[i].state == RUNNING){
                                            //std::cout << "I am inside!" << std::endl;
                                            memory[i].state = WAITING;
                                            Process temp2 = memory[i];
                                            commonProcessesQueue.insert(commonProcessesQueue.begin(),temp2);
                                        }
                                        else if(memory[i].pid == temp.pid){
                                            memory[i].state = RUNNING;
                                        }
                                    }
                                    currentRunningProcessType = REALTIME;
                                    currentRunningProcessPID = temp.pid;
                                    //temp.state = RUNNING;
                                }
                                else{ //Else just put the process in its respective queue
                                    realTimeProcessesQueue.push_back(temp);
                                }
                                remainingRAM -= size;
                                lookingForAvailableSlot = false;
                            }
                            position++;
                        }
                        else if(memory.size() == position + 1){ //Checks if there is space for the process after all the processes
                            if(amountOfRAM - begin >= size){
                                Process temp = {pidIncrement,size,REALTIME,begin+1,begin+size,WAITING};
                                pidIncrement++;
                                memory.insert(memory.begin() + position + 1,temp);

                                if(currentRunningProcessType == NOTHING){ //Put process in CPU if CPU is free
                                    currentRunningProcessPID = temp.pid;
                                    currentRunningProcessType = REALTIME;
                                    temp.state = RUNNING;
                                }
                                else if(currentRunningProcessType == COMMON){ //Pre-empt check if there is a common process in CPU
                                    for(int i = 0; i < memory.size(); i++){
                                        //std::cout << "I am here with PID: " << memory[i].pid;
                                        //std::cout << " and a state of: " << memory[i].state << std::endl;
                                        if(memory[i].state == RUNNING){
                                            //std::cout << "I am inside!" << std::endl;
                                            memory[i].state = WAITING;
                                            Process temp2 = memory[i];
                                            commonProcessesQueue.insert(commonProcessesQueue.begin(),temp2);
                                        }
                                        else if(memory[i].pid == temp.pid){
                                            memory[i].state = RUNNING;
                                        }
                                    }
                                    currentRunningProcessType = REALTIME;
                                    currentRunningProcessPID = temp.pid;
                                    //temp.state = RUNNING;
                                }
                                else{ //Else put the process in the Ready Queue
                                    realTimeProcessesQueue.push_back(temp);
                                }
                                remainingRAM -= size;
                                lookingForAvailableSlot = false;
                            }
                        }
                        else{ //Just in case
                            std::cout << "Not enough space" << std::endl;
                        }
                    }
                }
            }
        }
        else if(instruction.substr(0,1) == "Q"){ //Current process' time slice has ended
            if(currentRunningProcessType == REALTIME){ //Checks if current process is REALTIME
                if(realTimeProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){ //Update the rest of the queue
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            memory[i].state = WAITING;
                            realTimeProcessesQueue.push_back(memory[i]);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){ //Update the rest of the queue
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            memory[i].state = WAITING;
                            realTimeProcessesQueue.push_back(memory[i]);
                        }
                    }
                }
                else{ //Just in case
                    std::cout << "The time slice is done but there is no other processes that require the CPU!" << std::endl;
                }
            }
            else if(currentRunningProcessType == COMMON){ //Checks if current process is COMMON
                if(realTimeProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessType = REALTIME;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){ //Update the rest of the queue
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            memory[i].state = WAITING;
                            commonProcessesQueue.push_back(memory[i]);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(commonProcessesQueue.size() == 1){ //If there was only one process in the queue
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){ //Update the rest of the queue
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            memory[i].state = WAITING;
                            commonProcessesQueue.push_back(memory[i]);
                        }
                    }
                }
                else{ //Just in case
                    std::cout << "The time slice is done but there is no other processes that require the CPU!" << std::endl;
                }
            }
        }
        else if(instruction.substr(0,1) == "t"){ //Terminate process currently using CPU
            if(currentRunningProcessType == REALTIME){ //Checks if current process is REALTIME
                if(realTimeProcessesQueue.size() > 0){ //Checks if there is any process in the rt queue that wants to use the CPU
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){ //Update the rest of the queue
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and terminat the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){ //Checks if there is any process in the common queue that wants to use the CPU
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){ //Update the rest of the queue
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and terminate the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                        }
                    }
                }
                else{ //To reach this condition, there is no other processes waiting to use CPU
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                            currentRunningProcessPID = -1;
                            currentRunningProcessType = NOTHING;
                        }
                    }
                }
            }
            else if(currentRunningProcessType == COMMON){ //Checks if current process is COMMON
                if(realTimeProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessType = REALTIME;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){ //If there was only one process in the queue
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){ //Update the rest of the queue
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and remove the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(commonProcessesQueue.size() == 1){ //If there was only one process in the queue
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){ //Update the rest of the queue
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){ //Update the processes states and remove the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                        }
                    }
                }
                else{ //If there is no process waiting in a queue
                    for(int i = 0; i < memory.size(); i++){ //Terminate the process that WAS using the CPU
                        if(memory[i].pid == currentRunningProcessPID){
                            remainingRAM += memory[i].size;
                            memory.erase(memory.begin() + i);
                            currentRunningProcessPID = -1;
                            currentRunningProcessType = NOTHING;
                        }
                    }
                }
            }
        }
        else if(instruction.substr(0,1) == "d" && instruction.length() == 1){ //Current process in CPU wants to use a Hard Disk
            int desiredHDD = -1;
            std::cin >> desiredHDD;
            if(currentRunningProcessType == REALTIME){ //Checks if current process is REALTIME
                if(realTimeProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            if(hdds[desiredHDD].hddProcesses.empty()){
                                memory[i].state = USING_HDD;
                            }
                            else{
                                memory[i].state = WAITING_FOR_HDD;
                            }
                            hdds[desiredHDD].hddProcesses.push_back(memory[i]);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(realTimeProcessesQueue.size() == 1){
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            if(hdds[desiredHDD].hddProcesses.empty()){
                                memory[i].state = USING_HDD;
                            }
                            else{
                                memory[i].state = WAITING_FOR_HDD;
                            }
                            hdds[desiredHDD].hddProcesses.push_back(memory[i]);
                        }
                    }
                }
                else{
                    std::cout << "The time slice is done but there is no other processes that require the CPU!" << std::endl;
                }
            }
            else if(currentRunningProcessType == COMMON){ //Checks if current process is COMMON
                if(realTimeProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessType = REALTIME;
                    currentRunningProcessPID = realTimeProcessesQueue[0].pid;
                    if(realTimeProcessesQueue.size() == 1){
                        realTimeProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < realTimeProcessesQueue.size(); k++){
                            realTimeProcessesQueue[k-1] = realTimeProcessesQueue[k];
                            realTimeProcessesQueue.erase(realTimeProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            if(hdds[desiredHDD].hddProcesses.empty()){
                                memory[i].state = USING_HDD;
                            }
                            else{
                                memory[i].state = WAITING_FOR_HDD;
                            }
                            hdds[desiredHDD].hddProcesses.push_back(memory[i]);
                        }
                    }
                }
                else if(commonProcessesQueue.size() > 0){
                    long long int oldPID = currentRunningProcessPID;
                    currentRunningProcessPID = commonProcessesQueue[0].pid;
                    currentRunningProcessType = COMMON;
                    if(commonProcessesQueue.size() == 1){
                        commonProcessesQueue.pop_back();
                    }
                    else{
                        for(int k = 1; k < commonProcessesQueue.size(); k++){
                            commonProcessesQueue[k-1] = commonProcessesQueue[k];
                            commonProcessesQueue.erase(commonProcessesQueue.begin() + k);
                        }
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == oldPID){
                            if(hdds[desiredHDD].hddProcesses.empty()){
                                memory[i].state = USING_HDD;
                            }
                            else{
                                memory[i].state = WAITING_FOR_HDD;
                            }
                            hdds[desiredHDD].hddProcesses.push_back(memory[i]);
                        }
                    }
                }
                else{
                    std::cout << "The time slice is done but there is no other processes that require the CPU!" << std::endl;
                }
            } 
        }
        else if(instruction.substr(0,1) == "D"){ //Process using given HDD is done and can return to CPU
            int desiredHDD = -1;
            std::cin >> desiredHDD;
            if(currentRunningProcessPID < 1 && hdds[desiredHDD].hddProcesses.size() > 0){ //If CPU is idle
                currentRunningProcessPID = hdds[desiredHDD].hddProcesses[0].pid;
                currentRunningProcessType = hdds[desiredHDD].hddProcesses[0].type;
                if(hdds[desiredHDD].hddProcesses.size() == 1){
                    hdds[desiredHDD].hddProcesses.pop_back();
                }
                else{
                    /*for(int k = 1; k < hdds[desiredHDD].hddProcesses.size(); k++){
                        hdds[desiredHDD].hddProcesses[k-1] = hdds[desiredHDD].hddProcesses[k];
                        hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin() + k);
                    }*/
                    hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin());
                    hdds[desiredHDD].hddProcesses[0].state = USING_HDD;
                }
                for(int i = 0; i < memory.size(); i++){
                    if(memory[i].pid == currentRunningProcessPID){
                        memory[i].state = RUNNING;
                    }
                    else if(memory[i].pid == hdds[desiredHDD].hddProcesses[0].pid){
                        memory[i].state = USING_HDD;
                    }
                }
            }
            else if(currentRunningProcessType == COMMON){ //Checks if the current process in CPU is COMMON
                if(hdds[desiredHDD].hddProcesses[0].type == REALTIME){ //Checks if the process that just finished using HDD is REALTIME
                    long long int prevID = currentRunningProcessPID;
                    currentRunningProcessPID = hdds[desiredHDD].hddProcesses[0].pid;
                    currentRunningProcessType = hdds[desiredHDD].hddProcesses[0].type;
                    if(hdds[desiredHDD].hddProcesses.size() == 1){
                        hdds[desiredHDD].hddProcesses.pop_back();
                    }
                    else{
                        /*for(int k = 1; k < hdds[desiredHDD].hddProcesses.size(); k++){
                            hdds[desiredHDD].hddProcesses[k-1] = hdds[desiredHDD].hddProcesses[k];
                            hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin() + k);
                        }*/
                        hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin());
                        hdds[desiredHDD].hddProcesses[0].state = USING_HDD;
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == currentRunningProcessPID){
                            memory[i].state = RUNNING;
                        }
                        else if(memory[i].pid == hdds[desiredHDD].hddProcesses[0].pid){
                            memory[i].state = USING_HDD;
                        }
                        else if(memory[i].pid == prevID){
                            commonProcessesQueue.insert(commonProcessesQueue.begin(),memory[i]);
                        }
                    }
                }
                else if(hdds[desiredHDD].hddProcesses[0].type == COMMON){ //Checks if the process that just finished using HDD is REALTIME
                    long long int finishedProcessPID = hdds[desiredHDD].hddProcesses[0].pid;
                    if(hdds[desiredHDD].hddProcesses.size() == 1){
                        hdds[desiredHDD].hddProcesses.pop_back();
                    }
                    else{
                        /*for(int k = 1; k < hdds[desiredHDD].hddProcesses.size(); k++){
                        hdds[desiredHDD].hddProcesses[k-1] = hdds[desiredHDD].hddProcesses[k];
                        hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin() + k);
                        }*/
                        hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin());
                        hdds[desiredHDD].hddProcesses[0].state = USING_HDD;
                    }
                    for(int i = 0; i < memory.size(); i++){
                        if(memory[i].pid == finishedProcessPID){
                            memory[i].state = WAITING;
                            commonProcessesQueue.push_back(memory[i]);
                        }
                        else if(memory[i].pid == hdds[desiredHDD].hddProcesses[0].pid){
                            memory[i].state = USING_HDD;
                        }
                    }
                }
            }
            else if(currentRunningProcessType == REALTIME){
                long long int finishedProcessPID = hdds[desiredHDD].hddProcesses[0].pid;
                if(hdds[desiredHDD].hddProcesses.size() == 1){
                    hdds[desiredHDD].hddProcesses.pop_back();
                }
                else{
                    /*for(int k = 1; k < hdds[desiredHDD].hddProcesses.size(); k++){
                        hdds[desiredHDD].hddProcesses[k-1] = hdds[desiredHDD].hddProcesses[k];
                        hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin() + k);
                    }*/
                    hdds[desiredHDD].hddProcesses.erase(hdds[desiredHDD].hddProcesses.begin());
                    hdds[desiredHDD].hddProcesses[0].state = USING_HDD;
                }
                for(int i = 0; i < memory.size(); i++){
                    if(memory[i].pid == finishedProcessPID){
                        memory[i].state = WAITING;
                        if(memory[i].type == REALTIME){
                            realTimeProcessesQueue.push_back(memory[i]);
                        }
                        else if(memory[i].type == COMMON){
                            commonProcessesQueue.push_back(memory[i]);
                        }
                    }
                    else if(memory[i].pid == hdds[desiredHDD].hddProcesses[0].pid){
                        memory[i].state = USING_HDD;
                    }
                }
            }
            else{
                std::cout << "UNKOWN ERROR---PLEASE CONTACT THE NSA FOR MORE INFORMATION" << std::endl;
            }
        }
        else if(instruction.substr(0,1) == "S"){ //Print
            std::cin >> instruction;
            if(instruction.substr(0,1) == "r"){ //Print the process currently using the CPU and the ready queues
                if(currentRunningProcessPID <= 0){
                    std::cout << "CPU: idle" << std::endl;
                }
                else{
                    std::cout << "CPU: P" << currentRunningProcessPID;
                }
                std::cout << std::endl << "RT-queue: ";
                if(realTimeProcessesQueue.empty()){
                    std::cout << "empty";// << std::endl;
                }
                else{
                    std::cout << "P" << realTimeProcessesQueue[0].pid << std::endl;
                    for(int i = 1; i < realTimeProcessesQueue.size(); i++){
                        std::cout << " <- " << realTimeProcessesQueue[i].pid;
                    }
                }
                std::cout << std::endl;
                std::cout << "Common-queue: ";
                if(commonProcessesQueue.empty()){
                    std::cout << "empty" << std::endl;
                }
                else{
                    std::cout << "P" << commonProcessesQueue[0].pid;// << std::endl;
                    for(int i = 1; i < commonProcessesQueue.size(); i++){
                        std::cout << " <- P" << commonProcessesQueue[i].pid;
                    }
                }
                std::cout << std::endl;
            }
            else if(instruction.substr(0,1) == "i"){ //Print the processes using which HDDs and their respective ready queues
                for(HardDisk h : hdds){
                    if(h.hddProcesses.size() > 0){
                        std::cout << "HDD " << h.number << ": P" << h.hddProcesses[0].pid << std::endl;
                        std::cout << "I/O " << h.number << ": ";
                        if(h.hddProcesses.size() > 1){
                            std::cout << "P" << h.hddProcesses[1].pid;// << std::endl;
                            for(int i = 2; i < h.hddProcesses.size(); i++){
                                std::cout << "<- P" << h.hddProcesses[i].pid;
                            } 
                            std::cout << std::endl;
                        }
                        else{
                            std::cout << "empty" << std::endl;
                        }
                    }
                }
            }
            else if(instruction.substr(0,1) == "m"){ //PRint the processes in memory and their respective memory addresses
                for(int i = 0; i < memory.size(); i++){
                    std::cout << "\t";
                    std::cout << memory[i].beginningPositionInMemory << "-" << memory[i].endPositionInMemory;
                    std::cout << " P" << memory[i].pid << std::endl;
                }
            }
        }
        else{
            std::cout << "ERROR! ERROR! INVALID INSTRUCTION!" << std::endl;
            std::cout << "REBOOT REQUIRED" << std::endl;
            std::cout << "REBOOTING";
            std::cout << " ."; 
            std::cout << " ."; 
            std::cout << " ."; 
            std::cout << " ."; 
            std::cout << " ."; 
            std::cout << " .";
            std::cout << " ." << std::endl;
            std::cout << "REBOOT COMPLETE" << std::endl;
            std::cout << "YOU MAY RESUME TESTING" << std::endl;
        }
    }
}