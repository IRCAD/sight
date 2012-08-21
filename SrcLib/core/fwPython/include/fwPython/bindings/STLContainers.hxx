#include <boost/python.hpp>
#include <boost/python/tuple.hpp>

#undef tolower //defined by python, conflicting with std::lower

namespace fwPython {
namespace bindings {



template< typename STDCONTAINER>
::boost::python::list make_pylist( STDCONTAINER &cont)
{
    ::boost::python::list list;
     typename STDCONTAINER::const_iterator iter;
    for ( iter=cont.begin(); iter != cont.end(); ++iter )
    {
        list.append(  ::boost::python::object(*iter) );
    }
    return list;
}


} //namespace bindings
} //namespace fwPython

