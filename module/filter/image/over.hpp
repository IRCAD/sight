/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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
 * @brief Composites ordered images, with zero-valued pixels treated as transparent.
 *
 * Each nonzero pixel replaces the accumulated value, so the last nonzero value in the input order wins. Images must
 * be 3D scalar images with matching type, size, pixel format, and physical geometry.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::over">
       <input image="${first_image}" />
       <input image="${second_image}" />
       <output image="${output_image}" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b input.image [sight::data::image]: Ordered images to composite. At least one image is required.
 *
 * @subsection Output Output
 * - \b output.image [sight::data::image]: Composite image using the first input's type and geometry.
 */
class over final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(over, sight::service::filter);

    over() noexcept;
    ~over() noexcept final = default;

protected:

    void starting() final;
    void stopping() final;
    void updating() final;

private:

    ptr_vector_in<data::image> m_images {this, "input.image"};
    ptr_inout<data::image> m_output {this, "output.image"};
};

} // namespace sight::module::filter::image
