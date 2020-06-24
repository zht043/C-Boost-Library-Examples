#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>


using namespace std;
using namespace boost;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr; // smart pointer(no need to mannually deallocate

boost::mutex mu;

void client_session(socket_ptr socket) {
    asio::streambuf read_buffer;
    std::istream input_stream(&read_buffer);
    std::string received;
    try {
        while(1) {
           

            // read_buffer is binded to input_stream
            asio::read_until(*socket, read_buffer, "\n"); // read until getting delimiter "\n"
            
            mu.lock();
            // convert input stream to string
            received = std::string(std::istreambuf_iterator<char>(input_stream), {});
            std::cout << received;
            mu.unlock();

            // socket->write_some(boost::asio::buffer("received")); // oop style
            /* boost::asio:write/read vs socket.write_some/read_some : 
            * write can write all in one call, write_some doesn't guarenteed 
            * all bytes got written, which might need a loop to do so
            */
            boost::asio::write(*socket, boost::asio::buffer("received!\n") );
            
        }
    }
    catch (std::exception& e)
    {
        mu.lock();
        if(e.what() == std::string("read_until: End of file")) {
            std::cout << "Peer client is closed" << std::endl;
        } 
        else 
        std::cout << "[Exception] " << e.what() << std::endl;
        mu.unlock();
    }
    mu.lock();
    std::cout << "End of Thread!" << std::endl;
    mu.unlock();
}

int main() {
    io_service service;

    try {
        ip::tcp::endpoint endpoint_to_listen(ip::tcp::v4(), 8888); // listen on port 8888
        
        /* boost acceptor constructor automatically take cares of all three 
         * steps: "bind() ---> listen() ---> accept()" of a unix socket  
         */
        ip::tcp::acceptor acceptor(service, endpoint_to_listen); 
            

        std::cout << ">> Server started, open another terminal and run 'nc localhost 8888' to test this server" << std::endl; 
        std::cout << "   (Note: use 'netstat -tlnp' to check all TCP/IP port usage of this computer)" << std::endl;

        while(1) {
            socket_ptr socket(new ip::tcp::socket(service)); // new socket object and let the smart pointer to point to it
            acceptor.accept(*socket); // waiting for new connection (blocking)
            mu.lock();
            std::cout << "Accepted socket request from: " << socket->remote_endpoint().address().to_string() 
                                << std::endl;
            mu.unlock();
            boost::thread(boost::bind(client_session, socket)); // process client request in a new thread
        }
    }
    catch (std::exception& e)
    {
        mu.lock();
        std::cout << "[Exception] " << e.what() << std::endl;
        mu.unlock();
    }
    return 0;
}

// test this program using the following cmd
// nc localhost 8888
// then type any message u want to send to the server

// use "netstat -tlnp" to check linux TCP/IP port usages