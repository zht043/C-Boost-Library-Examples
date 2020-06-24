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
    class socket_service{
    public:
        tcp::socket *socket;
        std::string *write_buffer;
        asio::streambuf *read_buffer;
        std::istream *read_stream;
        tcp::acceptor *acceptor;
        socket_service(tcp::socket *sock, tcp::acceptor *acc) {
            this->socket = sock;
            this->acceptor = acc;
            this->read_buffer = new asio::streambuf();
            this->read_stream = new std::istream(this->read_buffer);
            this->write_buffer = NULL;
        }
        void set_write_buffer(std::string *buf) {
            this->write_buffer = buf;
        }
        ~socket_service() {
            delete socket;
            delete write_buffer;
            delete read_buffer;
            delete read_stream;
            delete acceptor;
        }

    };

    io_service *_service;
    vector<socket_service*> services;
    

protected:
public:
    TcpServer(io_service& service) {
        this->_service = &service;
    }

    ~TcpServer() {
        for(auto& service : services) {
            delete service;
        }
    }

    void start(tcp::endpoint const &endpoint_to_listen) {
        socket_service *sock_service = new socket_service(new tcp::socket(*_service), 
                                               new tcp::acceptor(*_service, endpoint_to_listen));
        services.push_back(sock_service);

        sock_service->acceptor->async_accept(*(sock_service->socket), 
            boost::bind(&TcpServer::handle_accept, this, sock_service));
    }


    void handle_accept(socket_service* sock_service) {
        std::cout << "Accepted socket request from: [" 
                  << sock_service->socket->remote_endpoint().address().to_string() << " port "
                  << sock_service->socket->remote_endpoint().port() << "] " 
                  << "on port " << sock_service->socket->local_endpoint().port() 
                  << std::endl;

        // read one line from client    
        asio::async_read_until(*(sock_service->socket), *(sock_service->read_buffer), "\n", 
                boost::bind(&TcpServer::handle_read, this, sock_service)); 
      

        // invoke another accept on this endpoint as a new service
        socket_service *new_sock_service = new socket_service(new tcp::socket(*_service), 
                                               sock_service->acceptor); // same acceptor but using a new socket to accept new session
        services.push_back(new_sock_service);
        new_sock_service->acceptor->async_accept(*(new_sock_service->socket), 
            boost::bind(&TcpServer::handle_accept, this, new_sock_service));
    }

    void handle_read(socket_service* sock_service) {

        // convert input stream to string
        std::string received = std::string(std::istreambuf_iterator<char>(*(sock_service->read_stream)), {});
        std::cout << received;

        sock_service->set_write_buffer(new std::string("received, thank you!\n"));
        sock_service->socket->async_write_some(buffer(*(sock_service->write_buffer)), 
                boost::bind(&TcpServer::handle_write, this, sock_service));
    } 

    void handle_write(socket_service* sock_service) {
        // read one line from client    
        asio::async_read_until(*(sock_service->socket), *(sock_service->read_buffer), "\n", 
                boost::bind(&TcpServer::handle_read, this, sock_service)); 
    }

};


// Asio asynchronus servers
int main() {
    
    io_service service;
    tcp::endpoint ep1(tcp::v4(), 6666);
    tcp::endpoint ep2(tcp::v4(), 7777);
    tcp::endpoint ep3(tcp::v4(), 8888);


    std::cout << ">> Server started, open another terminal and run 'nc localhost [port #]' to test this server" << std::endl; 
    std::cout << "   (Note: use 'netstat -tlnp' to check all TCP/IP port usage of this computer)" << std::endl;

    try {
        TcpServer server(service);
        server.start(ep1);
        server.start(ep2); 
        server.start(ep3); 
        service.run();
    } 
    catch (std::exception& e)
    {
        std::cout << "[Exception] " << e.what() << std::endl;
    }
    return 0;
}

// test this program using the following cmd
// nc localhost 8888
// then type any message u want to send to the server

// use "netstat -tlnp" to check linux TCP/IP port usages