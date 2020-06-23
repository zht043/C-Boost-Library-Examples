#include <iostream>
#include <boost/chrono.hpp>
#include <boost/chrono/system_clocks.hpp>

using namespace boost;

double millis(void) {
    auto t = chrono::high_resolution_clock::now();
    return double(t.time_since_epoch().count()) / 1000000.00f;
}

template<class T> 
void delay(T delay_ms) {
    double t0 = millis();
    while(millis() - t0 < (double)delay_ms);
}

int main() {
    std::cout << "<============ Chrono Lib Test ============>" << std::endl; 
    
    std::cout << "System Clock: " 
              << chrono::system_clock::now()
              << std::endl;

    std::cout << "Steady Clock: " 
              << chrono::steady_clock::now()
              << std::endl;
    
    std::cout << "High Resolution Clock: " 
              << chrono::high_resolution_clock::now()
              << std::endl;

    std::cout << chrono::time_point_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()) << std::endl;
    delay(1000);
    std::cout << chrono::time_point_cast<chrono::milliseconds>(chrono::high_resolution_clock::now()) << std::endl;

    return 0;
}