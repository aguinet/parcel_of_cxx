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

template <typename Container>
struct has_random_access: public std::is_same<typename std::iterator_traits<typename Container::const_iterator>::iterator_category, std::random_access_iterator_tag>
{ };

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
typename std::enable_if<has_random_access<Container>::value == false, Container>::type map(Func const& f, Container const& c)
{
	Container ret;
	map_reserve(ret, c.size());
	for (auto const& v: c) {
		ret.emplace_back(std::move(f(v)));
	}
	return std::move(ret);
}

template <class Func, class Container>
typename std::enable_if<has_random_access<Container>::value == true, Container>::type map(Func const& f, Container const& c)
{
	Container ret;
	ret.resize(c.size());
	for (size_t i = 0; i < c.size(); i++) {
		ret[i] = std::move(f(c[i]));
	}
	return std::move(ret);
}

#endif
