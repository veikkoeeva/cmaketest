#include "hello.hpp"

#include <string>

using std::string;

hello::hello() {}

hello::~hello() {}

string hello::greet(string const& greeter) const
{
	return "Hello, " + greeter + "!";
}