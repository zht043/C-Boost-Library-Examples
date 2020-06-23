#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace std;
using namespace boost::asio;


int main(int argc, char* argv[]) {

    io_service service;
    char buff[1000000];

    // tcp local host test
    //ip::tcp::resolver tcp_resolver(service);
    //ip::tcp::resolver::query tcp_query("localhost","10127",ip::tcp::resolver::query::canonical_name);
    //ip::tcp::resolver::iterator tcp_iter = tcp_resolver.resolve(tcp_query);
    //ip::tcp::endpoint tcp_endpoint = *tcp_iter;
    ip::tcp::endpoint tcp_endpoint(ip::address::from_string("127.0.0.1"), 80);
    ip::tcp::socket tcp_socket(service);
    
    cout << tcp_endpoint.address().to_string() << ":" 
         << tcp_endpoint.port() << endl;

    try {
        tcp_socket.open(ip::tcp::v4());
        tcp_socket.connect(tcp_endpoint);
        tcp_socket.write_some(buffer("GET /index.html\r\n"));
        tcp_socket.read_some(buffer(buff, 1000000));
        tcp_socket.shutdown(ip::tcp::socket::shutdown_receive);
        tcp_socket.close();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        /* if throws connection failed, plz check 
            whether there is a server running listening to localhost 127.0.0.1
            Ubuntu does not have such server by default, look up apache2 for example*/
    }
    
    cout << buff << endl;
    
    

    
    service.run(); //remark: io_service::run() is a blocking method

    return 0;
}
