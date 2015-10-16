/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_MACROS_HPP__
#define __FWSERVICES_MACROS_HPP__

#include <fwCore/concept_checks.hpp>

#include <fwCom/util/log.hpp>

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
 *
 */

#define fwServicesRegisterMacro( ServiceType, ServiceImpl, ServiceObject )                        \
    class BOOST_PP_CAT (  ServiceTypeConceptCheck, __LINE__ )                                      \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceType >));               \
    };                                                                                             \
    class BOOST_PP_CAT (  ServiceImplConceptCheck, __LINE__ )                                      \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceImpl >));               \
    };                                                                                             \
    class BOOST_PP_CAT (  ServiceObjectConceptCheck, __LINE__ )                                    \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceObject >));             \
    };                                                                                             \
    static ::fwServices::ServiceFactoryRegistrar< ServiceImpl >                                    \
    BOOST_PP_CAT( serviceRegistrar, __LINE__) ( #ServiceImpl, #ServiceType, #ServiceObject );

//@}

/// Signal async emit Parameters. Print SLM_COM with Sender string.
#define fwServicesNotifyMacro( Sender, Signal, Parameters)                     \
    OSLM_COM(Sender << " uses '" << Signal->getID() << "' sig to asyncEmit."); \
    Signal->asyncEmit Parameters;

/// Signal async emit Parameters and block given Slot connection. Print SLM_COM with Sender string.
#define fwServicesBlockAndNotifyMacro( Sender, Signal, Parameters, Slot)    \
    {                                                                       \
        ::fwCom::Connection::Blocker block(Signal->getConnection(Slot));    \
        fwServicesNotifyMacro(Sender, Signal, Parameters)                   \
    }

}

#endif /*__FWSERVICES_MACROS_HPP__*/
