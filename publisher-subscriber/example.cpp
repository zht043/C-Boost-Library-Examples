#include <iostream>
#include "simple_pubsub.hpp"
#include <boost/chrono.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/thread.hpp> 

using namespace SPS;
using namespace std;


void delay(unsigned int milliseconds) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(milliseconds));
}

int main(int argc, char *argv[]) {

    boost::thread pub_thread( []() {
        
        Publisher<std::string> pub1("Topic1", "Msg1");
        Publisher<double> pub2("Topic1", "Msg2");
        Publisher<int> pub3("Topic2", "ID");
        
        
        while(1) {
            pub1.async_publish("Hello, I'm pub1");
            pub2.async_publish(888.888);
            pub3.async_publish(5);
            
        }
    });

    boost::thread sub_thread( []() {
        
        Subscriber<std::string> sub1A("Topic1", "Msg1");
        Subscriber<std::string> sub1B("Topic1", "Msg1");
        Subscriber<std::string> sub1C("Topic1", "Msg1");
        

        Subscriber<double> sub2A("Topic1", "Msg2");
        Subscriber<double> sub2B("Topic1", "Msg2");


        Subscriber<int> sub3("Topic2", "ID");

        while(!sub1A.subscribe());
        while(!sub1B.subscribe());
        while(!sub1C.subscribe());
        while(!sub2A.subscribe());
        while(!sub2B.subscribe());
        while(!sub3.subscribe());
        
        
        while(1) {
            std::cout << "==================================" << std::endl;

            std::cout << "sub1A: " << sub1A.latest_msg() << std::endl;
            std::cout << "sub1B: " << sub1B.latest_msg() << std::endl;
            std::cout << "sub1C: " << sub1C.latest_msg() << std::endl;

            std::cout << "sub2A: " << sub2A.latest_msg() << std::endl;
            std::cout << "sub2B: " << sub2B.latest_msg() << std::endl;

            std::cout << "sub3: " << sub3.latest_msg() << std::endl;

            std::cout << "==================================" << std::endl;
        }
        
    });

    pub_thread.join();
    sub_thread.join();

    return 0;
}