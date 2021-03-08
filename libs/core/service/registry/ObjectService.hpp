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

#include "service/config.hpp"
#include "service/IService.hpp"

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

namespace sight::service
{
namespace registry
{

/**
 * @brief maintain the relation between objects and services
 *
 * The API of ObjectService should not be directly invoked,
 * service/op methods (i.e. add, get,...) should be used instead
 */
class SERVICE_CLASS_API ObjectService : public core::BaseObject,
                                        public core::com::HasSignals
{
public:

    SIGHT_DECLARE_CLASS(ObjectService, core::BaseObject)
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Service container
     * keeps relation between objects identifiers and attached services
     */
    typedef ::boost::bimaps::bimap<
            ::boost::bimaps::multiset_of< data::Object::cwptr, std::owner_less< data::Object::cwptr > >,
            ::boost::bimaps::multiset_of< service::IService::sptr >
            > ServiceContainerType;

    typedef std::set< CSPTR( data::Object ) >      ObjectVectorType;
    typedef std::set< SPTR( service::IService ) > ServiceVectorType;

    typedef std::map< service::IService::KeyType, WPTR( data::Object )> ObjectMapType;

    /**
     * @name Signals API
     * @{
     */
    /// Type of signal m_sigRenderRequested
    typedef core::com::Signal< void (data::Object::sptr, const std::string&) > RegisterSignalType;

    SERVICE_API static const core::com::Signals::SignalKeyType s_REGISTERED_SIG;
    SERVICE_API static const core::com::Signals::SignalKeyType s_UNREGISTERED_SIG;
    ///@}

    /// Constructor
    SERVICE_API ObjectService();

    /// Return some informations contain in the registry
    SERVICE_API std::string getRegistryInformation() const;

    /**
     * @name Management of registrations
     */
    //@{

    /**
     * @brief Register the service alone
     *
     * @param service Service to add to the OSR
     */
    SERVICE_API void registerService( service::IService::sptr service );

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
    SERVICE_API void registerService(data::Object::sptr object, const service::IService::KeyType& objKey,
                                     service::IService::AccessType access,
                                     service::IService::sptr service);

    /**
     * @brief Register the service (service) for the input object (obj) at the given service key.
     * It also updates IService inputs of service to point to obj
     * removal at obj destruction.
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    SERVICE_API void registerServiceInput(const data::Object::csptr& object,
                                          const service::IService::KeyType& objKey,
                                          const service::IService::sptr& service);

    /**
     * @brief Emit the signal 'registered'
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    SERVICE_API void registerServiceOutput(data::Object::sptr object,
                                           const service::IService::KeyType& objKey,
                                           service::IService::sptr service);
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
    SERVICE_API void unregisterService(  service::IService::sptr service );

    /**
     * @brief Remove an object key from a service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service whose key should be removed
     */
    SERVICE_API void unregisterService( const service::IService::KeyType& objKey,
                                        service::IService::AccessType access,
                                        service::IService::sptr service );

    /**
     * @brief Emit the signal 'unregistered'
     *
     * @param objKey Key of the object
     * @param service Service whose key should be removed
     */
    SERVICE_API void unregisterServiceOutput( const service::IService::KeyType& objKey,
                                              service::IService::sptr service );

    /**
     * @brief Return true if a key is registered for a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    SERVICE_API bool isRegistered( const service::IService::KeyType& objKey,
                                   service::IService::AccessType access,
                                   service::IService::sptr service) const;

    /**
     * @brief Return the object pointer of a key of a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    SERVICE_API data::Object::csptr getRegistered(const service::IService::KeyType& objKey,
                                                  service::IService::AccessType access,
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
    SERVICE_API ServiceVectorType getServices( const std::string& serviceType ) const;

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
    void internalRegisterService( data::Object::sptr obj, service::IService::sptr service,
                                  const service::IService::KeyType& objKey,
                                  service::IService::AccessType access);

    /**
     * @brief Register the service (service) for the input object (obj)
     * It also updates IService::m_inputs of service to point to obj
     * removal at obj destruction.
     * @warning not thread-safe
     */
    void internalRegisterServiceInput( const data::Object::csptr& obj, const service::IService::sptr& service,
                                       const service::IService::KeyType& objKey);

    /**
     * @brief container manipulator (Helper)
     * remove service from ObjectService containers, do not perform any "semantic action" like stopping service
     * @note (internal use) use with care
     */
    void  removeFromContainer( service::IService::sptr service );
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
SERVICE_API service::registry::ObjectService::sptr get();

/**
 * @brief Wraps ObjectService::getRegistryInformation
 */
SERVICE_API std::string getRegistryInformation();

/**
 * @brief Wraps ObjectService::getServices
 */
template<class SERVICE>
std::set< SPTR(SERVICE) > getServices();

/**
 * @brief Wraps ObjectService::getServices
 */
SERVICE_API service::registry::ObjectService::ServiceVectorType getServices( const std::string& serviceType );

/**
 * @brief Register the service alone
 *
 * @param service Service to add to the OSR
 */
SERVICE_API void registerService( service::IService::sptr service );

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
SERVICE_API void registerService(data::Object::sptr obj, const service::IService::KeyType& objKey,
                                 service::IService::AccessType access, service::IService::sptr service);

/**
 * @brief Register the service (service) for the input object (obj) at the given service key.
 * It also updates IService::m_inputs of service to point to obj
 * removal at obj destruction.
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
SERVICE_API void registerServiceInput(data::Object::csptr obj, const service::IService::KeyType& objKey,
                                      service::IService::sptr service);

/**
 * @brief Emit the signal 'registered'
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
SERVICE_API void registerServiceOutput(data::Object::sptr obj, const service::IService::KeyType& objKey,
                                       service::IService::sptr service);

/**
 * @brief Remove the service (service) from the m_container
 *
 * @param service Service whose key should be removed
 */
SERVICE_API void unregisterService( service::IService::sptr service );

/**
 * @brief Remove an object key from a service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service whose key should be removed
 */
SERVICE_API void unregisterService( const service::IService::KeyType& objKey,
                                    service::IService::AccessType access,
                                    service::IService::sptr service );

/**
 * @brief Emit the signal 'unregistered'
 *
 * @param objKey Key of the object
 * @param service Service whose key should be removed
 */
SERVICE_API void unregisterServiceOutput( const service::IService::KeyType& objKey,
                                          service::IService::sptr service );

/**
 * @brief Return true if a key is registered for a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
SERVICE_API bool isRegistered(const service::IService::KeyType& objKey,
                              service::IService::AccessType access,
                              service::IService::sptr service);

/**
 * @brief Return the object pointer of a key of a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
SERVICE_API data::Object::csptr getRegistered( const service::IService::KeyType& objKey,
                                               service::IService::AccessType access,
                                               service::IService::sptr service );

SERVICE_API SPTR( service::registry::ObjectService::RegisterSignalType ) getRegisterSignal();
SERVICE_API SPTR( service::registry::ObjectService::RegisterSignalType ) getUnregisterSignal();

} // namespace OSR

} // namespace sight::service

#include "service/registry/ObjectService.hxx"
