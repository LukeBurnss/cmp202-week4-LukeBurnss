// Simple threading example
// Adam Sampson <a.sampson@abertay.ac.uk>

#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// Import things we need from the standard library
using std::chrono::seconds;
using std::cout;
using std::endl;
using std::ofstream;
using std::this_thread::sleep_for;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::unique_lock;

//global variables
int result;
mutex result_mutex;
condition_variable result_cv;
bool result_ready = false;

void producer()
{
	unique_lock<mutex> lock(result_mutex);
	result = 42; // Example computation
	result_ready = true;
	result_cv.notify_one();
}

void consumer()
{
	unique_lock<mutex> lock(result_mutex);
	while (!result_ready)
	{
		result_cv.wait(lock);
	}
	cout << "Result is " << result << endl;
}

int main(int argc, char *argv[])
{
	thread producerThread(producer);
	thread consumerThread(consumer);

	producerThread.join();
	consumerThread.join();

	return 0;
}
