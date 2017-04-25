/*
 * Task.h
 * Author: Sachin Edlabadkar
 */

#ifndef TASK_H_
#define TASK_H_

#include <iostream>
#include <cstdio>
#include <functional>
#include <chrono>
#include <ctime>
#include <mutex>
#include "db.h"

using namespace std;

template <class E>
    using function_type = typename std::function<E()>;


template <class T, class E>
class Task {
public:
    typedef T clock_type;
    typedef typename T::time_point time_point;
    typedef typename chrono::seconds duration;

    int taskID;//Task ID
    string taskName;//Optional
    function_type<E> func;
    DB& dbInstance = DB::getInstance();
    time_point startTime;
    duration repeatDuration;

    Task(int taskID, int interval){
        this->repeatDuration = (duration)interval;
        this->taskID = taskID;
        this->taskName = "Random Task";
        this->startTime = chrono::system_clock::now();
        string query = string ("SELECT id from tasks where id = " + to_string(this->taskID));
        if (!dbInstance.execQuery(query)){
            dbInstance.execQuery(string ("INSERT INTO tasks VALUES(" + to_string(this->taskID) +", '" + this->taskName + "')"));
        }
    };
	Task(int taskID, int interval, function_type<E> func){
	    this->repeatDuration = (duration)(interval);
	    this->taskID = taskID;
	    this->taskName = "Task-" + to_string(taskID);
	    this->func = func;
	    this->startTime = chrono::system_clock::now();
        string query = string ("SELECT id from tasks where id = " + to_string(this->taskID));
        if (!dbInstance.execQuery(query)){
            dbInstance.execQuery(string ("INSERT INTO tasks VALUES(" + to_string(this->taskID) +", '" + this->taskName + "')"));
        }
	};
	Task(int taskID, string name, int interval, function_type<E> func){
	    this->repeatDuration = (duration)(interval);
	    this->taskName = name;
	    this->taskID = taskID;
	    this->func = func;
	    this->startTime = chrono::system_clock::now();
        string query = string ("SELECT id from tasks where id = " + to_string(this->taskID));
        if (!dbInstance.execQuery(query)){
            dbInstance.execQuery(string ("INSERT INTO tasks VALUES(" + to_string(this->taskID) +", '" + this->taskName + "')"));
        }
	};
	void SetInterval(int newInterval){
	    this->repeatDuration = (duration)(newInterval);
	};
	virtual ~Task(){

	};

	bool operator<(const Task& other) const {
        return startTime < other.startTime;
    }

	void wrapperGenericFunc(){
	    time_point rt = chrono::system_clock::now();
	    time_t rt_t = chrono::system_clock::to_time_t(rt);
	    runTime = (ctime(&rt_t));
	    val = this->func(); //Execute the task's actual function
	    string query = string ("SELECT max(run_metric) FROM task_data GROUP BY id HAVING id = " + to_string(this->taskID));
        if (dbInstance.execQuery(query)){
            maxVal = dbInstance.result;
        }
        query =string("INSERT INTO task_data VALUES(" + to_string(this->taskID) + ", '" + runTime + "', " + to_string(val) + ", " + to_string(maxVal) + ")");
        if (dbInstance.execQuery(query)){
            cout << "Added Row:" << endl;
            cout << "ID: " << this->taskID <<  " val: " << val << " max: " << maxVal << " time: " << runTime << endl;
        }

	};

private:
	double val = 0.0;
	double maxVal = 0.0;
	string runTime = "";
};

#endif /* TASK_H_ */
