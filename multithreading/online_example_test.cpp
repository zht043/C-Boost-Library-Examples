#include "crpcut.hpp"

#include <boost/thread.hpp> 
#include <iostream> 
#include <complex.h>
#include <stack>
#include <mutex>
#include <boost/thread/once.hpp>
#include <boost/thread/future.hpp>
#include <boost/asio.hpp>

void my_func()
{
    std::cout << "detach-分离 或 join-等待 线程" << std::endl;
}

TEST(BoostThread, detachOrjoin)//简单线程,线程状态joinable、detached
{
    boost::thread t(my_func);
    boost::thread::yield();//当前线程放弃余下的时间片。
    std::cout << t.joinable() << std::endl;
    t.join();

    boost::thread t1(my_func);
    std::cout << t1.joinable() << std::endl;
    t1.detach();
    std::cout << t1.joinable() << std::endl;
}

//线程的创建需要传递给thread对象一个可调用物（函数或函数对象），它必须具
//有operator()以供线程执行。
boost::mutex io_mutex;
struct count 
{
    count(int id) : id(id) { }

    void operator()()
    {
        for (int i = 0; i < 10; ++i)
        {
            boost::mutex::scoped_lock
                lock(io_mutex);
            std::cout << id << ": "
                << i << std::endl;
        }
    }

    int id;
};

TEST(BoostThread, Typeobject)//复杂类型对象作为参数来创建线程
{
    boost::thread thrd1(count(1));
    boost::thread thrd2(count(2));
    thrd1.join();
    thrd2.join();
}

class HelloWorldStatic
{
public:
    static void hello()
    {
        std::cout <<
            "Hello world, I''m a thread!"
            << std::endl;
    }
    static void start()
    {

        boost::thread thrd(hello);
        thrd.join();
    }

};

TEST(BoostThread, InClassStatic)//类内部创建线程
{
    HelloWorldStatic::start();//在这里start()和hello()方法都必须是static方法。
}

class HelloWorld
{
public:
    void hello()
    {
        std::cout <<
            "Hello world, I''m a thread!"
            << std::endl;
    }
    void start()
    {
        boost::function0< void> f = boost::bind(&HelloWorld::hello, this);
        boost::thread thrd(f);
        thrd.join();
    }

};

TEST(BoostThread, InClass)//start()和hello()方法不是静态方法则采用此方法创建线程
{
    HelloWorld hello;
    hello.start();
}

class HelloWorldOut
{
public:
    void hello(const std::string& str)
    {
        std::cout << str;
    }
};

TEST(BoostThread, OutClass)
{
    HelloWorldOut obj;
    boost::thread thrd(boost::bind(&HelloWorldOut::hello, &obj, "Hello world, I''m a thread!" ) ) ;
    thrd.join();
}

void func1(const int &id)
{
    std::cout << "func1 id : " << id << std::endl;
}

struct MyThread
{
    void operator()(const int &id)
    {
        std::cout << "MyThread id : " << id << std::endl;
    }

    void func1(const int &id)
    {
        std::cout << "MyThread::func1 id : " << id << std::endl;
    }
};

TEST(BoostThread, Threadparameters)
{
    //普通函数  
    boost::thread t1(func1, 11);
    t1.join();

    //函数对象  
    MyThread myThread;
    boost::thread t2(myThread, 22);
    t2.join();

    //成员函数  
    boost::thread t3(&MyThread::func1, myThread, 33);
    t3.join();

    //临时对象  
    boost::thread t4(MyThread(), 44);
    t4.join();

    //对象引用  
    boost::thread t5(boost::ref(myThread), 55);
    t5.join();
}

void wait(int seconds)
{
    boost::this_thread::sleep(boost::posix_time::seconds(seconds));
}

void threadinterrupt()
{
    try
    {
        for (int i = 0; i < 4; ++i)
        {
            wait(1);
            std::cout << i<<" 是否允许中断:"<< boost::this_thread::interruption_enabled()<<" 是否被要求中断:"<< boost::this_thread::interruption_requested() << std::endl;
        }
    }
    catch (boost::thread_interrupted&)
    {
        std::cout << "已经中断" << std::endl;
    }
}

#define testinterruption() { std::cout << "\r\n是否允许中断:" << boost::this_thread::interruption_enabled() << " 是否被要求中断:" << boost::this_thread::interruption_requested()<< __FUNCTION__<< __LINE__ << std::endl;}

