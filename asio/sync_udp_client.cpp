#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>


using namespace std;
using namespace boost;
using namespace boost::asio;



int main() {
    io_service service;
    ip::udp::endpoint endpoint(ip::udp::v4(), 8888); 

    try {
        ip::udp::socket *socket = new ip::udp::socket(service);
        socket->open(ip::udp::v4());

        boost::array<char, 1024> receive_buffer; 
        
        size_t num_received;
        std::string to_send;
        for(int i = 0; i <= 100000; i++) {

            to_send = std::string("Hi server! [iter ") + to_string(i) + "]\n";
            socket->send_to(asio::buffer(to_send), endpoint);

            num_received = socket->receive_from(asio::buffer(receive_buffer), endpoint);
            std::string reply_packet = std::string(receive_buffer.begin(), receive_buffer.begin() + num_received);
            std::cout << reply_packet;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "[Exception] " << e.what() << std::endl;
    }
    return 0;
}
