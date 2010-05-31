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
    FWDATA_API DynamicAttributesInitializer(){};
    FWDATA_API virtual void init( WPTR( ::fwTools::Object ) _obj);

};

} //namespace fwData

#endif /* FWDATA_DYNAMICATTRIBUTESINITIALIZER_HPP_ */
