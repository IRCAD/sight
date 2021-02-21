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

#include "modules/viz/sample/config.hpp"

#include <ui/base/IGuiContainer.hpp>

namespace sight::module::viz::sample

{
/**
 * @brief Render a 3D negatoscope of a medical image.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="::sight::module::viz::sample::SImage" >
      <in key="image" uid="..." autoConnect="yes" />
   </service>
 *
 * @subsection Input Input
 * - \b image [sight::data::Image]: image to display.
 */
class MODULE_VIZ_SAMPLE_CLASS_API SImage : public sight::ui::base::IGuiContainer
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SImage, sight::ui::base::IGuiContainer)

    /// Creates the service.
    MODULE_VIZ_SAMPLE_API SImage() noexcept;

    /// Destroys the service.
    MODULE_VIZ_SAMPLE_API virtual ~SImage() noexcept;

protected:

    /// Checks if the input image is autoconnect.
    MODULE_VIZ_SAMPLE_API void configuring() override;

    /// Registers and starts sub-services.
    MODULE_VIZ_SAMPLE_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     * @note This is actually useless since the sub-service already listens to the data,
     * but this prevents a warning in fwServices from being raised.
     *
     * Connect data::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    MODULE_VIZ_SAMPLE_API KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_VIZ_SAMPLE_API void updating() override;

    /// Stops and unregisters sub-services.
    MODULE_VIZ_SAMPLE_API void stopping() override;

private:

    /// Contains the render service.
    service::IService::sptr m_renderSrv { nullptr };

    /// Contains the trackball interactor adaptor.
    service::IService::sptr m_interactorSrv { nullptr };

    /// Contains the negato adaptor.
    service::IService::sptr m_negatoSrv { nullptr };

    /// Defines whether or not to autoConnect to the image, needed for the purpose of tutorials.
    bool m_imageAutoConnect { false };

};

} // namespace sight::module::viz::sample.
