#include "ThreadQueue.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
using namespace std;

ThreadQueue<string> messageQueue;
mutex print_mtx;

void provider( int product_num, int wait_seconds ) 
{
    for( int i = 0; i < product_num; ++i ) 
    {
        string message( "Message-" );
        message.append( to_string( i ) );
        messageQueue.push( message );
        this_thread::sleep_for( chrono::seconds( wait_seconds ) );
    }
    lock_guard<mutex> guard( print_mtx );
    cout << "All works done!" << endl;
}

void consumer( int consumer_id ) 
{
    while( true ) 
    {
        string message;
        messageQueue.pop( message );
        {
            lock_guard<mutex> guard( print_mtx );
            cout << "consumer-" << consumer_id << " receive: " << message << endl; 
        }
    }
}

int main()
{
    thread pvi{ provider, 10, 1 };
    thread csm1{ consumer, 1 };
    thread csm2{ consumer, 2 };
    pvi.join();
    csm1.join();
    csm2.join();
    
    return 0;
}
