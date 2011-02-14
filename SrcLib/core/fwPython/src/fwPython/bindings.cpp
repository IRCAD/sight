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
#include "fwPython/bindings/Image.hpp"


// HACK -- CRIME SCENE -- DOT NOT CROSS {
struct IServiceProxy
{
    typedef SPTR(IServiceProxy) sptr ;
    IServiceProxy(const ::fwTools::Object::sptr obj)
    {
        ::fwServices::IService::sptr service;
        service = ::fwServices::IService::dynamicCast(obj);
        SLM_ASSERT("Cast to service failed", service);
        this->m_service = service;
    };

    void start() { m_service->start();  };
    void update(){ m_service->update();   };
    void stop()  { m_service->stop(); };

    ::fwServices::IService::sptr m_service;

};
// }

 

IServiceProxy getSrv(std::string o)
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject( o );
    IServiceProxy proxy(obj);
    return proxy;
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
    def( "getSrv", &getSrv );

    // fwData::Object binding
    class_< ::fwData::Object, bases<  ::fwTools::Object  > , ::fwData::Object::sptr >("Object",no_init);

    // fwServices::IService binding

    class_< IServiceProxy >("IService",no_init)
        .def("update", &IServiceProxy::update)
        .def("start", &IServiceProxy::start)
        .def("stop", &IServiceProxy::stop)
        ;

    // fwData:: generic fields bindings
    class_< ::fwData::Boolean, bases<  ::fwData::Object  >, ::fwData::Boolean::sptr >("Boolean")
              .add_property("value",  &::fwData::Boolean::getValue  ,  &::fwData::Boolean::setValue );

    class_< ::fwData::Float, bases<  ::fwData::Object  >, ::fwData::Float::sptr >("Float")
              .add_property("value",  &::fwData::Float::getValue  ,  &::fwData::Float::setValue );

    class_< ::fwData::Integer, bases<  ::fwData::Object  >, ::fwData::Integer::sptr >("Integer")
              .add_property("value",  &::fwData::Integer::getValue  ,  &::fwData::Integer::setValue );

    class_< ::fwData::String, bases<  ::fwData::Object  >, ::fwData::String::sptr >("String")
              .add_property("value",  &::fwData::String::getValue  ,  &::fwData::String::setValue );
    
    ::fwPython::bindings::export_image();

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
