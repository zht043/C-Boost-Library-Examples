#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <vector>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


class TcpServer {

private:
    typedef boost::shared_ptr<tcp::socket> socket_ptr; // smart pointer(no need to mannually deallocate
    typedef boost::shared_ptr<tcp::acceptor> acceptor_ptr;
    io_service *_service;
    vector<acceptor_ptr> acceptors;
    char read_buffer[256];
    char write_buffer[256];

protected:
public:
    TcpServer(io_service& service) {
        this->_service = &service;
    }

    TcpServer(io_service& service, tcp::endpoint& ep) {
        this->_service = &service;
        acceptor_ptr acc(new tcp::acceptor(service, ep));   
        acceptors.push_back(acc);
    }

    // add more endpoints to listen
    void add_endpoint(tcp::endpoint& ep) {
        acceptor_ptr acc(new tcp::acceptor(*_service, ep));   
        acceptors.push_back(acc);
    }

    void start() {
        socket_ptr socket;
        for(auto& acc : acceptors) {
            socket = socket_ptr(new tcp::socket(*_service));
            acc->async_accept(*socket, 
                boost::bind(&TcpServer::handle_accept, this, socket));
        }
    }

    void handle_accept(socket_ptr socket) {
        std::cout << "Accepted socket request from: [" 
                  << socket->remote_endpoint().address().to_string() << " port "
                  << socket->remote_endpoint().port() << "] " 
                  << "on port " << socket->local_endpoint().port() 
                  << std::endl;

        socket->async_read_some(buffer(read_buffer), 
                boost::bind(&TcpServer::handle_read, this, socket)); // read up to socket->receive_buffer_size
        
    }

    void handle_read(socket_ptr socket) {

        std::cout << std::string(read_buffer);

        strcpy(write_buffer, "received!\n");
        socket->async_write_some(buffer(write_buffer, strlen(write_buffer)), 
                boost::bind(&TcpServer::handle_write, this));


        memset(read_buffer, 0, strlen(read_buffer));
        socket->async_read_some(buffer(read_buffer), 
                boost::bind(&TcpServer::handle_read, this, socket));
    } 

    void handle_write(void) {}

};


// Asio asynchronus servers
int main() {
    
    io_service service;
    tcp::endpoint ep1(tcp::v4(), 8888);
    tcp::endpoint ep2(tcp::v4(), 6666);

    std::cout << ">> Server started, open another terminal and run 'nc localhost [port #]' to test this server" << std::endl; 
    std::cout << "   (Note: use 'netstat -tlnp' to check all TCP/IP port usage of this computer)" << std::endl;

    TcpServer server(service, ep1);
    server.add_endpoint(ep2); // listening to 2 endpoints at same time
    server.start(); 
    service.run();

    return 0;
}

// test this program using the following cmd
// nc localhost 8888
// then type any message u want to send to the server

// use "netstat -tlnp" to check linux TCP/IP port usages