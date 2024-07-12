/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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
class mip_matching_registration : public service::registerer
{
public:

    SIGHT_DECLARE_SERVICE(mip_matching_registration, service::registerer);

    /**
     * @brief Constructor
     */
    mip_matching_registration() noexcept = default;
    /**
     * @brief Destructor
     */
    ~mip_matching_registration() noexcept override = default;

protected:

    /**
     * @brief Starts the service.
     *
     * @throw core::tools::failed
     */
    void starting() override;

    /**
     * @brief Stops the service.
     *
     * @throw core::tools::failed
     */
    void stopping() override;

    /**
     * @brief Configures the service.
     *
     * @throw core::tools::failed
     */
    void configuring() override;

    /**
     * @brief Compute the registration.
     *
     * @throw core::tools::failed
     */
    void updating() override;

    /**
     * @brief Compute the registration by calling the `update` slot. The timestamp parameter is discarded.
     */
    void compute_registration(core::clock::type _timestamp) override;

private:

    sight::data::ptr<sight::data::image, sight::data::access::in> m_source {this, "fixed"};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_target {this, "moving"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_transform {this, "transform"};
};

} // namespace sight::module::filter::image
