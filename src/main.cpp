#include "hello.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <iostream>

using std::cout;
using std::endl;

int main()
{
	hello helloer;
	cout << helloer.greet("World here") << endl;

	return EXIT_SUCCESS;
}