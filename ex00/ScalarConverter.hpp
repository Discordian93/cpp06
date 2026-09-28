#ifndef SCALARCONVERTER_HPP
# define SCALARCONVERTER_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cerrno>
#include <cmath>

// The class only exposes a static method: it stores no state, therefore it
// must not be instantiable. All the constructors/destructor are private and
// the copy assignment operator is declared but never defined (C++98 way to
// forbid copies).
class ScalarConverter
{
	public:
		static void convert(const std::string& literal);

	private:
		ScalarConverter();
		ScalarConverter(const ScalarConverter&);
		ScalarConverter& operator=(const ScalarConverter&);
		~ScalarConverter();
};

#endif
