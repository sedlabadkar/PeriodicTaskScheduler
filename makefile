all:
	g++ -pthread -std=c++11 main.cpp -lsqlite3 -o periodicTaskScheduler
clean:
	rm periodicTaskScheduler
