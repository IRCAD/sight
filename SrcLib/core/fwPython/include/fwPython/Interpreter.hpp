#ifndef _FWPYTHON_INTERPRETER_HPP_
#define _FWPYTHON_INTERPRETER_HPP_

namespace fwPython
{

class Interpreter {

public :

   // create a python interpretor
   Interpreter();

    
   // a destroy the  python interpretor
   ~Interpreter();

   //execute the script
   void execute();
};



} // namespace fwPython



#endif //_FWPYTHON_INTERPRETER_HPP_