void f()
{
    testinterruption();
    // interruption enabled here
    {
        boost::this_thread::disable_interruption di;
        boost::this_thread::interruption_point();
        testinterruption();
        // interruption disabled
        {
            testinterruption();
            boost::this_thread::disable_interruption di2;
            testinterruption();
            // interruption still disabled
        } // di2 destroyed, interruption state restored
          // interruption still disabled
        boost::this_thread::interruption_point();
        testinterruption();
    } // di destroyed, interruption state restored
      // interruption now enabled
    testinterruption();
    boost::this_thread::interruption_point();
    testinterruption();
}

void g()
{
    testinterruption();
    // interruption enabled here
    {
        testinterruption();
        boost::this_thread::disable_interruption di;
        boost::this_thread::interruption_point();
        testinterruption();
        // interruption disabled
        {
            testinterruption();
            boost::this_thread::restore_interruption ri(di);
            testinterruption();
            // interruption now enabled
        } // ri destroyed, interruption disable again
        testinterruption();
        boost::this_thread::interruption_point();
    } // di destroyed, interruption state restored
      // interruption now enabled
    testinterruption();
    boost::this_thread::interruption_point();
    testinterruption();
}

//预定于中断点：
//boost::thread::join()
//boost::thread::timed_join()
//boost::thread::try_join_for(),
//boost::thread::try_join_until(),
//boost::condition_variable::wait()
//boost::condition_variable::timed_wait()
//boost::condition_variable::wait_for()
//boost::condition_variable::wait_until()
//boost::condition_variable_any::wait()
//boost::condition_variable_any::timed_wait()
//boost::condition_variable_any::wait_for()
//boost::condition_variable_any::wait_until()
//boost::thread::sleep()
//boost::this_thread::sleep_for()
//boost::this_thread::sleep_until()
//boost::this_thread::interruption_point()

TEST(BoostThread, interrupt)//中断
{
    std::cout << "中断" << std::endl;
    boost::thread t(threadinterrupt);
    wait(3);
    t.interrupt();
    t.join();

    boost::thread tf(f);
    tf.interrupt();
    tf.join();

    boost::thread tg(g);
    tg.interrupt();
    tg.join();
}

TEST(BoostThread, threadid_count)
{
    std::cout << boost::this_thread::get_id() << std::endl;
    std::cout << boost::thread::hardware_concurrency() << std::endl;
}

void thread_group_Fun()
{
    std::cout <<"当前线程ID:"<< boost::this_thread::get_id() << std::endl;
}

TEST(BoostThread, thread_group)
{
    boost::thread_group grp;
    boost::thread *p = new boost::thread(thread_group_Fun);
    grp.add_thread(p);
    std::cout << "remove_thread:" << p->get_id()<< std::endl;
    grp.remove_thread(p);

    grp.create_thread(thread_group_Fun);
    grp.create_thread(thread_group_Fun);   

    grp.join_all();                 
}

class BankAccount {
    boost::mutex mtx_; // explicit mutex declaration 
    int balance_;
public:
    void Deposit(int amount) {
        boost::lock_guard<boost::mutex> guard(mtx_);
        balance_ += amount;
    }
    void Withdraw(int amount) {
        boost::lock_guard<boost::mutex> guard(mtx_);
        balance_ -= amount;
    }
    int GetBalance() {
        boost::lock_guard<boost::mutex> guard(mtx_);
        return balance_;
    }
};

BankAccount JoesAccount;

void bankAgent()
{
    for (int i = 10; i > 0; --i) {
        JoesAccount.Deposit(500);
        std::cout << "bankAgent:" << JoesAccount.GetBalance() << std::endl;
    }
}

void Joe() {
    for (int i = 10; i > 0; --i) {
        JoesAccount.Withdraw(100);
        std::cout <<"Joe:"<< JoesAccount.GetBalance() << std::endl;
    }
}

TEST(BoostThread, mutex)//互斥
{
    boost::thread thread1(bankAgent); // start concurrent execution of bankAgent
    boost::thread thread2(Joe); // start concurrent execution of Joe
    thread1.join();
    thread2.join();
}

typedef boost::shared_lock<boost::shared_mutex> readLock;
typedef boost::unique_lock<boost::shared_mutex> writeLock;
boost::shared_mutex rwmutex;

std::vector<int> random_numbers;

void readOnly()
{
    for (int i = 0; i < 3; ++i)
    {
        wait(1);
        readLock rdlock(rwmutex);
        std::cout << random_numbers.back() << std::endl;
    }
}

void writeOnly()
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    for (int i = 0; i < 3; ++i)
    {
        writeLock wtlock(rwmutex);
        random_numbers.push_back(std::rand());
        wait(1);
    }
}

int sum = 0;

void count()
{
    for (int i = 0; i < 3; ++i)
    {
        wait(1);
        readLock rdlock(rwmutex);
        sum += random_numbers.back();
    }
}

