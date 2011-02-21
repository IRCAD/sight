/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_SERVICEREGISTRAR_HPP_
#define _FWSERVICES_SERVICEREGISTRAR_HPP_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <fwCore/base.hpp>
#include <fwCore/Demangler.hpp>

#include <fwTools/ClassFactory.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwServices/registry/ServiceFactory.hpp"

#define REGISTER_BINDING_ID_CPY_V2( BaseClassType , SubClassType , KeyType, keyvalue, id )                                                            \
    static const KeyType  BOOST_PP_CAT(registredKeyValue, id ) = keyvalue;                                                                         \
    static ::fwServices::ServiceFactoryRegistrar< BaseClassType, SubClassType, KeyType > BOOST_PP_CAT( registrar, id ) ( BOOST_PP_CAT(registredKeyValue, id ) );


namespace fwServices {

/**
 * @brief This is an helper for registring an Class : internaly it create the factory and register it
 * to FactoryRegistry
 * @class ClassRegistrar
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
template<class BASECLASS, class SUBCLASS,class KEY>
class ServiceFactoryRegistrar
{
public:

    /**
     * @brief Constructor : create the factory and register it
     * @param[in] key type that factory can candle
     */
    ServiceFactoryRegistrar(const KEY & key)
    {
        // create factory
        ::boost::shared_ptr< ::fwTools::IClassFactory >  af( new ::fwTools::ClassFactory< BASECLASS,SUBCLASS,KEY >(key) );

        // register it
        ::fwTools::ClassFactoryRegistry::addFactory( af );
        ::fwServices::registry::ServiceFactory::getDefault()->addFactory( af, ::fwCore::TypeDemangler<SUBCLASS>().getClassname(), key.first, key.second);
    }

};

} //end namespace fwServices


#endif // _FWSERVICES_SERVICEREGISTRAR_HPP_
