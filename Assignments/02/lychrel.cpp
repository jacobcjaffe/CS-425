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
	std::atomic<size_t> _index = 0;
    // here is where we can parallelize the code
	int LastId = MaxThreads - 1;

	const int numRetrieved = 6;
	// ok so first we need to loop through he threads at each pass
	for (int id = 0; id < MaxThreads; ++id){
		std::thread th{[&, id]() {
			std::vector<Number> nums;
			size_t iter = 0;
			size_t ind;
			while(data.getNext(numRetrieved, nums)) {
				for (int i = 0; i < nums.size(); i++) {
					Number number = nums[i];
					Number n = number;
					iter = 0;
					
					// check if n is a palindrome
					while (!n.is_palindrome() && (++iter < MaxIterations)) {
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

					// lock
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
				nums.clear();
			}
			barrier.arrive_and_wait();
		}};
		(id < LastId) ? th.detach() : th.join();
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
