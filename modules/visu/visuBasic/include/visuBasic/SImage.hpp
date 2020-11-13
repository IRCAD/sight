/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "visuBasic/config.hpp"

#include <fwGui/IGuiContainerSrv.hpp>

namespace visuBasic

{
/**
 * @brief Render a 3D negatoscope of a medical image.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="::visuBasic::SImage" >
      <in key="image" uid="..." autoConnect="yes" />
   </service>
 *
 * @subsection Input Input
 * - \b image [::fwData::Image]: image to display.
 */
class VISUBASIC_CLASS_API SImage : public ::fwGui::IGuiContainerSrv
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SImage, ::fwGui::IGuiContainerSrv)

    /// Creates the service.
    VISUBASIC_API SImage() noexcept;

    /// Destroys the service.
    VISUBASIC_API virtual ~SImage() noexcept;

protected:

    /// Checks if the input image is autoconnect.
    VISUBASIC_API void configuring() override;

    /// Registers and starts sub-services.
    VISUBASIC_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     * @note This is actually useless since the sub-service already listens to the data,
     * but this prevents a warning in fwServices from being raised.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    VISUBASIC_API KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    VISUBASIC_API void updating() override;

    /// Stops and unregisters sub-services.
    VISUBASIC_API void stopping() override;

private:

    /// Contains the render service.
    ::fwServices::IService::sptr m_renderSrv { nullptr };

    /// Contains the trackball interactor adaptor.
    ::fwServices::IService::sptr m_interactorSrv { nullptr };

    /// Contains the negato adaptor.
    ::fwServices::IService::sptr m_negatoSrv { nullptr };

    /// Defines whether or not to autoConnect to the image, needed for the purpose of tutorials.
    bool m_imageAutoConnect { false };

};

} // namespace visuBasic.
