#ifndef MAP_H
#define MAP_H

template <class Func, class Container>
Container map(Func const& f, Container const& c)
{
	Container ret;
	for (auto const& v: c) {
		ret.push_back(f(v));
	}
	return ret;
}

#endif
