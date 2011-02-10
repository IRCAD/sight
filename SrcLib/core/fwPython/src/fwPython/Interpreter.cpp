#include <boost/python.hpp>
#include <fwCore/base.hpp>
#include "fwPython/Interpreter.hpp"
#include "fwPython/bindings.hpp"

namespace fwPython
{

Interpreter::Interpreter() 
{
   namespace bp = ::boost::python;

   Py_Initialize();
   SLM_ASSERT( "python initialization failed ",Py_IsInitialized() );

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
    Py_Finalize();
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
