#ifndef FWDATA_DYNAMICATTRIBUTESINITIALIZER_HPP_
#define FWDATA_DYNAMICATTRIBUTESINITIALIZER_HPP_

#include <fwCore/base.hpp>
#include <fwTools/Factory.hpp>

#include "fwData/config.hpp"

namespace fwData
{

class FWDATA_CLASS_API  DynamicAttributesInitializer: public ::fwTools::Factory::Initializer
{

public :
    fwCoreClassDefinitionsWithFactoryMacro((DynamicAttributesInitializer), (()), new DynamicAttributesInitializer );

    FWDATA_API DynamicAttributesInitializer(){};
    FWDATA_API virtual void init( ::fwTools::Object::wptr _obj);
    FWDATA_API virtual void uninit( ::fwCore::LogicStamp::csptr key );

};

} //namespace fwData

#endif /* FWDATA_DYNAMICATTRIBUTESINITIALIZER_HPP_ */
