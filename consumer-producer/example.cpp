#include <iostream>
#include "ConsumerProducerQueue.hpp"

int main() {

    ConsumerProducerQueue<std::string> bounded_buffer(50);
    boost::mutex print_lock;

    boost::thread producer_thread1( [&]() {
        for(int i = 0; i < 100; i++) {
            bounded_buffer.produce("[" + std::to_string(i) + "]");
        }
    });

    boost::thread producer_thread2( [&]() {
        for(int i = 0; i > -100; i--) {
            bounded_buffer.produce("[" + std::to_string(i) + "]");
        }
    });

    boost::thread consumer_thread1( [&]() {
        while(1) {
            print_lock.lock();
            std::cout << "consumer1: " << bounded_buffer.consume() 
                      << " [queue size]" << bounded_buffer.size() << std::endl;
            print_lock.unlock();
            boost::this_thread::yield();
        }
    });

    boost::thread consumer_thread2( [&]() {
        while(1) {
            print_lock.lock();
            std::cout << "consumer2: " << bounded_buffer.consume() 
                      << " [queue size]" << bounded_buffer.size()<< std::endl;
            print_lock.unlock();
            boost::this_thread::yield();
        }
    });



    producer_thread1.join();
    producer_thread2.join();
    consumer_thread1.join();
    consumer_thread2.join();

    return 0;

}