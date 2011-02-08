#ifndef _FWPYTHON_INTERPRETER_HPP_
#define _FWPYTHON_INTERPRETER_HPP_

#include <string>
#include "fwPython/config.hpp"


namespace fwPython
{

class FWPYTHON_CLASS_API Interpreter {

public :

   // create a python interpretor
   FWPYTHON_API Interpreter();

    
   // a destroy the  python interpretor
   FWPYTHON_API ~Interpreter();

   //execute the python code sequence
   FWPYTHON_API int execute(std::string code);
};



} // namespace fwPython



#endif //_FWPYTHON_INTERPRETER_HPP_

