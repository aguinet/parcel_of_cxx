#include <boost/random.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <cstdint>
#include <memory>

#include "map.h"

using namespace boost::python;

struct myobj
{
	myobj()
	{
		std::cout << "constructor" << std::endl;
	}
	
	~myobj()
	{
		std::cout << "destructor" << std::endl;
	}

	static void* operator new(std::size_t sz)
	{
		std::cout << "custom new for size " << sz << '\n';
		return ::operator new(sz);
	}

	static void operator delete(void* ptr)
	{
		std::cout << "custom placement delete called" << std::endl;
		::operator delete(ptr);
	}
};


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

myobj* new_obj()
{
	return new myobj();
}

std::shared_ptr<myobj> new_shared_obj()
{
	return std::shared_ptr<myobj>(new myobj());
}

BOOST_PYTHON_MODULE(pymap)
{
	def("map_u32", &map_u32);
	def("new_obj", &new_obj, return_value_policy<manage_new_object>());
	def("new_shared_obj", &new_shared_obj);

	class_<myobj, std::shared_ptr<myobj>>("myobj");
}
