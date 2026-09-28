#include "ScalarConverter.hpp"

// Test program: ./convert <literal>
int	main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: ./convert <scalar literal>" << std::endl;
		return (1);
	}
	ScalarConverter::convert(argv[1]);
	return (0);
}
