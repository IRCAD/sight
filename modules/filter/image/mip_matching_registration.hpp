/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <service/registerer.hpp>

namespace sight::module::filter::image
{

/**
 * @brief This service uses a fast registration algorithm to quickly register two images together if the transform
 * between both is pure translation.
 *
 * @section Signals Signals
 *
 * @section Slots Slots
 *   - \b update: compute the registration
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::filter::image::SFastRegistration">
       <in key="source" uid="..." />
       <in key="target" uid="..." />
       <in key="transform" uid="..." />
   </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b source [sight::data::image]: Input image that will be aligned to the target.
 * - \b target [sight::data::image]: Input image to which the source will be aligned.
 *
 * @subsection In-Out In-Out
 * - \b transform [sight::data::matrix4]: initial registration. The computed registration will be
 *   concatenated to the initial registration matrix.
 */
class MODULE_FILTER_IMAGE_CLASS_API mip_matching_registration : public service::registerer
{
public:

    SIGHT_DECLARE_SERVICE(mip_matching_registration, service::registerer);

    /**
     * @brief Constructor
     */
    MODULE_FILTER_IMAGE_API mip_matching_registration() noexcept;
    /**
     * @brief Destructor
     */
    MODULE_FILTER_IMAGE_API ~mip_matching_registration() noexcept override;

protected:

    /**
     * @brief Starts the service.
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_IMAGE_API void starting() override;

    /**
     * @brief Stops the service.
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_IMAGE_API void stopping() override;

    /**
     * @brief Configures the service.
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_IMAGE_API void configuring() override;

    /**
     * @brief Compute the registration.
     *
     * @throw core::tools::failed
     */
    MODULE_FILTER_IMAGE_API void updating() override;

    /**
     * @brief Compute the registration by calling the `update` slot. The timestamp parameter is discarded.
     */
    MODULE_FILTER_IMAGE_API void computeRegistration(core::hires_clock::type timestamp) override;

private:

    sight::data::ptr<sight::data::image, sight::data::Access::in> m_source {this, "fixed"};
    sight::data::ptr<sight::data::image, sight::data::Access::in> m_target {this, "moving"};
    sight::data::ptr<sight::data::matrix4, sight::data::Access::inout> m_transform {this, "transform"};
};

} // namespace sight::module::filter::image
