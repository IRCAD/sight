/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_MACROS_HPP__
#define __FWSERVICES_MACROS_HPP__

#include "fwServices/IService.hpp"
#include "fwServices/ServiceFactoryRegistrar.hpp"

#include <fwCore/concept_checks.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

namespace fwServices
{

/**
 * @name Macros for declaring service to object association
 */

#define __FWSERVICES_REGISTER_MACRO( ServiceType, ServiceImpl )                                    \
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
    static ::fwServices::ServiceFactoryRegistrar< ServiceImpl >                                    \
    BOOST_PP_CAT( serviceRegistrar, __LINE__) ( #ServiceImpl, #ServiceType );

#define __FWSERVICES_REGISTER_OBJECT_MACRO( ServiceImpl, ServiceObject )                           \
    class BOOST_PP_CAT (  ServiceObjectConceptCheck, __LINE__ )                                    \
    {                                                                                              \
    public:                                                                                        \
        BOOST_CONCEPT_ASSERT((::fwCore::concepts::SharedPtrTypedef< ServiceObject >));             \
    };                                                                                             \
    static ::fwServices::ServiceObjectFactoryRegistrar                                             \
    BOOST_PP_CAT( serviceObjectRegistrar, __LINE__) ( #ServiceImpl, #ServiceObject );

//@{

#define __FWSERVICES_REGISTER_MACRO_2(ServiceImpl, ServiceObject)                                  \
    __FWSERVICES_REGISTER_MACRO(ServiceImpl, ServiceObject)

#define __FWSERVICES_REGISTER_MACRO_3(ServiceType, ServiceImpl, ServiceObject)                     \
    __FWSERVICES_REGISTER_MACRO(ServiceType, ServiceImpl)                                          \
    __FWSERVICES_REGISTER_OBJECT_MACRO(ServiceImpl, ServiceObject)

/**
 * @brief Service object type association including all string-based registration
 * Associations concern Service-ObjectType are ObjectType-Service. Keys are typeid.
 */
#if !BOOST_PP_VARIADICS_MSVC

#define fwServicesRegisterMacro(...) BOOST_PP_OVERLOAD(__FWSERVICES_REGISTER_MACRO_,__VA_ARGS__)(__VA_ARGS__)

#else

#define fwServicesRegisterMacro(...) \
    BOOST_PP_CAT(BOOST_PP_OVERLOAD(__FWSERVICES_REGISTER_MACRO_,__VA_ARGS__)(__VA_ARGS__),BOOST_PP_EMPTY())

#endif
//@}

}

#endif /*__FWSERVICES_MACROS_HPP__*/