TEST(BoostThread, readwrite)//读写锁
{
    boost::thread t1(writeOnly);
    boost::thread t2(readOnly);
    boost::thread t3(count);
    t1.join();
    t2.join();
    t3.join();
    std::cout << "Sum: " << sum << std::endl;
}

boost::condition_variable_any cond;
boost::mutex rwmutexCV;
std::vector<int> random_numbersCV;

void print()
{
    std::size_t next_size = 1;
    for (int i = 0; i < 3; ++i)
    {
        boost::unique_lock<boost::mutex> lock(rwmutexCV);
        while (random_numbersCV.size() != next_size)
            cond.wait(rwmutexCV);
        std::cout << random_numbersCV.back() << std::endl;
        ++next_size;
        cond.notify_all();
    }
}

void fill()
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    for (int i = 0; i < 3; ++i)
    {
        boost::unique_lock<boost::mutex> lock(rwmutexCV);
        random_numbersCV.push_back(std::rand());
        cond.notify_all();
        cond.wait(rwmutexCV);
    }
}

TEST(BoostThread, ConditionVariables)//条件变量
{
    boost::thread t1(fill);
    boost::thread t2(print);
    t1.join();
    t2.join();
}

//生产者-消费者模式的后进先出型(std::stack)缓冲区
class Buffer
{
public:
    //构造函数
    Buffer(size_t n) :un_read(0), capacity(n) {}

    //写入数据x
    void put(int x) {
        //局部域
        {
            boost::mutex::scoped_lock lock(mu);  //锁定互斥量
            while (is_full()) {
                cond_put.wait(mu);  //条件变量等待
            }
            stk.push(x);  //写入数据
            ++un_read;
        }  //解锁互斥量

        cond_get.notify_one();  //通知可以读取数据
    }

    //读取数据
    void get(int *x) {

        {
            boost::mutex::scoped_lock lock(mu);
            while (is_empty()) {
                cond_get.wait(mu);
            }
            *x = stk.top();
            stk.pop();
            --un_read;
        }

        cond_put.notify_one();
    }

private:
    //判断缓冲区是否满
    bool is_full() {
        return un_read == capacity;
    }

    //判断缓冲区是否为空
    bool is_empty() {
        return un_read == 0;
    }

private:
    boost::mutex mu;  //互斥量，配合条件变量使用
    boost::condition_variable_any cond_put;  //写入条件变量
    boost::condition_variable_any cond_get;  //读取条件变量
    std::stack<int> stk;  //缓冲区对象
    int un_read;
    int capacity;
};

Buffer buf(5);  //定义一个缓冲区对象

boost::mutex io_mu_;  //定义一个输出互斥量

//生产者,n个
void producer(int n)
{
    for (int i = 0; i < n; i++) {
        //输出信息
        {
            boost::mutex::scoped_lock lock(io_mu_);
            std::cout << "put " << i << " to buffer" << std::endl;

        }
        buf.put(i);  //写入数据
    }
}

//消费者
void consumer(int n)
{
    int result(0);
    for (int i = 0; i < n; i++) {
        {
            buf.get(&result);  //读取数据
            boost::mutex::scoped_lock lock(io_mu_);
            std::cout << "get " << result << " from buffer" << std::endl;
        }
    }
}

TEST(BoostThread, producer_consumer_test)
{
    boost::thread t_producer(producer, 20);
    boost::thread t_consumer1(consumer, 10);
    boost::thread t_consumer2(consumer, 10);

    t_producer.join();
    t_consumer1.join();
    t_consumer2.join();
}

class ThreadPool {
public:
    explicit ThreadPool(size_t size) : work_(io_service_) {
        for (size_t i = 0; i < size; ++i) {
            workers_.create_thread(
                boost::bind(&boost::asio::io_service::run, &io_service_));
        }
    }

    ~ThreadPool() {
        io_service_.stop();
        workers_.join_all();
    }

    // Add new work item to the pool.
    template<class F>
    void Enqueue(F f) {
        io_service_.post(f);
    }

private:
    boost::thread_group workers_;
    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
};

TEST(BoostThread, ThreadPool)
{
    // Create a thread pool of 4 worker threads.
    ThreadPool pool(4);

    // Queue a bunch of work items.
    for (int i = 0; i < 8; ++i) {
        pool.Enqueue([i] {
            std::cout << "hello " << i << std::endl;
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            std::cout << "world " << i << std::endl;
        });
    }
}

void init_number_generator()
{
    static boost::thread_specific_ptr<bool> tls;
    if (!tls.get())
        tls.reset(new bool(false));
    if (!*tls)
    {
        *tls = true;
        std::srand(static_cast<unsigned int>(std::time(0)));
    }
}

boost::mutex mutex;

