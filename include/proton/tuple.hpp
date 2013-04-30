#ifndef PROTON_TUPLE_HEADER
#define PROTON_TUPLE_HEADER


/** @file tuple.hpp
 *  @brief tuple support.
 *  Please include this header instead of \<tuple\>.
 */

#include <tuple>
#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <limits>

#include <proton/base.hpp>

namespace proton{

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

constexpr long fix_index(long i, long size)
{
    return (i>size)?
				size
			:(
				(i<0)?
					(i+size < 0 ?
						0
					:
						i+size
					)
				:
					i
			);
}

constexpr long sub_index(long i, long size)
{
    return (i>=size)?
				size-1
			:(
				(i<0)?
					(i+size < 0 ?
						0
					:
						i+size
					)
				:
					i
			);
}

constexpr long get_index(long i, long size)
{
    return (i>=size)?
				-1
			:(
				(i<0)?
					(i+size < 0 ?
						-1
					:
						i+size
					)
				:
					i
			);
}

template<long i, typename ...T>
struct at_index{
	static_assert(i>=0, "out of range");
	const std::tuple<T...>* p;
	typedef decltype(std::get<i>(*p)) type;
};

constexpr long fix_size(long begin, long end, long size)
{
	return fix_index(begin,size)>fix_index(end,size)?
				0
			:
				fix_index(end,size)-fix_index(begin,size);
}

template<typename T, size_t begin, size_t size>
struct sub{
private:
	static_assert(begin < std::tuple_size<T>::value, "out of range");

	std::tuple<typename std::tuple_element<begin, T>::type> *p;
	typedef typename sub<T, begin+1,
						(begin+size > std::tuple_size<T>::value ?
							(std::tuple_size<T>::value-begin-1)
							: (size-1))
						>::type next_types;
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


template<typename ...T>
struct len_t<std::tuple<T...> >{
    static size_t result(const std::tuple<T...>& x)
    {
        return sizeof...(T);
    }
};

} // ns detail

/** @addtogroup tuple
 * @{
 */

/** like x[index] in python
 */

template<long index, typename ...T>
typename detail::at_index<detail::get_index(index,sizeof...(T)),T...>::type
	at(const std::tuple<T...>& x)
{
	return std::get<detail::get_index(index,sizeof...(T))>(x);
}

/** get a slice of tuple x[begin:end] in python
 */
template<long begin, long end=std::numeric_limits<long>::max(), typename ...T>
typename detail::sub<std::tuple<T...>, detail::fix_index(begin, sizeof...(T)),
									   detail::fix_size(begin,end, sizeof...(T))>::type
	sub(const std::tuple<T...>& x)
{
	typedef typename detail::sub<std::tuple<T...>, detail::fix_index(begin, sizeof...(T)),
									   detail::fix_size(begin,end, sizeof...(T))>::type ret_t;
#ifdef __llvm__
	return ret_t(*reinterpret_cast<const ret_t*>(&std::get<(detail::sub_index(begin, sizeof...(T)))>(x)));
#else
	#ifdef __GNUC__
		return ret_t(*reinterpret_cast<const ret_t*>(&std::get<(detail::sub_index(end-1, sizeof...(T)))>(x)));
	#else
		static_assert(0, "unknown compiler")
	#endif
#endif
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
auto _t(T&& ...x) -> decltype(std::make_tuple(x...))
{
	return std::make_tuple(x...);
}

/** eq to forward_as_tuple
 */
template<typename ...T>
auto _f(T&& ...x) -> decltype(std::forward_as_tuple(x...))
{
	return std::forward_as_tuple(x...);
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
 * @example tuple.cpp
 * @}
 */
}

#endif // PROTON_TUPLE_HEADER
