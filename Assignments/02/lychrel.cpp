/////////////////////////////////////////////////////////////////////////////
//
//  lychrel.cpp
//
//  A program that searches for the largest (in terms of number of
//    iterations without exceeding a specified maximum) palindrome value.
//
//  The program reports a list of numbers that share the maximum number of
//    iterations, along with the size and final palindrome number
//

#include <barrier>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>


#include "LychrelData.h"

// A structure recording the starting number, and its final palindrome.
//   An vector of these records (typedefed to "Records") is built during
//   program execution, and output before the program terminates.
struct Record {
    Number n;
    Number palindrome;
};
using Records = std::vector<Record>;

// Application specific constants
const size_t MaxIterations = 7500;
const size_t MaxThreads = 10;

//
// --- main ---
//

int main() {
    LychrelData data;

    std::cerr << "Processing " << data.size() << " values ...\n";

    size_t maxIter = 0;  // Records the current maximum number of iterations
    Records records; // list of values that took maxIter iterations

    // all the threads share the same barrier amd mutex
    std::barrier barrier{MaxThreads};
    //is how the threads communicate with eachother to tell when the variables they share are allowed to be changed by them.
    std::mutex mutex;
	std::mutex queueMutex;

	std::atomic<bool> _finished = false;
	std::queue<std::vector<Number>> vecQueue;
    // here is where we can parallelize the code
	int LastId = MaxThreads - 1;

	// ok so first we need to loop through he threads at each pass
	for (int id = 0; id < MaxThreads; ++id){
		// have two threads continuously grabbing data
		if (id == 0) {
			std::thread th{[&, id]() {
				std::vector<Number> buffer(4);
				while (data.getNext(4, buffer)){
					std::vector<Number> vect2;
					copy(buffer.begin(), buffer.end(), back_inserter(vect2));
					{
						std::lock_guard lock{queueMutex};
						vecQueue.push(vect2);
					}
				}
				_finished = true;
				barrier.arrive_and_wait();
			}};
			(id < LastId) ? th.detach() : th.join();
		}
		else {
			std::thread th{[&, id]() {
				Number number;
				size_t iter = 0;
				std::vector<Number> nums;
				while(true) {
					// check to see if thread can pull from queue
					nums.clear();
					{
						std::lock_guard lock{queueMutex};
						if (vecQueue.empty() && _finished) {
							break;
						}					
						else if (!_finished) {
							// need to wait for the queue to be filled
							std::this_thread::yield();
							continue;
						}
						else {
							copy(vecQueue.front().begin(), vecQueue.front().end(), back_inserter(nums));
							vecQueue.pop();
						}
					}
					
					for (int i = 0; i < nums.size(); i++) {
						number = nums[i];
						iter = 0;
						Number n = number;
						while (!n.is_palindrome() && ++iter < MaxIterations) {
							Number sum(n.size());
							Number r = n.reverse();
							auto rd = n.begin();
							bool carry = false;
							std::transform(n.rbegin(), n.rend(), sum.rbegin(), 
								[&](auto d) {
									auto v = d + *rd++ + carry;
									carry = v > 9;
									if (carry) { v -= 10; }
									return v;
								}
							);
							if (carry) { sum.push_front(1); }
							n = sum;
						}
						{
							std::lock_guard lock{mutex}; 
							if (!(iter < maxIter || iter == MaxIterations)){
								Record record{number, n};
								if (iter > maxIter) {
									records.clear();
									maxIter = iter;
								}
								records.push_back(record);
							}
						}
					}
				}
				barrier.arrive_and_wait();
			}};
			(id < LastId) ? th.detach() : th.join();
		}
	}
    // Output our final results
    std::cout << "\nmaximum number of iterations = " << maxIter << "\n";
    for (auto& [number, palindrome] : records) {
        //here is where we can parallelize the code 
        std::cout 
            << "\t" << number 
            << " : [" << palindrome.size() << "] "
            << palindrome << "\n";
    }
}
