#include <boost/python.hpp>
#include <boost/cstdint.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>

#include <fwTools/Stringizer.hpp>
#include <fwCore/Demangler.hpp>
#include "fwPython/bindings/STLContainers.hpp"

namespace fwPython
{
namespace bindings
{

template <class T>
std::string getString(std::vector<T> &v)
{
    std::string res;
    res +=  fwTools::getString(v.begin(),v.end());
    return res;
}


template< typename T >
void wrap_vector()
{
    using namespace boost::python;
    std::string className = ::fwCore::Demangler(typeid(typename T::value_type)).getLeafClassname();
    std::string docString = std::string( "binding of " ) + ::fwCore::Demangler(typeid(T)).getClassname();
    class_< T >( className.c_str(), docString.c_str(), init< typename T::size_type > () )

        // install wrapper to __len__, __getitem__, __setitem__, __delitem__, __iter__ and __contains.
        .def(vector_indexing_suite< T >())

        // allow a pretty print
        .def("__str__", getString< typename T::value_type >)
        ;
}
//------------------------------------------------------------------------------

void export_STLContainers()
{
    using namespace boost::python;
    wrap_vector< std::vector<boost::int8_t> >();
    wrap_vector< std::vector<boost::int16_t> >();
    wrap_vector< std::vector<boost::int32_t> >();
    wrap_vector< std::vector<boost::int64_t> >();

    wrap_vector< std::vector<boost::uint8_t> >();
    wrap_vector< std::vector<boost::uint16_t> >();
    wrap_vector< std::vector<boost::uint32_t> >();
    wrap_vector< std::vector<boost::uint64_t> >();

    wrap_vector< std::vector<float> >();
    wrap_vector< std::vector<double> >();

#ifndef WIN32
    wrap_vector< std::vector<size_t> >();
#endif
}

} // end namespace bindings
} // end namespace fwPython
