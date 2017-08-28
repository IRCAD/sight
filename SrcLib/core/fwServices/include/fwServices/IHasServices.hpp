/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_IHASSERVICES_HPP__
#define __FWSERVICES_IHASSERVICES_HPP__

#include "fwServices/config.hpp"
#include <fwServices/op/Add.hpp>

#include <fwCore/macros.hpp>

#include <vector>

namespace fwServices
{
class IService;

/**
 * @brief   Interface for objects that need to manage services.
 */
class FWSERVICES_CLASS_API IHasServices
{
public:

    typedef std::vector < SPTR(::fwServices::IService) > ServiceVector;

    /**
     * @brief Get all subservices linked to this service
     * @return The vector of linked services
     */
    const ServiceVector& getRegisteredServices() const;

    /**
     * @brief Return a specific registered service
     * @param[in] id Identifier of the service
     */
    FWSERVICES_API CSPTR( ::fwServices::IService) getRegisteredService(const ::fwTools::fwID::IDType& _id) const;

protected:

    /// Constructor
    FWSERVICES_API IHasServices() noexcept;

    /// Destructor
    FWSERVICES_API virtual ~IHasServices() noexcept;

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    FWSERVICES_API SPTR(::fwServices::IService) registerService(const std::string& _implType,
                                                                const std::string& _id = "" );

    /**
     * @brief Register a new service linked to this service
     * @param[in] _implType Type of the service
     * @param[in] id Optional identifier of the service
     */
    template <class T>
    SPTR(T) registerService(const std::string& _implType, const std::string& _id = "" );

    /**
     * @brief Unregister a specific service
     * @param id Identifier of the service
     */
    FWSERVICES_API void unregisterService(const ::fwTools::fwID::IDType& _id);

    /**
     * @brief Unregister all services linked to this service, optionally matches only a given type of services
     * @param _implType Optional type of the services to unregister
     */
    FWSERVICES_API void unregisterServices(const std::string& _implType = "");

private:

    /// Sub services linked to this service
    ServiceVector m_subServices;
};

//------------------------------------------------------------------------------

inline const IHasServices::ServiceVector& IHasServices::getRegisteredServices() const
{
    return m_subServices;
}

//------------------------------------------------------------------------------

template <class T>
SPTR(T) IHasServices::registerService(const std::string& _implType, const std::string& _id)
{
    auto srv = ::fwServices::add<T>(_implType, _id);
    m_subServices.push_back(srv);

    return srv;
}

//------------------------------------------------------------------------------

} //namespace fwServices

#endif /*__FWSERVICES_IHASSERVICES_HPP__*/
