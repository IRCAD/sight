/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPYTHON_INTERPRETER_HPP__
#define __FWPYTHON_INTERPRETER_HPP__

#include "fwPython/config.hpp"

#include <fwTools/Object.hpp>

#include <boost/python.hpp>
#undef tolower //defined by python, conflicting with std::lower

#include <string>

namespace fwPython
{

class FWPYTHON_CLASS_API Interpreter
{

public:

    // create a python interpretor
    FWPYTHON_API Interpreter();


    // a destroy the  python interpretor
    FWPYTHON_API ~Interpreter();

    // insert an object inside the python global dictionnay with the specified key
    FWPYTHON_API void addObject(std::string key, ::fwTools::Object::sptr object);


    //execute the python code sequence
    FWPYTHON_API int execute(std::string code);

    /**
     * @brief Return the value of the given sys module attribute value.
     *
     * @param moduleName Python module name
     * @param attrName   attribute name of the specified Python module
     *
     * @return attribute value
     */
    FWPYTHON_API std::string getAttrValue(const std::string& moduleName, const std::string& attrName);

protected:

    void importBindings();

    //direct accesor to global dict
    boost::python::object m_globals;
    //direct accesor to local dict
    boost::python::dict m_locals;

};



} // namespace fwPython



#endif //__FWPYTHON_INTERPRETER_HPP__

