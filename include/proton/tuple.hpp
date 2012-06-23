#ifndef PROTON_TUPLE_HEADER
#define PROTON_TUPLE_HEADER


/** @file vector.hpp
 *  @brief vector support.
 *  Please include this header instead of \<vector\>.
 */

#include <tuple>
#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>

#include <proton/base.hpp>

namespace proton{

/** @addtogroup tuple_
 * @{
 */

namespace detail{
// helper function to print a tuple of any size
template<typename T, std::size_t I>
struct output_tuple {
    static void output(std::ostream& s, T&& t)
    {
        output_tuple<T, I-1>::output(s,t);
        s << ", " << std::get<I-1>(t);
    }
};

template<typename T>
struct output_tuple<T, 1> {
    static void output(std::ostream& s, T&& t)
    {
        s << std::get<0>(t);
    }
};

template<typename T>
struct output_tuple<T, 0> {
    static void output(std::ostream& s, T&& t)
    {
    }
};

template<typename T, size_t begin, size_t size>
struct sub{
private:
	static_assert(begin < std::tuple_size<T>::value, "out of range");
	static_assert(begin+size <= std::tuple_size<T>::value, "out of range");

	std::tuple<typename std::tuple_element<begin, T>::type> *p;
	typedef typename sub<T, begin+1, size-1>::type next_types;
	next_types* q;

public:
	typedef decltype(std::tuple_cat(*p,*q)) type;
};

template<typename T, size_t begin>
struct sub<T, begin, 0>{
	typedef std::tuple<> type;
};

template<typename T, size_t begin>
struct sub<T,begin,1>{
	static_assert(begin < std::tuple_size<T>::value, "out of range");

	typedef std::tuple<typename std::tuple_element<begin,T>::type > type;
};

} // ns detail

/** get a slice of tuple x[begin:end] in python.
 * @param begin must be constexpr determined during compile time
 * @param end  must be constexpr determined during compile time
 */
template<size_t begin, size_t end, typename ...T>
typename detail::sub<std::tuple<T...>, begin, end-begin>::type
	sub(const std::tuple<T...>& x)
{
	typedef typename detail::sub<std::tuple<T...>, begin, end-begin>::type ret_t;
	return ret_t(*(ret_t*)(&std::get<end-1>(x))); // [FIXME] in g++, the items in a tuple is in reverse order. for other implementation, need fix.
}

/** get a slice of tuple x[begin:] in python.
 * @param begin must be constexpr determined during compile time
 */
template<size_t begin, typename ...T>
typename detail::sub<std::tuple<T...>, begin, sizeof...(T)-begin>::type
	sub_to_end(const std::tuple<T...>& x)
{
	typedef typename detail::sub<std::tuple<T...>, begin, sizeof...(T)-begin>::type ret_t;
	return ret_t(*(ret_t*)(&std::get<sizeof...(T)-1>(x))); // [FIXME] in g++, the items in a tuple is in reverse order. for other implementation, need fix.
}

/** general output for tuple.
 * @param s the output stream
 * @param x the tuple to be outputed
 * @return s
 */
template <typename ...T>
std::ostream& operator<<(std::ostream& s, const std::tuple<T...>& x)
{
    s << "(";
    detail::output_tuple<decltype(x), sizeof...(T)>::output(s,x);
    s << ")";
    return s;
}

template <typename ...T>
std::wostream& operator<<(std::wostream& s, const std::tuple<T...>& x)
{
    s << L"(";
    detail::output_tuple<decltype(x), sizeof...(T)>::output(s,x);
    s << L")";
    return s;
}

/** tuple + tuple
 */
template<typename T2, typename ...T1>
auto operator+(const std::tuple<T1...>& x, T2&& y) -> decltype(std::tuple_cat(x,y))
{
	return std::tuple_cat(x,y);
}

template<typename T2, typename ...T1>
auto operator+(std::tuple<T1...>&& x, T2&& y) -> decltype(std::tuple_cat(x,y))
{
	return std::tuple_cat(x,y);
}

/** eq to make_tuple
 */
template<typename ...T>
auto _t(T&& ...x) -> decltype(make_tuple(x...))
{
	return make_tuple(x...);
}

#if 0
/* vector_ * n
 */
template<typename T, typename A>
vector_<T,A> operator*(const std::vector<T,A>& s, size_t n)
{
    vector_<T,A> r;
    r.reserve(s.size()*n);
    for(size_t i=0; i<n; i++)
        r.extend(s);
    return r;
}

/* n * vector_
 */
template<typename T, typename A>
vector_<T,A> operator*(size_t n, const std::vector<T,A>& s)
{
    return s*n;
}
#endif

/**
 * @}
 */
}

#endif // PROTON_TUPLE_HEADER
