#ifndef DOUBLE_INT_HPP
#define DOUBLE_INT_HPP
#include <inttypes.h>
template <class T> struct double_int;
template <class T> using double_int_t = typename double_int<T>::type;
template<> struct double_int< uint8_t> { using type = uint16_t; };
template<> struct double_int<  int8_t> { using type =  int16_t; };
template<> struct double_int<uint16_t> { using type = uint32_t; };
template<> struct double_int< int16_t> { using type =  int32_t; };
template<> struct double_int<uint32_t> { using type = uint64_t; };
template<> struct double_int< int32_t> { using type =  int64_t; };
#if __SIZEOF_INT128__
template<> struct double_int<uint64_t> { using type = unsigned __int128; };
template<> struct double_int< int64_t> { using type =   signed __int128; };
#endif
#endif
