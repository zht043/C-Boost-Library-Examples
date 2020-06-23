
#include <iostream> 
#include <mutex>
#include <boost/thread.hpp> 
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>

using namespace boost;


double millis(void) {
    auto t = chrono::high_resolution_clock::now();
    return double(t.time_since_epoch().count()) / 1000000.00f;
}


void delay(int delay_ms) {
    this_thread::sleep_for(chrono::milliseconds(delay_ms));
}


void my_func()
{
    std::cout << "detach-分离 或 join-等待 线程" << std::endl;
}

void test1()//简单线程,线程状态joinable、detached
{
    thread t(my_func);
    thread::yield();//当前线程放弃余下的时间片。
    std::cout << t.joinable() << std::endl;
    t.join();

    thread t1(my_func);
    std::cout << t1.joinable() << std::endl;
    t1.detach();
    std::cout << t1.joinable() << std::endl;
}




mutex m;
void count(int id) {
    for (int i = 0; i < 10; ++i)
    {
        m.lock();
        std::cout << id << ": "
            << i << std::endl;
        m.unlock();
    }
}

void test2() //复杂类型对象作为参数来创建线程
{
    thread thrd1(count, 1);
    thread thrd2(count, 2);
    thrd1.join();
    thrd2.join();
}

int tick_period = 100;

void timer_callback(asio::high_resolution_timer& timer) {
    static int cnt = 0;
    std::cout << cnt++ << std::endl;
    timer.expires_from_now(std::chrono::milliseconds(tick_period));
    timer.async_wait(bind(&timer_callback, boost::ref(timer)));
}

void test3_func(asio::io_service& service) {
    
    asio::high_resolution_timer timer(service);
    timer.expires_from_now(std::chrono::milliseconds(tick_period));
    timer.async_wait(bind(&timer_callback, boost::ref(timer)));
    service.run(); // blocking 
}

void test3() {
    std::cout << "=========================" << std::endl;
    asio::io_service service;
    thread thrd(test3_func, boost::ref(service));
    delay(2000);
    service.stop();
    thrd.join();
}


int main() {
    test1();
    delay(1000);

    // mutex test
    test2();

    // async within thread test
    test3();

    return 0;
}