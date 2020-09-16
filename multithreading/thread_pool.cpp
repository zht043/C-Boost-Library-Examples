#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>



//-------------------------------------------------------------------------------------------------------------------//

/* reference: https://kezunlin.me/post/f241bd30/ */

class ThreadPool {
public:
    ThreadPool(unsigned int num_threads) : io_work(ios) {
        for (int i = 0; i < num_threads; i++) {
            threads.create_thread(boost::bind(&boost::asio::io_service::run, &ios));
        }
    }

    ~ThreadPool() {
        ios.stop();
        try { // suppress all exceptions 
            threads.join_all(); // wait for all threads to terminate
        }
        catch ( const std::exception& ) {}
    }

    template<class Function>
    void execute(Function func) {
        ios.post(func); // add the function to the io_service queue 
                        // to be run in the threads created in the constructor
        // non-blocking, return immediately

        /* if there aren't available threads in the pool, i.e. every
           thread in the pool is already busy executing some other 
           function, the new coming thread has to wait until one of 
           the running thread is finished. */

        /* No counter provided to track the number of free threads
           due to implementation overheads with wrapping func with 
           a counter addition when function finishes, and at the same
           time supporting boost::bind externally for passing func. 
           Refer to this post for more details: 
                https://stackoverflow.com/questions/12215395/thread-pool-using-boost-asio/12267138#12267138
        */
    }

private:
    boost::thread_group threads;
    boost::asio::io_service ios;
    boost::asio::io_service::work io_work;
};

//-------------------------------------------------------------------------------------------------------------------//









void delay(unsigned int milliseconds) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(milliseconds));
}





int main(void) {

    ThreadPool thread_pool(3);

    boost::mutex mu;

    thread_pool.execute([&](){
        mu.lock();
        std::cout << "Thread 1 Executing" << std::endl;
        mu.unlock();

        delay(1000); // wait 1 second

        mu.lock();
        std::cout << "Thread 1 Finished" << std::endl;
        mu.unlock();
    });

    thread_pool.execute([&](){
        mu.lock();
        std::cout << "Thread 2 Executing" << std::endl;
        mu.unlock();
        delay(500); // wait 0.5 second

        mu.lock();
        std::cout << "Thread 2 Finished" << std::endl;
        mu.unlock();
    });

    thread_pool.execute([&](){
        mu.lock();
        std::cout << "Thread 3 Executing" << std::endl;
        mu.unlock();
        delay(1000); // wait 1 second

        mu.lock();
        std::cout << "Thread 3 Finished" << std::endl;
        mu.unlock();
    });

    thread_pool.execute([&](){
        mu.lock();
        std::cout << "Thread 4 Executing" << std::endl;
        mu.unlock();
        delay(1000); // wait 1 second

        mu.lock();
        std::cout << "Thread 4 Finished" << std::endl;
        mu.unlock();
    });


    delay(2000); // wait 2 seconds until all tasks are finished
    /* can't directly join the threads because in a thread pool, 
     * all threads shall exist until the entire program is finished
     */
    return 0;
}