/*
 *  main.cpp
 *  Author: Sachin Edlabadkar
 */

#include <iostream>
#include <cstdio>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "TaskScheduler.h"

typedef scheduling::TaskScheduler<std::chrono::system_clock, double> task_scheduler;

//Current Virtual Memory Usage
double getTotalVirtualMemoryUsedInfo(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    double totalVirtualMemUsed = memInfo.totalram - memInfo.freeram;
    totalVirtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    totalVirtualMemUsed *= memInfo.mem_unit;
    return totalVirtualMemUsed;
}

//Current physical memory usage helper
int helper(char* line){
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

//Current Physical memory usage
double getCurrentPhysicalMemoryUsage(){
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = helper(line);
            break;
        }
    }
    fclose(file);
    return (double)result;
}

void startScheduler(task_scheduler *ts){
    ts->addTask(1, 30, getTotalVirtualMemoryUsedInfo);
    ts->addTask(2, 60, getCurrentPhysicalMemoryUsage);
    //ts->editTask(1, 35);
    ts->run();
}

int main(int argc, char* argv[]){
    task_scheduler *ts = new task_scheduler(5);//5 threads in the pool
    std::thread t([ts]{startScheduler(ts);});
    t.detach();
    cout << "\nTask to Get total virtual memory usages added to scheduler with id 1 and interval 30 seconds" << endl;
    cout << "Task to Get physical memory usage by current process added to scheduler with id 2 and interval 60 seconds" << endl;

	//Input loop
    while(true){
        cout << "\nChoose option" << endl;
        cout << "1. Edit Task" << endl;
        cout << "2. Remove Task" << endl;
        cout << "3. Exit \n" << endl;
        string line;
        getline(cin, line);
        if (line == "1"){
            int taskID, interval;
            cout << "TaskID:";
            cin >> taskID;
            cout << "New repeat interval:";
            cin >> interval;
            if (ts->editTask(taskID, interval)){
                cout << " Task Changed successfully " << endl;
            } else {
                cout << " Failed to update task " << endl;
            }
        } else if (line == "2"){
            int taskID;
            cout << "TaskID:";
            cin >> taskID;
            if (ts->removeTask(taskID)){
                cout << "Task removed" << endl;
            } else {
                cout << "Task removal failed" << endl;
            }
        } else if  (line == "3"){
            ts->~TaskScheduler();
            return 0;
        } else {
            cout << "Invalid Option" << endl;
        }
    }
    return -1;
}
