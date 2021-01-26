/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "services/config.hpp"
#include "services/IService.hpp"

#include <core/com/HasSignals.hpp>

#include <core/LogicStamp.hpp>
#include <core/mt/types.hpp>

#include <core/tools/Failed.hpp>
#include <core/tools/Object.hpp>

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <map>
#include <unordered_map>
#include <set>

namespace sight::data
{
class Object;
}

namespace sight::services
{
namespace registry
{

/**
 * @brief maintain the relation between objects and services
 *
 * The API of ObjectService should not be directly invoked,
 * services/op methods (i.e. add, get,...) should be used instead
 */
class SERVICES_CLASS_API ObjectService : public core::BaseObject,
                                         public core::com::HasSignals
{
public:

    fwCoreClassMacro(ObjectService, core::BaseObject)
    fwCoreAllowSharedFromThis();

    /**
     * @brief Service container
     * keeps relation between objects identifiers and attached services
     */
    typedef ::boost::bimaps::bimap<
            ::boost::bimaps::multiset_of< data::Object::cwptr, std::owner_less< data::Object::cwptr > >,
            ::boost::bimaps::multiset_of< services::IService::sptr >
            > ServiceContainerType;

    typedef std::set< CSPTR( data::Object ) >      ObjectVectorType;
    typedef std::set< SPTR( services::IService ) > ServiceVectorType;

    typedef std::map< services::IService::KeyType, WPTR( data::Object )> ObjectMapType;

    /**
     * @name Signals API
     * @{
     */
    /// Type of signal m_sigRenderRequested
    typedef core::com::Signal< void (data::Object::sptr, const std::string&) > RegisterSignalType;

    SERVICES_API static const core::com::Signals::SignalKeyType s_REGISTERED_SIG;
    SERVICES_API static const core::com::Signals::SignalKeyType s_UNREGISTERED_SIG;
    ///@}

    /// Constructor
    SERVICES_API ObjectService();

    /// Return some informations contain in the registry
    SERVICES_API std::string getRegistryInformation() const;

    /**
     * @name Management of registrations
     */
    //@{

    /**
     * @brief Register the service alone
     *
     * @param service Service to add to the OSR
     */
    SERVICES_API void registerService( services::IService::sptr service );

    /**
     * @brief Register the service (service) for the object (obj) at the given service key.
     * It also updates IService objects of service to point to obj
     * removal at obj destruction.
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service whose key should be added
     */
    SERVICES_API void registerService(data::Object::sptr object, const services::IService::KeyType& objKey,
                                      services::IService::AccessType access,
                                      services::IService::sptr service);

    /**
     * @brief Register the service (service) for the input object (obj) at the given service key.
     * It also updates IService inputs of service to point to obj
     * removal at obj destruction.
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    SERVICES_API void registerServiceInput(const data::Object::csptr& object,
                                           const services::IService::KeyType& objKey,
                                           const services::IService::sptr& service);

    /**
     * @brief Emit the signal 'registered'
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    SERVICES_API void registerServiceOutput(data::Object::sptr object,
                                            const services::IService::KeyType& objKey,
                                            services::IService::sptr service);
    //@}

    /**
     * @name Management of unregistrations
     */

    //@{

    /**
     * @brief Remove the service (service) from the m_container
     *
     * @param service Service whose key should be removed
     */
    SERVICES_API void unregisterService(  services::IService::sptr service );

    /**
     * @brief Remove an object key from a service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service whose key should be removed
     */
    SERVICES_API void unregisterService( const services::IService::KeyType& objKey,
                                         services::IService::AccessType access,
                                         services::IService::sptr service );

    /**
     * @brief Emit the signal 'unregistered'
     *
     * @param objKey Key of the object
     * @param service Service whose key should be removed
     */
    SERVICES_API void unregisterServiceOutput( const services::IService::KeyType& objKey,
                                               services::IService::sptr service );

    /**
     * @brief Return true if a key is registered for a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    SERVICES_API bool isRegistered( const services::IService::KeyType& objKey,
                                    services::IService::AccessType access,
                                    services::IService::sptr service) const;

    /**
     * @brief Return the object pointer of a key of a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    SERVICES_API data::Object::csptr getRegistered(const services::IService::KeyType& objKey,
                                                   services::IService::AccessType access,
                                                   IService::sptr service) const;
    //@}

    /**
     * @name Some useful getters
     */

    //@{

    /**
     * @brief Return a container with all services of type SERVICE registered in m_container
     * @note Services may be associated to different object
     */
    template<class SERVICE>
    std::set< SPTR(SERVICE) > getServices() const;

    /**
     * @brief Return registered services matching serviceType
     * @note Should be optimized
     * @note Invoke getServices( data::Object::sptr , const std::string & ) for each registered object
     *
     */
    SERVICES_API ServiceVectorType getServices( const std::string& serviceType ) const;

    //@}

protected:

    /// Registered services
    ServiceVectorType m_services;

    mutable core::mt::ReadWriteMutex m_containerMutex;

private:

    /**
     * @brief Register the service (service) for the object (obj)
     * It also updates IService objects of service to point to obj
     * removal at obj destruction.
     * @warning not thread-safe
     */
    void internalRegisterService( data::Object::sptr obj, services::IService::sptr service,
                                  const services::IService::KeyType& objKey,
                                  services::IService::AccessType access);

    /**
     * @brief Register the service (service) for the input object (obj)
     * It also updates IService::m_inputs of service to point to obj
     * removal at obj destruction.
     * @warning not thread-safe
     */
    void internalRegisterServiceInput( const data::Object::csptr& obj, const services::IService::sptr& service,
                                       const services::IService::KeyType& objKey);

    /**
     * @brief container manipulator (Helper)
     * remove service from ObjectService containers, do not perform any "semantic action" like stopping service
     * @note (internal use) use with care
     */
    void  removeFromContainer( services::IService::sptr service );
};

} // namespace registry

/**
 * @brief OSR wraps main Object-Service registry access
 */
namespace OSR
{

/**
 * @brief returns the Object-Service registry main instance
 */
SERVICES_API services::registry::ObjectService::sptr get();

/**
 * @brief Wraps ObjectService::getRegistryInformation
 */
SERVICES_API std::string getRegistryInformation();

/**
 * @brief Wraps ObjectService::getServices
 */
template<class SERVICE>
std::set< SPTR(SERVICE) > getServices();

/**
 * @brief Wraps ObjectService::getServices
 */
SERVICES_API services::registry::ObjectService::ServiceVectorType getServices( const std::string& serviceType );

/**
 * @brief Register the service alone
 *
 * @param service Service to add to the OSR
 */
SERVICES_API void registerService( services::IService::sptr service );

/**
 * @brief Register the service (service) for the object (obj) at the given service key.
 * It also updates IService objects of service to point to obj
 * removal at obj destruction.
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service whose key should be added
 */
SERVICES_API void registerService(data::Object::sptr obj, const services::IService::KeyType& objKey,
                                  services::IService::AccessType access, services::IService::sptr service);

/**
 * @brief Register the service (service) for the input object (obj) at the given service key.
 * It also updates IService::m_inputs of service to point to obj
 * removal at obj destruction.
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
SERVICES_API void registerServiceInput(data::Object::csptr obj, const services::IService::KeyType& objKey,
                                       services::IService::sptr service);

/**
 * @brief Emit the signal 'registered'
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
SERVICES_API void registerServiceOutput(data::Object::sptr obj, const services::IService::KeyType& objKey,
                                        services::IService::sptr service);

/**
 * @brief Remove the service (service) from the m_container
 *
 * @param service Service whose key should be removed
 */
SERVICES_API void unregisterService( services::IService::sptr service );

/**
 * @brief Remove an object key from a service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service whose key should be removed
 */
SERVICES_API void unregisterService( const services::IService::KeyType& objKey,
                                     services::IService::AccessType access,
                                     services::IService::sptr service );

/**
 * @brief Emit the signal 'unregistered'
 *
 * @param objKey Key of the object
 * @param service Service whose key should be removed
 */
SERVICES_API void unregisterServiceOutput( const services::IService::KeyType& objKey,
                                           services::IService::sptr service );

/**
 * @brief Return true if a key is registered for a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
SERVICES_API bool isRegistered(const services::IService::KeyType& objKey,
                               services::IService::AccessType access,
                               services::IService::sptr service);

/**
 * @brief Return the object pointer of a key of a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
SERVICES_API data::Object::csptr getRegistered( const services::IService::KeyType& objKey,
                                                services::IService::AccessType access,
                                                services::IService::sptr service );

SERVICES_API SPTR( services::registry::ObjectService::RegisterSignalType ) getRegisterSignal();
SERVICES_API SPTR( services::registry::ObjectService::RegisterSignalType ) getUnregisterSignal();

} // namespace OSR

} // namespace sight::services

#include "services/registry/ObjectService.hxx"
