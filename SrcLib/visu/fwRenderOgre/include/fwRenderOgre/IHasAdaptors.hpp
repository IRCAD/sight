/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_IHASADAPTORS_HPP__
#define __FWRENDEROGRE_IHASADAPTORS_HPP__

#include "fwRenderOgre/config.hpp"

#include <fwCore/macros.hpp>

#include <fwTools/fwID.hpp>

#include <vector>

namespace fwServices
{
class IService;
}

namespace fwRenderOgre
{
class IAdaptor;

/**
 * @brief   Interface for objects that need to manage services.
 */
class FWRENDEROGRE_CLASS_API IHasAdaptors
{
public:

    typedef std::vector < WPTR(::fwRenderOgre::IAdaptor) > AdaptorVector;

    /**
     * @brief Get all subservices linked to this adaptor
     * @return The vector of linked services
     */
    const AdaptorVector& getRegisteredAdaptors() const;

    /**
     * @brief Return a specific registered service
     * @param id Identifier of the service
     */
    FWRENDEROGRE_API CSPTR( ::fwServices::IService) getRegisteredService(::fwTools::fwID::IDType _id) const;

protected:

    /// Constructor
    FWRENDEROGRE_API IHasAdaptors() noexcept;

    /// Destructor
    FWRENDEROGRE_API virtual ~IHasAdaptors() noexcept;

    /**
     * @brief Register a new service linked to this adaptor
     */
    FWRENDEROGRE_API void registerService( SPTR(::fwRenderOgre::IAdaptor) _service );

    /**
     * @brief Unregister a specific service
     * @param id Identifier of the service
     */
    FWRENDEROGRE_API void unregisterService(::fwTools::fwID::IDType _id);

    /**
     * @brief Unregister all services linked to this adaptor
     * @param classname Classname of services to unregister
     */
    FWRENDEROGRE_API void unregisterServices(std::string _classname = "");

    /// Sub adaptors linked to this adaptor
    AdaptorVector m_subAdaptors;
};

//------------------------------------------------------------------------------

inline const IHasAdaptors::AdaptorVector& IHasAdaptors::getRegisteredAdaptors() const
{
    return m_subAdaptors;
}

//------------------------------------------------------------------------------

} //namespace fwRenderOgre

#endif /*__FWRENDEROGRE_IHASADAPTORS_HPP__*/
