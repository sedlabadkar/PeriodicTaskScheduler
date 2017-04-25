/*
 * TaskScheduler.h
 * Author: Sachin Edlabadkar
 */

using namespace std;

#ifndef TASKSCHEDULER_H_
#define TASKSCHEDULER_H_
#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <algorithm>
#include <list>
#include "Task.h"

#define NUM_THREADS 5

namespace scheduling{
template <class T, class E>
    class TaskScheduler {
    private:
        int taskCount = 0;
        vector<thread> threads; //Thread pool
        std::mutex mutex;
        std::condition_variable task_updated; //A task that we execute has changed
        std::condition_variable task_available_to_execute; //New task is available to execute. Wake threads

        std::list<Task<T, E>> task_list; //List of all tasts that scheduler executes
        std::deque<Task<T, E>> tasksExecQueue; //Queue for tasks waiting to execute - Threads pick tasks from here
        typedef typename std::list<Task<T, E>>::iterator iterator_task;

        DB& dbInstance = DB::getInstance();

        void thread_run(){
            while (true) {
                {
                std::unique_lock<std::mutex> lk(mutex);
                while (this->tasksExecQueue.empty()) {
                    task_available_to_execute.wait(lk);
                }
                this->tasksExecQueue.front().wrapperGenericFunc();
                this->tasksExecQueue.pop_front(); //Removed this task from the exec queue
                }
            }
        };

        int getNextTaskID(){
            string query = string ("SELECT max(id) FROM tasks");
            if (dbInstance.execQuery(query)){
                taskCount = dbInstance.result + 1;
            }
            return taskCount;
        }
    public:
        bool run(){
            {
                std::lock_guard<std::mutex> lk(mutex);
                for (auto& t : threads) { //Add threads to scheduler
                    t = std::thread([this]{this->thread_run();});
                    t.detach();
                }
            }
            while (true) {
                std::unique_lock<std::mutex> lk(mutex);
                auto nextTask = std::min_element(task_list.begin(), task_list.end()); //Get next task;
                auto next_task_start_time = nextTask == task_list.end()? Task<T, E>::time_point::max() : (*nextTask).startTime;

                if (this->task_updated.wait_until(lk, next_task_start_time) == std::cv_status::timeout) {
                    //Not a one time task
                    if (nextTask->repeatDuration != chrono::seconds::zero()) {
                        nextTask->startTime += nextTask->repeatDuration;
                    } else {
                        task_list.erase(nextTask);
                    }
                    this->tasksExecQueue.push_back(*nextTask);
                    task_available_to_execute.notify_all(); //All threads are notified
                }
            }
            return true;
        };
        TaskScheduler(){
            threads = vector<thread>(NUM_THREADS);
            dbInstance.execQuery("CREATE TABLE IF NOT EXISTS tasks(id INTEGER, name TEXT, PRIMARY KEY(id))");
            dbInstance.execQuery("CREATE TABLE IF NOT EXISTS task_data(id INTEGER, run_time TEXT, run_metric REAL, max_so_far REAL)");
        };
        TaskScheduler(int numThreads){
            threads = vector<thread>(numThreads);
            dbInstance.execQuery("CREATE TABLE IF NOT EXISTS tasks(id INTEGER, name TEXT, PRIMARY KEY(id))");
            dbInstance.execQuery("CREATE TABLE IF NOT EXISTS task_data(id INTEGER, run_time TEXT, run_metric REAL, max_so_far REAL)");
        };
        virtual ~TaskScheduler(){
            cout << "exiting" << endl;
        };

        int addTask(string name, int interval, function_type<E> func){
            Task<T, E> newTask(getNextTaskID(), name, interval, func);
            std::lock_guard<std::mutex> lk(mutex);
            task_list.push_back(newTask);
            this->task_updated.notify_all();
            return this->taskCount;

        };
        int addTask(int interval, function_type<E> func){
            Task<T, E> *newTask = new Task<T, E>(getNextTaskID(), interval, func);
            std::lock_guard<std::mutex> lk(mutex);
            task_list.push_back(*newTask);
            this->task_updated.notify_all();
            return this->taskCount;
        };
        int addTask(int id, int interval, function_type<E> func){
            Task<T, E> *newTask = new Task<T, E>(id, interval, func);
            std::lock_guard<std::mutex> lk(mutex);
            task_list.push_back(*newTask);
            this->task_updated.notify_all();
            return this->taskCount;
        };
        //edit task
        bool editTask(int taskID, int newInterval){
            iterator_task task_it = task_list.begin();
            while(task_it != task_list.end()){
                if (task_it->taskID == taskID){
                    task_it->SetInterval(newInterval);
                    return true;
                }
                task_it++;
            }
            return false;
        };

        bool removeTask(int taskID){
            iterator_task task_it= task_list.begin();
            while (task_it!= task_list.end())
            {
                if (task_it->taskID == taskID){
                    task_it = task_list.erase(task_it);
                    return true;
                } else {
                    task_it++;
                }
            }
            return false;
        };
    };
}
#endif /* TASKSCHEDULER_H_ */
