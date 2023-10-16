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

    // here is where we can parallelize the code
	int LastId = MaxThreads - 1;

	// ok so first we need to loop through he threads at each pass
	for (int id = 0; id < MaxThreads; ++id){
		if (id < 5) {
			std::thread th{[&, id]() {
				Number number;
				size_t iter = 0;
				while(data.getNext(number)) {
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
				barrier.arrive_and_wait();
			}};
			(id < LastId) ? th.detach() : th.join();
		}
		else {
			std::thread th{[&, id]() {
				Number number;
				size_t iter = 0;
				while(data.getNext(number)) {
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
				barrier.arrive_and_wait();
			}};
			(id < LastId) ? th.detach() : th.join();
		}

        
		/*
        // need to divide the the data into chunks that will be given to the threads.
        size_t chunkSize = int(data.size() / MaxThreads + 1);
        
        std::thread t{[&,id]() {
            auto start = id * chunkSize;
            auto end = std::min(data.size(), start + chunkSize);
            for(auto i = start; i < end; ++i) {
                size_t iter = 0;
                Number number = data[i];
                Number n = number;
                while (!n.is_palindrome() && ++iter < MaxIterations) {
					Number sum(n.size());   // Value used to store current sum of digits
					Number r = n.reverse(); // reverse the digits of the value
					auto rd = n.begin(); 
					bool carry = false;  // flag to indicate if we had a carry
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
        barrier.arrive_and_wait();
        }};
		*/
        //(id < LastId) ? t.detach() : t.join();
	}
        // Update our records.  First, determine if we have a new
        //   maximum number of iterations that isn't the control limit
        //   (MaxIterations) for a particular number.  If we're less
        //   tha the current maximum (maxIter) or we've exceeded the number
        //   of permissible iterations, ignore the current result and move
        //   onto the next number.
        //if (iter < maxIter || iter == MaxIterations) { continue; }

        // Otherwise update our records, which possibly means discarding
        //   our current maximum and rebuilding our records list.
        //Record record{number, n};
        //if (iter > maxIter) {
        //    records.clear();
        //    maxIter = iter;
       // }

        //records.push_back(record);
    
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
