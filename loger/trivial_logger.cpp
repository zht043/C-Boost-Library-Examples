#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>


#include <iostream>
#include <boost/chrono.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/thread.hpp> 


unsigned int millis(void) {
    auto t = boost::chrono::high_resolution_clock::now();
    return (unsigned int)(double(t.time_since_epoch().count()) / 1000000.00f);
}

unsigned int micros(void) {
    auto t = boost::chrono::high_resolution_clock::now();
    return (unsigned int)(double(t.time_since_epoch().count()) / 1000.00f);
}


int main(int, char*[])
{

    /* boost logger is thread safe! */

    // set filters
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::info
    );

    double cout_latency, log_latency;
    double t0;
    
    std::cout << "<<<<< Logging started >>>>>" << std::endl;

    t0 = micros();
    std::cout << "A trace severity message" << std::endl;
    cout_latency = micros() - t0;

    // increasing severity order from top to bottom
    t0 = micros();
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    log_latency = micros() - t0;

    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
    std::cout << "<<<<< Logging finished >>>>>" << std::endl;
    BOOST_LOG_TRIVIAL(info) << "cout_latency: " << cout_latency; 
    BOOST_LOG_TRIVIAL(info) << "log_latency: " << log_latency; 
    return 0;
}