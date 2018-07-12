#include <iostream>
#include <boost/thread.hpp>
#include <servidorboost.h>
#include <functional>

using namespace std;

int main()
{
    ServidorBoost* srv = new ServidorBoost();

    boost::thread t1(std::bind(&ServidorBoost::incommingConnectionLoop, srv));

    boost::thread t2(std::bind(&ServidorBoost::readyReadLoop, srv));

    boost::thread t3(std::bind(&ServidorBoost::messageQueueLoop, srv));


    boost::thread_group threads;
    threads.add_thread(&t1);
    threads.add_thread(&t2);
    threads.add_thread(&t3);

    threads.join_all();

//    threads.create_thread(boost::bind(ServidorBoost::incommingConnectionLoop(), &srv));


    return 0;
}
