/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <ui/__/service.hpp>

namespace sight::module::viz::sample
{

/**
 * @brief Render a 3D negatoscope of a medical image.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::viz::sample::image" >
      <in key="image" uid="..." auto_connect="true" />
   </service>
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: image to display.
 */
class MODULE_VIZ_SAMPLE_CLASS_API image : public sight::ui::service
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(image, sight::ui::service);

    /// Creates the service.
    MODULE_VIZ_SAMPLE_API image() noexcept;

    /// Destroys the service.
    MODULE_VIZ_SAMPLE_API ~image() noexcept override;

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
     * Connect data::image::MODIFIED_SIG to service::slots::UPDATE
     */
    MODULE_VIZ_SAMPLE_API connections_t auto_connections() const override;

    /// Does nothing.
    MODULE_VIZ_SAMPLE_API void updating() override;

    /// Stops and unregisters sub-services.
    MODULE_VIZ_SAMPLE_API void stopping() override;

private:

    /// Contains the render service.
    service::base::sptr m_render_srv {nullptr};

    /// Contains the trackball interactor adaptor.
    service::base::sptr m_interactor_srv {nullptr};

    /// Contains the negato adaptor.
    service::base::sptr m_negato_srv {nullptr};

    /// Default transfer function
    data::transfer_function::sptr m_tf;

    data::ptr<data::image, data::access::in> m_image {this, "image", false};
};

} // namespace sight::module::viz::sample.
