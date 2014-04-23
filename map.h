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
void map_reserve(Container& c, typename std::enable_if<has_reserve<Container>::value == true, size_t>::type s)
{
	c.reserve(s);
}

template <class Container>
void map_reserve(Container&, typename std::enable_if<has_reserve<Container>::value == false, size_t>::type)
{
}

template <class Func, class Container>
Container map(Func const& f, Container const& c)
{
	Container ret;
	map_reserve(ret, c.size());
	for (auto const& v: c) {
		ret.emplace_back(std::move(f(v)));
	}
	return std::move(ret);
}

#endif
