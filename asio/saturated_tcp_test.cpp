#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>


using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> socket_ptr; // smart pointer(no need to mannually deallocate


int main() {
    io_service service;

    try {
        tcp::endpoint ep(address::from_string("127.0.0.1"), 8888); // localhost port 8888
        tcp::socket socket(service);
        
        socket.open(tcp::v4());
        socket.connect(ep);
        std::cout << "Successfully connected to " << ep.address() << " port " << ep.port() << std::endl;

        asio::streambuf read_buffer;
        std::istream input_stream(&read_buffer);
        std::string received;
        for(int i = 0; i <= 100000; i++) {
            asio::write(socket, buffer("Hello World! " + std::to_string(i) + "\n"));
 
            // read_buffer is binded to input_stream
            asio::read_until(socket, read_buffer, "\n"); // read until getting delimiter "\n"

            // convert input stream to string
            received = std::string(std::istreambuf_iterator<char>(input_stream), {}); // c++11 or above

            std::cout << "Server replies: " << received; 
        }        

        std::cout << "Finished sent!" << std::endl;

        socket.shutdown(tcp::socket::shutdown_both);
        socket.close();
        
    }
    catch (std::exception& e)
    {
        std::cout << "[Exception] " << e.what() << std::endl;
    }
    return 0;
}
