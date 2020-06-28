#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2.hpp>


using namespace std;
using namespace boost;
using namespace boost::asio;



class My_Observer {
public:
    void update(int data) {
        cout << data << endl;
    }
};

class My_Subject {
    My_Observer *obs;
    signals2::signal<void (int)> sig;
public:
    void add_observer(My_Observer& obs) {
        this->obs = &obs; // for multiple observers use a vector
    }
    auto& signaller() {
        return sig;
    } 

    void notify_observer() {
        this->obs->update(23333);
    }
    void signal_observer() {
        this->sig(66666);
    }
};



void step1() {
    cout << "step1" << endl;
}

void step2() {
    cout << "step2" << endl;
}

void step2_1() {
    cout << "step2_1" << endl;
}

string slot_with_return(int n) {
    cout << "Slot_with_return: " << n << endl;
    return "HaHaHa";
}

int main() {

    My_Subject sub;
    My_Observer obs;

    // oop observer pattern
    sub.add_observer(obs);
    sub.notify_observer();
    

    // signal & slot mechanism 
    sub.signaller().connect(boost::bind(&My_Observer::update, &obs, _1)); // bind(&::update, &obs) means passing the pointer to the (non static) member function of obs object
    sub.signal_observer();

    // signal with order
    signals2::signal<void ()> sig;
    sig.connect(1, &step2_1); // group 1
    sig.connect(1, &step2); //group 1
    sig.connect(0, &step1); //group 0
    sig(); // signals all slots

    // signal with return
    signals2::signal<string (int)> sig2;
    sig2.connect(&slot_with_return);
    cout << *sig2(88888) << endl;
    return 0;
}