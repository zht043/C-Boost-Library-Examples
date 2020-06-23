#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace std;
using namespace boost::asio;



int main(int argc, char* argv[]) {

    if(argc - 1 != 1) {
        cout << "Usage: " << argv[0] << " <URL>" << endl;
        return 0;
    }
    string url = string(argv[1]); 

    io_service service;

    // resolve the ip address of an URL
    ip::tcp::resolver tcp_resolver(service);
    ip::tcp::resolver::query tcp_query(url, "80");
    ip::tcp::resolver::iterator tcp_iter;
    ip::tcp::endpoint tcp_endpoint;
    tcp_iter = tcp_resolver.resolve(tcp_query);
    tcp_endpoint = *tcp_iter;
    cout << tcp_endpoint.address().to_string() << ":" 
         << tcp_endpoint.port() << endl;

    // resolved udp ip addr should be exactly the same as tcp above
    ip::udp::resolver udp_resolver(service);
    ip::udp::resolver::query udp_query(url, "80");
    ip::udp::resolver::iterator udp_iter;
    ip::udp::endpoint udp_endpoint;
    udp_iter = udp_resolver.resolve(udp_query);
    udp_endpoint = *udp_iter;
    cout << udp_endpoint.address().to_string() << ":" 
         << udp_endpoint.port() << endl;

    

    
    service.run(); //remark: io_service::run() is a blocking method

    return 0;
}
