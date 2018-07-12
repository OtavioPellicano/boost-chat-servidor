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

    boost::thread t4(std::bind(&ServidorBoost::logQueueLoop, srv));


    boost::thread_group threads;
    threads.add_thread(&t1);
    threads.add_thread(&t2);
    threads.add_thread(&t3);
    threads.add_thread(&t4);

    threads.join_all();

    return 0;
}
