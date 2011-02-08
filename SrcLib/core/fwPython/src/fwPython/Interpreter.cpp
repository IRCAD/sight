#include "fwPython/Interpreter.hpp"
#include <Python.h>


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
    
void Interpreter::execute()
{
   PyRun_SimpleString("print \"hello world\"");
}

}
