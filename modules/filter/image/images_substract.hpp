/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Compute the substraction of two images.

 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::filter::image::images_substract">
       <in key="image1" uid="..." />
       <in key="image2" uid="..." />
       <inout key="result" uid="..." />
   </service>
   @endcode
 * @subsection In In
 * - \b image1 [sight::data::image]: first image.
 * - \b image2 [sight::data::image]: second image.
 * @subsection InOut InOut
 * - \b result [sight::data::image]: substract image.
 */

class images_substract final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(images_substract, sight::service::filter);

    images_substract()           = default;
    ~images_substract() override = default;

protected:

    void configuring() override;

    /// Overrides
    void starting() override;

    /// Overrides
    void stopping() override;

    /// Overrides
    void updating() override;

private:

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image1 {this, "image1"};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_image2 {this, "image2"};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_result {this, "result"};
};

} // namespace sight::module::filter::image
