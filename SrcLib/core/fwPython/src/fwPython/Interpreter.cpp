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
   bp::object ignored = bp::exec( code.c_str() );
   return succes;
}


}
