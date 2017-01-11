/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPython/bindings/base.hpp"
#include "fwPython/Interpreter.hpp"
#include "fwPython/python.hpp"

#include <fwCore/base.hpp>

#include <boost/python.hpp>

namespace fwPython
{

//------------------------------------------------------------------------------

::boost::python::dict GetNamespace( char const* mod )
{
    using namespace boost::python;
    dict moduleNamespace( import( mod ).attr( "__dict__" ) );
    return moduleNamespace;
}

//------------------------------------------------------------------------------

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
        m_globals = main.attr("__dict__");

        bp::exec("import sys\n"
                 "\n"
                 "class fwPyIOHandler:\n"
                 "    def __init__(self):\n"
                 "        import cStringIO\n"
                 "        self.listHook = []\n"
                 "        self.__stringIO = cStringIO.StringIO()\n"
                 "\n"
                 "    def write(self, msg):\n"
                 "        self.__stringIO.write(msg)\n"
                 "        for hook in self.listHook:\n"
                 "            hook.write(msg)\n"
                 "\n"
                 "    def read(self, msg):\n"
                 "        self.__stringIO.read(msg)\n"
                 "        for hook in self.listHook:\n"
                 "            hook.read(msg)\n"
                 "\n"
                 "    def close(self):\n"
                 "        self.__stringIO.close()\n"
                 "        for hook in self.listHook:\n"
                 "            hook.close()\n"
                 "\n"
                 "    def getvalue(self):\n"
                 "        return self.__stringIO.getvalue()\n"
                 "\n"
                 "sys.stderr = fwPyIOHandler()\n"
                 "sys.stdout = fwPyIOHandler()\n",
                 m_globals,
                 bp::dict());
    }
    catch( boost::python::error_already_set const&)
    {
        PyErr_Print();
    }
}

//------------------------------------------------------------------------------

Interpreter::~Interpreter()
{
    ::fwPython::finalize();
}

//------------------------------------------------------------------------------

void Interpreter::addObject( std::string key, ::fwTools::Object::sptr object)
{
    namespace bp = ::boost::python;
    try
    {
        ::boost::python::object pyObject = ::boost::python::object( object );
        m_globals[key]                   = pyObject;
    }
    catch( boost::python::error_already_set const&)
    {
        PyErr_Print();
    }
}

//------------------------------------------------------------------------------

int Interpreter::execute(std::string code)
{
    namespace bp = ::boost::python;
    int succes = 0;

    try
    {
        bp::object ignored = bp::exec( code.c_str(), m_globals, m_locals );
    }
    catch( bp::error_already_set const&)
    {
        PyErr_Print();
        succes = -1;
    }
    return succes;
}

//------------------------------------------------------------------------------

std::string Interpreter::getAttrValue(const std::string& moduleName, const std::string& attrName)
{
    namespace bp = ::boost::python;

    try
    {
        bp::object mod(bp::handle<>(PyImport_ImportModule(moduleName.c_str())));
        bp::object attr = mod.attr(attrName.c_str());
        return bp::extract<std::string>(attr.attr("getvalue")());
    }
    catch( bp::error_already_set const&)
    {
        PyErr_Print();

        bp::object sys(bp::handle<>(PyImport_ImportModule("sys")));
        bp::object out = sys.attr("stderr");
        return bp::extract<std::string>(out.attr("getvalue")());
    }
}

//------------------------------------------------------------------------------

}
