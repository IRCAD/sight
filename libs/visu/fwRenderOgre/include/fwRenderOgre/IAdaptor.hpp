/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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
class FWRENDEROGRE_CLASS_API IAdaptor :
    public ::fwServices::IService,
    public ::fwServices::IHasServices
{

friend class SRender;

public:

    /// Generates default methods.
    fwCoreClassMacro(IAdaptor, ::fwServices::IService)

    /// Sets the layer ID.
    FWRENDEROGRE_API void setLayerID(const std::string& _id);

    /// @returns the layer ID of this adaptor.
    FWRENDEROGRE_API const std::string& getLayerID() const;

    /// Sets the render service using this adaptor.
    FWRENDEROGRE_API void setRenderService(SRender::sptr _service);

    /// @returns the render service using this adaptor
    FWRENDEROGRE_API SRender::sptr getRenderService() const;

    /// @returns the layer where this adaptor is attached.
    FWRENDEROGRE_API Layer::sptr getLayer() const;

protected:

    /// Creates the adaptor.
    FWRENDEROGRE_API IAdaptor() noexcept;

    /// Destroys the adaptor.
    FWRENDEROGRE_API ~IAdaptor() noexcept override;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    FWRENDEROGRE_API void info(std::ostream& _sstream) override;

    /**
     * @brief Parses common adaptor parameters
     *
     * @code{.xml}
            <config layer="..." />
       @endcode
     *
     * @subsection Configuration Configuration:
     * - \b layer (mandatory, string): id of the layer where this adaptor applies.
     */
    FWRENDEROGRE_API void configureParams();

    /// Registers the adaptor into its render service.
    FWRENDEROGRE_API void initialize();

    /**
     * @brief Gets the Ogre SceneManager
     * @return Ogre default scene manager
     */
    FWRENDEROGRE_API ::Ogre::SceneManager* getSceneManager();

    /// Asks the render service to update rendering.
    FWRENDEROGRE_API virtual void requestRender();

    /// Defines the layer ID:
    ::std::string m_layerID;

    /// Contqins the t=render service which this adaptor is attached.
    ::fwRenderOgre::SRender::wptr m_renderService;

};

//------------------------------------------------------------------------------

} // namespace fwRenderOgre.
