#include <boost/python.hpp>
#include "fwPython/Interpreter.hpp"


namespace fwPython
{

Interpreter::Interpreter()
{
   Py_Initialize();
}

Interpreter::~Interpreter()
{
    Py_Finalize();
}
    
int Interpreter::execute(std::string code)
{
   namespace bp = ::boost::python;
   int succes=0;

   try
   {
   bp::object ignored = bp::exec( code.c_str() );
   }
   catch( bp::error_already_set const &)
   {
      PyErr_Print();
      succes = -1;
   }
   return succes;
}


}
