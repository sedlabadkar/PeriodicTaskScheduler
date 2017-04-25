Periodic Task Scheduler
========================

## What is it?

Periodic task scheduler is a simple task scheduler written in C++. It is capable of running multiple tasks at different, configurable intervals of Once per N seconds. Allows for addition of new tasks, cancelling existing tasks and reconfiguring existing tasks. For each run of the task, the output is stored in SQLite DB. 

## How to use?

1. #include "TaskScheduler.h"
2. Get taskScheduler object. The TaskScheduler class template signature is as follows:
    template <class T, class E>
    where,  
        class T = std::chrono::system_clock or any other clock type in std::chrono
        class E = Return type of tasks to be executed
3. To add new task, call the addTask method with repeat interval and task function as input.
4. To edit an existing task, call the edit task method with taskID and new interval as input. 
5. To remove a task from active list, call the remove task method with taskID as input. 

The included main.cpp might serve as an example. It provides a runtime terminal based menu to let user edit/remove tasks. By default, it adds two tasks to the scheduler and configures the scheduler to execute them every 30 and 60 seconds. These times can be changed at runtime by proving the taskID(1 or 2) and new time interval.

The two tasks added are to calculate total virtual memory usage and current physical memory usage. The tasks are defined by the methods getTotalVirtualMemoryUsedInfo() and getCurrentPhysicalMemoryUsage() in main.cpp.

To build and run the main.cpp example, use make command. A simplistic makefile is included with rules to build make all and make clean.

The following compiler options are needed to build the task scheduler successfully: 
$ g++ -std=c++11 -pthread <source files> -lsqlite3

To add a new task to the scheduler, the corresponding function must be implemented and passed in as an argument to addTask method. Currently, the scheduler only supports methods with return type as Class E (Defined in the TaskScheduler template). 