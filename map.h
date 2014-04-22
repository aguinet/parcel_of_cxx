#ifndef MAP_H
#define MAP_H

template<typename T>
struct has_reserve
{
    template<typename U, size_t (U::*)() const> struct SFINAE {}; 
    template<typename U> static char Test(SFINAE<U, &U::reserve>*);
    template<typename U> static int Test(...);
    static constexpr bool value = sizeof(Test<T>(0)) == sizeof(char);
};


template <class Container>
inline typename std::enable_if<has_reserve<Container>::value == true, void>::type map_reserve(Container& c, size_t const n)
{
	c.reserve(n);
}

template <class Container>
inline typename std::enable_if<has_reserve<Container>::value == false, void>::type map_reserve(Container&, size_t const)
{
}

template <class Func, class Container>
Container map(Func const& f, Container const& c)
{
	Container ret;
	map_reserve(ret, c.size());
	for (auto const& v: c) {
		ret.push_back(f(v));
	}
	return ret;
}

#endif
