#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


void handle_write(const boost::system::error_code& error) {
    if(error) {
        std::cerr << "[Error Code] " << error.message() << std::endl;
        while(1); 
    }
}

void handle_read(ip::udp::socket *socket, ip::udp::endpoint &ep,
                 boost::array<char, 1024>& receive_buffer, std::size_t num_received,
                 const boost::system::error_code& error ) {
    if(error) {
        std::cerr << "[Error Code] " << error.message() << std::endl;
        while(1); 
    }

    std::string packet_received = std::string(receive_buffer.begin(), receive_buffer.begin() + num_received);
            std::cout << "Packet received: " << packet_received;

    socket->async_send_to(asio::buffer("Packet received, thank you!\n"), ep, 
                    boost::bind(&handle_write, asio::placeholders::error));

    // next round of receive
    socket->async_receive_from(asio::buffer(receive_buffer), ep, 
                    boost::bind(&handle_read, socket, boost::ref(ep), boost::ref(receive_buffer),  
                    asio::placeholders::bytes_transferred, asio::placeholders::error));

}


int main() {
    io_service service;
    ip::udp::socket *socket;

    try {
        ip::udp::endpoint ep(ip::udp::v4(), 8888); // listen on port 8888 (server listens to client)
    
        socket = new ip::udp::socket(service, ep);

        std::cout << ">> Server started" << std::endl; 
        std::cout << "use 'nc -u localhost 8888' to send typed packet in stdin to this server" << std::endl;
        std::cout << "   (Note: use 'netstat -tlnp' to check all TCP/IP port usage of this computer)" << std::endl;

        boost::array<char, 1024> receive_buffer; 

        socket->async_receive_from(asio::buffer(receive_buffer), ep, 
                        boost::bind(&handle_read, socket, boost::ref(ep), boost::ref(receive_buffer),  
                        asio::placeholders::bytes_transferred, asio::placeholders::error));

        service.run();
    }
    catch (std::exception& e)
    {
        std::cout << "[Exception] " << e.what() << std::endl;

        delete socket;
    }


    
    return 0;
}
