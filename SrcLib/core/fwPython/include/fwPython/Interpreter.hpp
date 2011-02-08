#ifndef _FWPYTHON_INTERPRETER_HPP_
#define _FWPYTHON_INTERPRETER_HPP_

#include <string>

namespace fwPython
{

class Interpreter {

public :

   // create a python interpretor
   Interpreter();

    
   // a destroy the  python interpretor
   ~Interpreter();

   //execute the python code sequence
   int execute(std::string code);
};



} // namespace fwPython



#endif //_FWPYTHON_INTERPRETER_HPP_

