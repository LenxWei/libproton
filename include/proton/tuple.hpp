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
	const std::tuple<T...>* p;
	typedef decltype(std::get<get_index(i,sizeof...(T))>(*p)) type;
};

constexpr long fix_size(long begin, long end, long size)
{
	return fix_index(begin,size)>fix_index(end,size)  ?
				0
			:
				fix_index(end,size)-fix_index(begin,size);
}

template<typename ...T>
struct len_t<std::tuple<T...> >{
    static size_t result(const std::tuple<T...>& x)
    {
        return sizeof...(T);
    }
};

template<typename T>
class tuple_size;

template<typename ...T>
class tuple_size<std::tuple<T...> >{
public:
	static constexpr size_t value=sizeof...(T);
};

////////////////////
// build index_tuple<start, ... start+size-1>
////////////////////

template<long... _Indexes>
struct index_tuple;

template<>
struct index_tuple<>
{};

template<long start, long... _Indexes>
struct index_tuple<start, _Indexes...>
{
	typedef index_tuple<start, _Indexes..., start+1+sizeof...(_Indexes)> next;
};

// Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
template<long begin, long size>
struct build_index_tuple
{
	typedef typename build_index_tuple<begin, size - 1>::type::next type;
};

template<long start>
struct build_index_tuple<start, 1>
{
	typedef index_tuple<start> type;
};

template<long start>
struct build_index_tuple<start, 0>
{
	typedef index_tuple<> type;
};
///////////////////////

///////////////////////
// get the sub type
///////////////////////
template<long, long, typename, typename>
struct make_tuple_impl;

template<long start, long size, typename T, typename... _Tp>
struct make_tuple_impl<start, size, T, std::tuple<_Tp...> >
{
	typedef typename make_tuple_impl<start + 1, size-1,
				T, std::tuple<_Tp..., typename std::tuple_element<start, T>::type>
				>::type
		type;
};

template<long start, typename T, typename... _Tp>
struct make_tuple_impl<start, 0, T, std::tuple<_Tp...>>
{
	typedef std::tuple<_Tp...> type;
};

template<typename T, long start, long size>
struct sub_tuple_type{
	typedef typename make_tuple_impl<start, size, T, std::tuple<> >::type type;
};

//////////////////////

template<typename retT, typename tupT, typename I>
struct sub_tuple;

template<typename retT, typename tupT, long... i>
struct sub_tuple<retT, tupT, index_tuple<i...> >{
	static retT sub(const tupT& t)
	{
		return retT(std::get<i>(t)...);
	}
};
//////////////////////

} // ns detail

/** @addtogroup tuple
 * @{
 */

/** like x[index] in python
 */

template<long index, typename ...T>
typename detail::at_index<index,T...>::type
	at(const std::tuple<T...>& x)
{
	return std::get<detail::get_index(index,sizeof...(T))>(x);
}

namespace detail{
// helper function to print a tuple of any size
template<typename T, long I>
struct output_tuple {
    static void output(std::ostream& s, const T& t)
    {
        s << at<-I>(t) << ", ";
        output_tuple<T, I-1>::output(s,t);
    }
};

template<typename T>
struct output_tuple<T, 1> {
    static void output(std::ostream& s, const T& t)
    {
        s << at<-1>(t);
    }
};

template<typename T>
struct output_tuple<T, 0> {
    static void output(std::ostream& s, const T& t)
    {
    }
};

} // ns detail

/** get a slice of tuple x[begin:end] in python
 */

template<long begin, long end, typename T>
typename detail::sub_tuple_type<T, detail::fix_index(begin, detail::tuple_size<T>::value),
		detail::fix_size(begin, end, detail::tuple_size<T>::value)>::type sub(const T& t)
{
	typedef typename detail::sub_tuple_type<T, detail::fix_index(begin, detail::tuple_size<T>::value),
			detail::fix_size(begin, end, detail::tuple_size<T>::value)>::type retT;
	return detail::sub_tuple<retT, T,
			typename detail::build_index_tuple<detail::fix_index(begin, detail::tuple_size<T>::value),
					detail::fix_size(begin, end, detail::tuple_size<T>::value)>::type
			>::sub(t);
}
/*
template<long begin, long end=std::numeric_limits<long>::max(), typename ...T>
typename detail::sub_type<std::tuple<T...>, begin, end>::type
	sub(const std::tuple<T...>& x)
{
	typedef typename detail::sub_type<std::tuple<T...>, begin, end>::type ret_t;
#ifdef __clang__
	return ret_t(*reinterpret_cast<const ret_t*>(&std::get<(detail::sub_index(begin, sizeof...(T)))>(x)));
#else
	#ifdef __GNUC__
		return ret_t(*reinterpret_cast<const ret_t*>(&std::get<(detail::sub_index(end-1, sizeof...(T)))>(x)));
	#else
		static_assert(0, "unknown compiler")
	#endif
#endif
}
*/

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
