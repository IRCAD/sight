#include <iostream>
#include <boost/python.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include "fwPython/python.hpp"



namespace fwPython
{

class Config
{
public:
    static char *home;

};

char *Config::home = NULL;




char * charFromString(std::string str)
{
    char *dest = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), dest);
    dest[str.size()] = '\0';
    return dest;
}


void initialize()
{
    if(!Py_IsInitialized())
    {
        Py_InitializeEx(0); // 0 -> do not handle signals
        SLM_ASSERT( "python initialization failed ", isInitialized() );
    }
}

bool isInitialized()
{
    return Py_IsInitialized();
}

void finalize()
{
    //Currently, boost recommands to *not* finalize python
    //Py_Finalize();
    return;
}


void setHome(std::string dir)
{
    if(!Py_IsInitialized())
    {
        if (::fwPython::Config::home)
        {
            delete [] ::fwPython::Config::home;
        }

        ::fwPython::Config::home = charFromString(dir);

        Py_SetPythonHome( ::fwPython::Config::home );
        OSLM_INFO("Python Home set to " << dir);
    }
    SLM_ERROR_IF("Python home must be set before python initialisation", Py_IsInitialized());
}

void addPath(std::string dir)
{
    namespace bp = ::boost::python;
    SLM_ERROR_IF("Python must be initialised before adding a new dir", Py_IsInitialized());

    bp::object sys = bp::import("sys");
    sys.attr("path").attr("insert")(0, dir);

    OSLM_INFO(dir << " appended to Python Path");
}

char* getHome()
{
    return ::fwPython::Config::home;
}



} // namespace fwPython
