#ifndef HELLO_HPP
#define HELLO_HPP

#include <string>

class hello
{
	public:
		hello();
		~hello();
		std::string greet(std::string const& greeter) const;
};

#endif HELLO_HPP
