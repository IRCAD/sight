#include <boost/foreach.hpp>
#include <boost/python.hpp>
#include <fwCore/base.hpp>

#include "fwPython/python.hpp"
#include "fwPython/bindings/base.hpp"

#include "fwPython/Interpreter.hpp"

namespace fwPython
{


boost::python::dict GetNamespace( char const* mod )
{
    using namespace boost::python;
    dict moduleNamespace( import( mod ).attr( "__dict__" ) );
    return moduleNamespace;
}





Interpreter::Interpreter()
{
   ::fwPython::initialize();
   //SLM_ASSERT( "python is not initialized", ::fwPython::isInitialized() );

   namespace bp = ::boost::python;

   try
   {
       // Retrieve the main module.
       bp::object main = bp::import("__main__");
       // Retrieve the main module's namespace
       m_globals= main.attr("__dict__");
   }
   catch( boost::python::error_already_set const &)
   {
       PyErr_Print();
   }
}



Interpreter::~Interpreter()
{
   ::fwPython::finalize();
}


void Interpreter::addObject( std::string key, ::fwTools::Object::sptr object)
{
   namespace bp = ::boost::python;
    try
    {
        ::boost::python::object pyObject = ::boost::python::object( object );
        m_globals[key] = pyObject;
    }
   catch( boost::python::error_already_set const &)
    {
        PyErr_Print();
    }
}



int Interpreter::execute(std::string code)
{
   namespace bp = ::boost::python;
   int succes=0;

   try
   {
       bp::object ignored = bp::exec( code.c_str(), m_globals, m_locals );
   }
   catch( bp::error_already_set const &)
   {
      PyErr_Print();
      succes = -1;
   }
   return succes;
}


}
