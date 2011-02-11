#include <boost/python.hpp>
#include <iostream>

#include <fwTools/Object.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IService.hpp>

#include "fwPython/python.hpp"
#include "fwPython/bindings.hpp"


//struct IServiceWrap : public ::fwServices::IService
//{
    //typedef SPTR(IServiceWrap) sptr ;
    //void starting() throw ( ::fwTools::Failed ) {} ;
    //void stopping() throw ( ::fwTools::Failed ) {} ;
    //void configuring() throw ( ::fwTools::Failed ) {} ;
    //void updating() throw ( ::fwTools::Failed ) {} ;
    //void updating( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ) {} ;
//};

//void (IServiceWrap::*updating1) () throw (::fwTools::Failed) = &IServiceWrap::updating;
//void (IServiceWrap::*updating2) (fwServices::ObjectMsg::csptr) throw (::fwTools::Failed) = &IServiceWrap::updating;


::boost::python::object getImageBuffer (::fwData::Image::sptr image) {
    using namespace boost::python;
    handle<> bufHandle (PyBuffer_FromReadWriteMemory ((void*)(image->getBuffer()), ::fwData::imageSizeInBytes(*image) ));
    return object( bufHandle );
}


//BOOST_PYTHON_MODULE(fwTools)
//{
//}

BOOST_PYTHON_MODULE(fwData) // create a void initimage() function 
{                         // install  Py_InitModule with name + methods/class/function
    using namespace boost::python;

    // fwTools::Object binding
    class_< ::fwTools::Object , ::fwTools::Object::sptr >("Object",no_init)
                .def("getClassname", &::fwTools::Object::className );

    def( "getObject", &::fwTools::fwID::getObject );

    // fwData::Object binding
    class_< ::fwData::Object, bases<  ::fwTools::Object  > , ::fwData::Object::sptr >("Object",no_init);

    // fwService::IService binding

    //class_< IServiceWrap, bases<  ::fwTools::Object  > , IServiceWrap::sptr >("IService",no_init)
        //.def("update", updating1)
        ////.def("start", &IServiceWrap::start)
        ////.def("stop", &IServiceWrap::stop)
        //;

    // fwData:: generic fields bindings
    class_< ::fwData::Boolean, bases<  ::fwData::Object  >, ::fwData::Boolean::sptr >("Boolean")
              .add_property("value",  &::fwData::Boolean::getValue  ,  &::fwData::Boolean::setValue );

    class_< ::fwData::Float, bases<  ::fwData::Object  >, ::fwData::Float::sptr >("Float")
              .add_property("value",  &::fwData::Float::getValue  ,  &::fwData::Float::setValue );

    class_< ::fwData::Integer, bases<  ::fwData::Object  >, ::fwData::Integer::sptr >("Integer")
              .add_property("value",  &::fwData::Integer::getValue  ,  &::fwData::Integer::setValue );

    class_< ::fwData::String, bases<  ::fwData::Object  >, ::fwData::String::sptr >("String")
              .add_property("value",  &::fwData::String::getValue  ,  &::fwData::String::setValue );
    
    class_< ::fwData::Image, bases<  ::fwData::Object  >, ::fwData::Image::sptr >("Image")
              .def("getBuffer",  &getImageBuffer );
}

namespace fwPython
{

void initializeBindings()
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
