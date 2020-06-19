/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwServices/IHasServices.hpp>
#include <fwServices/IService.hpp>

#include <OGRE/OgreSceneManager.h>

#include <string>

namespace fwRenderOgre
{

/**
 * @brief Interface providing behavior of Ogre adaptor services
 */
class FWRENDEROGRE_CLASS_API IAdaptor : public ::fwServices::IService,
                                        public ::fwServices::IHasServices
{
friend class SRender;
public:
    fwCoreClassMacro(IAdaptor, ::fwServices::IService);

    /// Set the layer ID
    FWRENDEROGRE_API void setLayerID(const std::string& id);

    FWRENDEROGRE_API const std::string& getLayerID() const;

    /// Set the render service using this adaptor
    FWRENDEROGRE_API void setRenderService( SRender::sptr service );

    /// Get the render service using this adaptor
    FWRENDEROGRE_API SRender::sptr getRenderService() const;

    FWRENDEROGRE_API Layer::sptr getLayer() const;

protected:

    /// Constructor
    FWRENDEROGRE_API IAdaptor() noexcept;

    /// Destructor
    FWRENDEROGRE_API virtual ~IAdaptor() noexcept;

    //@{
    /// Overrides
    FWRENDEROGRE_API virtual void info(std::ostream& _sstream ) override;
    //@}

    /**
     * @brief Parse common adaptor parameters:
     * @code{.xml}
            <config layer="..." />
       @endcode
     * @subsection Configuration Configuration:
     * - \b layer (mandatory): id of the layer where this adaptor applies.
     */
    FWRENDEROGRE_API void configureParams();

    /// Register the adaptor into its SRender service
    FWRENDEROGRE_API void initialize();

    /**
     * @brief Get the Ogre SceneManager
     * @return Ogre default scene manager
     */
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager();

    /// Ask the render service (SRender) to update
    FWRENDEROGRE_API virtual void requestRender();

    std::string msgHead() const;

    /// Layer ID
    ::std::string m_layerID;

    /// Render service which this adaptor is attached
    ::fwRenderOgre::SRender::wptr m_renderService;
};

// ------------------------------------------------------------------------

inline std::string IAdaptor::msgHead() const
{
    return "[ Adaptor '" + this->getID() + "'] ";
}
//------------------------------------------------------------------------------

} //namespace fwRenderOgre
