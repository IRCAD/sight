#ifndef _FWPYTHON_INTERPRETER_HPP_
#define _FWPYTHON_INTERPRETER_HPP_

#include <boost/python.hpp>
#include <string>

#include <fwTools/Object.hpp>

#include "fwPython/config.hpp"




namespace fwPython
{





class FWPYTHON_CLASS_API Interpreter {

public :

   // create a python interpretor
   FWPYTHON_API Interpreter();

    
   // a destroy the  python interpretor
   FWPYTHON_API ~Interpreter();

   // insert an object inside the python global dictionnay with the specified key
   FWPYTHON_API void addObject(std::string key, ::fwTools::Object::sptr object);


   //execute the python code sequence
   FWPYTHON_API int execute(std::string code);

   protected :

   void importBindings();

   //direct accesor to global dict
   boost::python::object m_globals;
   //direct accesor to local dict
   boost::python::dict m_locals;

};



} // namespace fwPython



#endif //_FWPYTHON_INTERPRETER_HPP_

