#ifndef HEXOUT_H_
#define HEXOUT_H_

#include <type_traits>
#include <string>
#include <sstream>
#include <ios>
#include <iomanip>
#include <climits>

namespace detail {
	constexpr int HEX_DIGIT_BITS = 4;

	template<typename T> struct is_char : std::integral_constant<bool, std::is_same<T, char>::value ||
					      				   std::is_same<T, signed char>::value ||
									   std::is_same<T, unsigned char>::value> {};
}

template<typename T>
std::string hex_out_s(T val) {
	using namespace detail;

	std::stringstream sformatter;
	sformatter << std::hex << std::internal << "0x" << std::setfill('0') << std::setw(sizeof(T) * CHAR_BIT / HEX_DIGIT_BITS) << (is_char<T>::value ? static_cast<int>(val) : val);

	return sformatter.str();
}

#endif /* HEXOUT_H_ */

