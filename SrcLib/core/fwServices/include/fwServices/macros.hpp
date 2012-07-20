/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_MACROS_HPP__
#define __FWSERVICES_MACROS_HPP__

#include <fwCore/concept_checks.hpp>

#include "fwServices/IService.hpp"

#include "fwServices/ServiceFactoryRegistrar.hpp"

namespace fwServices
{

/**
 * @name Macros for declaring service to object association
 */

//@{
/**
 * @brief Service object type association including all string-based registration
 * Associations concern Service-ObjectType are ObjectType-Service. Keys are typeid.
 * @author  IRCAD (Research and Development Team).
 */

#define fwServicesRegisterMacro( ServiceType , ServiceImpl, ServiceObject )                        \
    class BOOST_PP_CAT(  ServiceTypeConceptCheck , __LINE__ )                                      \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceType >));               \
    };                                                                                             \
    class BOOST_PP_CAT(  ServiceImplConceptCheck , __LINE__ )                                      \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceImpl >));               \
    };                                                                                             \
    class BOOST_PP_CAT(  ServiceObjectConceptCheck , __LINE__ )                                    \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceObject >));             \
    };                                                                                             \
    static ::fwServices::ServiceFactoryRegistrar< ServiceImpl >                                    \
        BOOST_PP_CAT( serviceRegistrar, __LINE__) ( #ServiceImpl , #ServiceType, #ServiceObject );

//@}

}

#endif /*__FWSERVICES_MACROS_HPP__*/
