#include <fwCore/base.hpp>
#include <fwTools/Factory.hpp>

#include "fwData/DynamicAttributesInitializer.hpp"
#include "fwData/Object.hpp"

class InitializerRegistrar
{
public :
    InitializerRegistrar()
    {
        ::fwTools::Factory::addInitializer( ::boost::shared_ptr< ::fwTools::Factory::Initializer >( new ::fwData::DynamicAttributesInitializer ) ) ;
    }

    ~InitializerRegistrar()
    {}
} ;

WPTR(InitializerRegistrar) reg = SPTR(InitializerRegistrar)(new InitializerRegistrar);


namespace fwData
{

void DynamicAttributesInitializer::init( ::fwTools::Object::wptr _obj)
{
    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(_obj.lock());
    if (obj)
    {
        obj->__FWTOOLS_ATTRIBUTES_REGISTER_FUNC_NAME();
    }
}



} //end namespace fwData
