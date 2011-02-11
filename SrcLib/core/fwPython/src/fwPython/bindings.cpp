#include <boost/python.hpp>
#include <iostream>

#include <fwTools/Object.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include "fwPython/python.hpp"
#include "fwPython/bindings.hpp"





BOOST_PYTHON_MODULE(fwData) // create a void initimage() function 
{                         // install  Py_InitModule with name + methods/class/function
    using namespace boost::python;

    class_< ::fwTools::Object , ::fwTools::Object::sptr >("Object",no_init)
                .def("getClassname", &::fwTools::Object::className );

    class_< ::fwData::Object, bases<  ::fwTools::Object  > , ::fwData::Object::sptr >("Object",no_init);

    class_< ::fwData::Boolean, bases<  ::fwData::Object  >, ::fwData::Boolean::sptr >("Boolean")
              .add_property("value",  &::fwData::Boolean::getValue  ,  &::fwData::Boolean::setValue );

    class_< ::fwData::Float, bases<  ::fwData::Object  >, ::fwData::Float::sptr >("Float")
              .add_property("value",  &::fwData::Float::getValue  ,  &::fwData::Float::setValue );

    class_< ::fwData::Integer, bases<  ::fwData::Object  >, ::fwData::Integer::sptr >("Integer")
              .add_property("value",  &::fwData::Integer::getValue  ,  &::fwData::Integer::setValue );

    class_< ::fwData::String, bases<  ::fwData::Object  >, ::fwData::String::sptr >("String")
              .add_property("value",  &::fwData::String::getValue  ,  &::fwData::String::setValue );
    
    //class_< ::fwData::Image, bases<  ::fwData::Object  >, ::fwData::Image::sptr >("Image")
              //.def("getBuffer",  &::fwData::Image::getBuffer );
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

    SLM_ASSERT(
            "Python interpreter have to be initialzed before bindings.",
            ::fwPython::isInitialized()
            );

    initfwData();
    initialized = true;
}

} // namespace fwPython
