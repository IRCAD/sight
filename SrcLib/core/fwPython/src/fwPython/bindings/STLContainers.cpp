#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 

#include <vector>

#include <fwTools/Stringizer.hpp>
#include "fwPython/bindings/STLContainers.hpp"

namespace fwPython 
{
namespace bindings
{


std::string getString(std::vector<double> &v)
{
   return "std::vector<double> : " + fwTools::getString(v.begin(),v.end());
} 


void export_STLContainers()
{
    using namespace boost::python;
    class_< std::vector<double> >( "vectorDouble" , "binding of std::vector<double>", init<std::vector<double>::size_type> () )
       .def(vector_indexing_suite<std::vector<double> >()) // install wrapper to __len__, __getitem__, __setitem__, __delitem__, __iter__ and __contains.
       .def("__str__",getString);// allow a pretty print 
}

} // end namespace bindings
} // end namespace fwPython
