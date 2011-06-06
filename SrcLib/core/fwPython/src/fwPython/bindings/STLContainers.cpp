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
    res =  "std::vector< " + ::fwCore::Demangler(typeid(T)).getLeafClassname() +" > : ";
    res +=  fwTools::getString(v.begin(),v.end());
    return res;
}

//------------------------------------------------------------------------------

void export_STLContainers()
{
    using namespace boost::python;
    class_< std::vector<double> >( "vectorDouble" , "binding of std::vector<double>", init<std::vector<double>::size_type> () )
       .def(vector_indexing_suite<std::vector<double> >()) // install wrapper to __len__, __getitem__, __setitem__, __delitem__, __iter__ and __contains.
       .def("__str__",getString<double>);// allow a pretty print

    class_< std::vector<boost::int32_t> >( "vectorint32" , "binding of std::vector<<boost::int32_t>", init<std::vector<boost::int32_t>::size_type> () )
       .def(vector_indexing_suite<std::vector<boost::int32_t> >()) // install wrapper to __len__, __getitem__, __setitem__, __delitem__, __iter__ and __contains.
       .def("__str__",getString<boost::int32_t>);// allow a pretty print
}

} // end namespace bindings
} // end namespace fwPython
