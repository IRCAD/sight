
#ifndef _FWDATACAMP_FACTORY_HPP_
#define _FWDATACAMP_FACTORY_HPP_

#include <fwTools/Object.hpp>
#include <fwCamp/UserObject.hpp>

#include "fwDataCamp/config.hpp"

#ifdef WIN32
    typedef ::camp::UserObject * (*FactoryType) (::fwTools::Object *);

    namespace fwCamp
    {

    FWDATACAMP_TEMPLATE_API template  class FWDATACAMP_API Factory< FactoryType >;

    }  // namespace fwCamp

#endif


#endif // _FWDATACAMPP_FACTORY_HPP_

