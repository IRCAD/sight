#include <boost/python.hpp>
#include <iostream>

#include <fwTools/Object.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include "fwPython/python.hpp"



namespace fwPython
{

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

    //actually boost recommands to *not finalize* python
    //Py_Finalize();
    return;
}

} // namespace fwPython
