#include <boost/random.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <cstdint>
#include <memory>

#include "map.h"

using namespace boost::python;

list map_u32(object& f, list& l)
{
	stl_input_iterator<uint32_t> begin(l), end;
	std::vector<uint32_t> buf(begin, end);
	std::vector<uint32_t> mapped = map(
			[&f](uint32_t const v)
			{
				object ret = f(v);
				return boost::python::extract<uint32_t>(f(v))();
			},
			buf, seq_backend());
	list ret; 
	for (uint32_t v: mapped) {
		ret.append(v);
	}
	return ret;
}

BOOST_PYTHON_MODULE(pymap)
{
	def("map_u32", &map_u32);
}
