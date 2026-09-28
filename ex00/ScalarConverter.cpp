#include "ScalarConverter.hpp"

// ---------------------------------------------------------------------------
// detectType: figures out which scalar type the input string represents.
// Rules (from the subject):
//   'c'                                        -> char   (3 chars, quoted)
//   -inff / +inff / nanf                       -> float pseudo-literal
//   -inf  / +inf  / nan                        -> double pseudo-literal
//   digits with a trailing 'f'                 -> float
//   digits containing a '.'                    -> double
//   digits only (with optional sign)           -> int
// Anything else is UNKNOWN and rejected.
// ---------------------------------------------------------------------------
enum LiteralType { CHAR, INT, FLOAT, DOUBLE, UNKNOWN };

static LiteralType	detectType(const std::string& s)
{
if (s.size() == 3 && s[0] == '\'' && s[2] == '\'')
return (CHAR);

if (s == "-inff" || s == "+inff" || s == "nanf")
return (FLOAT);
if (s == "-inf" || s == "+inf" || s == "nan")
return (DOUBLE);

std::string::size_type	end = s.size();
bool	hasFloatSuffix = false;

// Optional trailing 'f' marks a float literal
if (end > 0 && s[end - 1] == 'f')
{
hasFloatSuffix = true;
end -= 1;
}

std::string	body = s.substr(0, end);
if (body.empty())
return (UNKNOWN);
if (body[0] == '-' || body[0] == '+')
body.erase(0, 1);
if (body.empty())
return (UNKNOWN);

// The rest must be digits, optionally with exactly one dot
unsigned int	dots = 0;
for (std::string::size_type i = 0; i < body.size(); ++i)
{
if (body[i] == '.')
dots++;
else if (!std::isdigit(static_cast<unsigned char>(body[i])))
return (UNKNOWN);
}
if (dots > 1)
return (UNKNOWN);

if (hasFloatSuffix)
return (FLOAT);
if (dots == 1)
return (DOUBLE);
return (INT);
}

// ---------------------------------------------------------------------------
// Formatting for float/double output.
// C++ streams would print the float 42.0f as "42" (no decimal point), but the
// subject expects "42.0f". Rule used here:
//   - if the value is an exact integer, force fixed notation with one decimal
//     digit ("42.0"), so floats/doubles always show they are not ints;
//   - otherwise print with enough significant digits to round-trip the type
//     (digits10 + 1), which reveals the real precision of float vs double.
// The trailing 'f' for floats is added manually: it is notation, not value.
// ---------------------------------------------------------------------------
static void	printFloat(float value)
{
if (value == std::floor(value))
std::cout << std::fixed << std::setprecision(1);
else
std::cout << std::setprecision(std::numeric_limits<float>::digits10 + 1);
std::cout << value << "f" << std::endl;
std::cout.unsetf(std::ios::floatfield);
}

static void	printDouble(double value)
{
if (value == std::floor(value))
std::cout << std::fixed << std::setprecision(1);
else
std::cout << std::setprecision(std::numeric_limits<double>::digits10 + 1);
std::cout << value << std::endl;
std::cout.unsetf(std::ios::floatfield);
}

// ---------------------------------------------------------------------------
// convert:
//   1. detect the natural type of the literal,
//   2. parse the string into that type (authorized string->number functions),
//   3. explicitly cast (static_cast) to the other three types and display.
// If a conversion overflows or makes no sense (nan/inf to char/int), we print
// "impossible" instead of triggering undefined behavior.
// ---------------------------------------------------------------------------
void	ScalarConverter::convert(const std::string& literal)
{
LiteralType	type = detectType(literal);

if (type == UNKNOWN)
{
std::cout << "Error: '" << literal
  << "' is not a valid C++ scalar literal" << std::endl;
return ;
}

// Step 1: string -> natural type. We keep the parsed value in a double,
// the widest floating type: strtod natively understands "nan", "+inf"
// and "-inf"; strtol gives exact integer parsing with overflow report.
double	value = 0.0;

switch (type)
{
case CHAR:
value = static_cast<double>(literal[1]);
break ;
case INT:
errno = 0;
value = static_cast<double>(std::strtol(literal.c_str(), NULL, 10));
if (errno == ERANGE)
{
std::cout << "Error: integer overflow in '" << literal << "'" << std::endl;
return ;
}
break ;
case FLOAT:
case DOUBLE:
value = std::strtod(literal.c_str(), NULL);
break ;
default:
return ;
}

// nan / +-inf have no integer or character representation at all.
const bool	special = std::isnan(value) || std::isinf(value);

// --- char ---------------------------------------------------------------
std::cout << "char: ";
if (special)
{
std::cout << "impossible" << std::endl;
}
else
{
// Only cast if the value fits in a signed char [-128, 127];
// outside that range the conversion is undefined -> impossible.
if (std::floor(value) < -128.0 || std::ceil(value) > 127.0)
{
std::cout << "impossible" << std::endl;
}
else
{
const char c = static_cast<char>(value);
if (std::isprint(static_cast<unsigned char>(c)))
std::cout << "'" << c << "'" << std::endl;
else
std::cout << "Non displayable" << std::endl;
}
}

// --- int ----------------------------------------------------------------
std::cout << "int: ";
if (special)
{
std::cout << "impossible" << std::endl;
}
else if (value < static_cast<double>(std::numeric_limits<int>::min())
|| value > static_cast<double>(std::numeric_limits<int>::max()))
{
// Overflow check against the exact int bounds before casting.
std::cout << "impossible" << std::endl;
}
else
{
std::cout << static_cast<int>(value) << std::endl;
}

// --- float & double -------------------------------------------------------
// Special values are printed raw (streams render them as nan/inf/-inf);
// the others go through the fixed-precision helpers above.
std::cout << "float: ";
if (special)
std::cout << value << "f" << std::endl;
else
printFloat(static_cast<float>(value));

std::cout << "double: ";
if (special)
std::cout << value << std::endl;
else
printDouble(value);
}
