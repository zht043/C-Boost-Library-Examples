/*
 * Simple Publisher Subscriber (SPS) C++ implementation
 * Template programming is used here, hence most part 
 * of the source code is in the header file
 */


#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>


/* Synchronization for Reader/Writer problems */
// get exclusive access
#define sps_writer_lock(mutex) do { \
    boost::upgrade_lock<boost::shared_mutex> __writer_lock(mutex); \
    boost::upgrade_to_unique_lock<boost::shared_mutex> __unique_writer_lock( __writer_lock ); \
}while(0)
// get shared access
#define sps_reader_lock(mutex) boost::shared_lock<boost::shared_mutex>  __reader_lock(mutex); 

#define Default_Topic "DefaultTopic"

namespace SPS {

    template<class Msg>
    class MsgChannel {
        protected:
            typedef std::unordered_map<std::string, MsgChannel<Msg>*> msg_table_t;
        public:

            MsgChannel(std::string topic_name, std::string msg_name) {
                sps_writer_lock(table_mutex);
                this->key = topic_name + "." + msg_name;
                // std::cout << key << std::endl;
                
                // if key doesn't exist
                if(msg_table.find(key) == msg_table.end()) {
                    msg_table[key] = this;
                }
            }

            static MsgChannel *get_channel(std::string topic_name, std::string msg_name) {
                sps_reader_lock(table_mutex);
                std::string key = topic_name + "." + msg_name;
                
                // if key doesn't exist
                if(msg_table.find(key) == msg_table.end()) {
                    return nullptr;
                }
                return msg_table[key];
            }

            void set_msg(Msg msg) {
                sps_writer_lock(msg_mutex);
                this->message = msg;
            }

            Msg get_msg() {
                sps_reader_lock(msg_mutex);
                return this->message;
            }

        protected:
            Msg message;
            static msg_table_t msg_table;
            
            boost::shared_mutex msg_mutex;
            static boost::shared_mutex table_mutex;
            std::string key;
    };

    template <class Msg>
    class Publisher {
        public:

            Publisher(std::string topic_name, std::string msg_name) {
                channel = boost::shared_ptr<SPS::MsgChannel<Msg>>(new SPS::MsgChannel<Msg>(topic_name, msg_name));
            }

            Publisher(std::string msg_name) : Publisher(Default_Topic, msg_name) {}

            ~Publisher() {}

            void publish(Msg message) {

            }

            void async_publish(Msg message) {
                channel->set_msg(message);
            }
            

        protected:
            boost::shared_ptr<SPS::MsgChannel<Msg>> channel;
            
    };

    template <class Msg>
    class Subscriber {
        public:


            Subscriber(std::string topic_name, std::string msg_name) {
                this->topic_name = topic_name;
                this->msg_name = msg_name;
            }

            Subscriber(std::string msg_name) : Subscriber(Default_Topic, msg_name){}

            ~Subscriber() {}

            bool subscribe() {
                channel = MsgChannel<Msg>::get_channel(topic_name, msg_name);
                if(channel == nullptr) return false;
                return true;
            }

            Msg latest_msg() {
                return channel->get_msg();
            }   

        protected:
            MsgChannel<Msg> *channel;
            std::string topic_name, msg_name;
    };


}

template <class Msg>
std::unordered_map<std::string, SPS::MsgChannel<Msg>*> SPS::MsgChannel<Msg>::msg_table;

template <class Msg>
boost::shared_mutex SPS::MsgChannel<Msg>::table_mutex;