#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>


using namespace std;
using namespace boost;
using namespace boost::asio;

typedef boost::shared_ptr<ip::udp::socket> socket_ptr; // smart pointer(no need to mannually deallocate


int main() {
    io_service service;

    try {
        ip::udp::endpoint ep_listen(ip::udp::v4(), 8888); // listen on port 8888 (server listens to client)
        ip::udp::endpoint ep_talk(ip::udp::v4(), 6666); // talk on port 6666 (server talks to client)
        
        // listen setup (typical server setup)
        socket_ptr socket1(new ip::udp::socket(service, ep_listen));

        // talk setup (typical client setup, here just testing the server can both be a server and client(virtual) at the same time)
        socket_ptr socket2(new ip::udp::socket(service));
        socket2->open(ip::udp::v4());

        std::cout << ">> Server started" << std::endl; 
        std::cout << "use 'nc -lu localhost 6666' to receive the talk from this server on stdout" << std::endl;
        std::cout << "use 'nc -u localhost 8888' to send typed packet in stdin to this server" << std::endl;
        std::cout << "   (Note: use 'netstat -tlnp' to check all TCP/IP port usage of this computer)" << std::endl;

        boost::array<char, 1024> receive_buffer; 

        boost::thread( [&ep_talk, &socket2] () -> void {
            std::string to_send;
            while(1) {
                to_send = std::string(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
                socket2->send_to(asio::buffer(to_send), ep_talk); 
                for(int i = 0; i <= 20; i++) {
                    to_send = std::string("Server says hi [iter ") + to_string(i) + "]\n";
                    socket2->send_to(asio::buffer(to_send), ep_talk);    
                }

                to_send = std::string("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
                socket2->send_to(asio::buffer(to_send), ep_talk); 
            }
        });

        std::string to_send;
        size_t num_received;
        while(1) {
            num_received = socket1->receive_from(asio::buffer(receive_buffer), ep_listen);
            std::string packet_received = std::string(receive_buffer.begin(), receive_buffer.begin() + num_received);
            std::cout << "Packet received: " << packet_received;
            
            to_send = std::string("Packet received, thank you!\n");
            socket1->send_to(asio::buffer(to_send), ep_listen); 
        }
    }
    catch (std::exception& e)
    {
        std::cout << "[Exception] " << e.what() << std::endl;
    }
    return 0;
}
