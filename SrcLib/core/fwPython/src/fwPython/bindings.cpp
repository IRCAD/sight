#include <boost/python.hpp>
#include <iostream>

#include <fwTools/Object.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include "fwPython/bindings.hpp"





BOOST_PYTHON_MODULE(fwData) // create a void initimage() function 
{                         // install  Py_InitModule with name + methods/class/function
    using namespace boost::python;

    class_< ::fwTools::Object , ::fwTools::Object::sptr >("Object",no_init)
                .def("getClassname", &::fwTools::Object::className );


    class_< ::fwData::String, bases<  ::fwTools::Object  >, ::fwData::String::sptr >("String")
              .add_property("value",  &::fwData::String::getValue  ,  &::fwData::String::setValue );
    
    class_< ::fwData::Image, bases<  ::fwTools::Object  >, ::fwData::Image::sptr >("Image")
              .def("getBuffer",  &::fwData::Image::getBuffer );
}

namespace fwPython
{

void initBindings()
{
    static bool initialized = false;
    if (initialized)
    {
        return;
    }
    initfwData();
    initialized = true;
}

}