void random_number_generator()
{
    init_number_generator();
    int i = std::rand();
    boost::lock_guard<boost::mutex> lock(mutex);
    std::cout << i << std::endl;
}

TEST(BoostThread, thread_specific_ptr)//线程局部存储
{
    boost::thread t[3];

    for (int i = 0; i < 3; ++i)
        t[i] = boost::thread(random_number_generator);

    for (int i = 0; i < 3; ++i)
        t[i].join();
}

std::once_flag flag1, flag2;

void simple_do_once()
{
    std::call_once(flag1, []() { std::cout << "Simple example: called once\n"; });
}

void may_throw_function(bool do_throw)
{
    if (do_throw) {
        std::cout << "throw: call_once will retry\n"; // 这会出现多于一次
        throw std::exception();
    }
    std::cout << "Didn't throw, call_once will not attempt again\n"; // 保证一次
}

void do_once(bool do_throw)
{
    try {
        std::call_once(flag2, may_throw_function, do_throw);
    }
    catch (...) {
    }
}

TEST(StdThread, std_call_once)
{
    std::thread st1(simple_do_once);
    std::thread st2(simple_do_once);
    std::thread st3(simple_do_once);
    std::thread st4(simple_do_once);
    st1.join();
    st2.join();
    st3.join();
    st4.join();

    std::thread t1(do_once, true);
    std::thread t2(do_once, true);
    std::thread t3(do_once, false);
    std::thread t4(do_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

// Some sort of connection class that should only be initialized once
struct Conn {
    static void init() { ++i_; }
    static boost::once_flag init_;
    static int i_;
    // ...
};

int Conn::i_ = 0;
boost::once_flag Conn::init_ = BOOST_ONCE_INIT;

void worker() {
    boost::call_once(Conn::init, Conn::init_);
    // Do the real work...
}

Conn c;  // You probably don't want to use a global, so see the
         // next Recipe

TEST(BoostThread, Boost_call_once)
{

    boost::thread_group grp;

    for (int i = 0; i < 100; ++i)
        grp.create_thread(worker);

    grp.join_all();

    std::cout << c.i_ << '\n';// c.i_ = 1
}

int fab(int n)
{
    if (n == 0 || n == 1)
    {
        return 1;
    }
    return fab(n - 1) + fab(n - 2);
}


TEST(BoostThread, futuretest)
{
    // 声明packaged_task对象，用模板参数指明返回值类型
    // packaged_task只接受无参函数，因此需要使用bind
    boost::packaged_task<int> pt(boost::bind(fab, 10));

    // 声明unique_future对象，接受packaged_task的future值
    // 同样要用模板参数指明返回值类型
    boost::unique_future<int> uf = pt.get_future();

    // 启动线程计算，必须使用boost::move()来转移package_task对象
    // 因为packaged_task是不可拷贝的
    boost::thread(boost::move(pt));
    uf.wait(); // unique_future等待计算结果
    assert(uf.is_ready() && uf.has_value());
    std::cout << uf.get()<<std::endl; // 输出计算结果99
}

int calculate_the_answer_to_life_the_universe_and_everything()
{
    return 42;
}

void invoke_lazy_task(boost::packaged_task<int>& task)
{
    try
    {
        task();
    }
    catch (boost::task_already_started&)
    {
    }
}

//shared_future与unique_future
TEST(BoostThread, BoostFuture)
{
    {
        boost::packaged_task<int> pt(calculate_the_answer_to_life_the_universe_and_everything);
        boost::unique_future<int> fi = pt.get_future();

        boost::thread task(boost::move(pt)); // launch task on a thread

        fi.wait(); // wait for it to finish

        ASSERT_TRUE(fi.is_ready());
        ASSERT_TRUE(fi.has_value());
        ASSERT_TRUE(!fi.has_exception());
        ASSERT_TRUE(fi.get_state() == boost::future_state::ready);
        ASSERT_TRUE(fi.get() == 42);
    }

    {
        boost::promise<int> pi;
        boost::unique_future<int> fi;
        fi = pi.get_future();

        pi.set_value(42);

        ASSERT_TRUE(fi.is_ready());
        ASSERT_TRUE(fi.has_value());
        ASSERT_TRUE(!fi.has_exception());
        ASSERT_TRUE(fi.get_state() == boost::future_state::ready);
        ASSERT_TRUE(fi.get() == 42);
    }

    {
        boost::packaged_task<int> task(calculate_the_answer_to_life_the_universe_and_everything);
        task.set_wait_callback(invoke_lazy_task);
        boost::unique_future<int> f(task.get_future());

        ASSERT_TRUE(f.get() == 42);
    }
}

int main(int argc, char *argv[])
{
return crpcut::run(argc, argv);
}