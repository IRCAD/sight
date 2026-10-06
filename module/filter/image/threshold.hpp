/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Keeps image values within an inclusive range, optionally converting them to a binary mask.
 *
 * The lower and upper bounds are inclusive. With `config.binary=true`, values in range become 255; otherwise their
 * original values are preserved. Values outside the range are set to 0 in either mode.
 * The input must be a 3D image with an integer pixel type supported by `sight::core::tools::integer_types`.
 * The output retains the input pixel type and image geometry.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::filter::image::threshold">
            <input image="${...}" />
            <output image="${...}" />
            <config lower="50" upper="255" />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b input.image [sight::data::image]: 3D integer image to threshold.
 *
 * @subsection Output Output
 * - \b output.image [sight::data::image]: Filtered image, with the same data type and geometry as the input.
 *
 * @subsection Configuration Configuration
 * - \b config.lower [sight::data::integer]: Inclusive lower bound. Defaults to 50.
 * - \b config.upper [sight::data::integer]: Inclusive upper bound. Defaults to 255.
 * - \b config.binary [sight::data::boolean]: If true, replace values in range with 255; otherwise preserve labels.
 *   Defaults to true.
 */
class threshold final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(threshold, sight::service::filter);

    threshold() noexcept;
    ~threshold() noexcept final = default;

protected:

    void starting() final;
    void stopping() final;

    /// Apply the threshold.
    void updating() final;

private:

    ptr_in<sight::data::image> m_source {this, "input.image"};
    ptr_inout<sight::data::image> m_target {this, "output.image"};
    ptr_in<sight::data::integer> m_lower_threshold {this, "config.lower", 50};
    ptr_in<sight::data::integer> m_upper_threshold {this, "config.upper", 255};
    ptr_in<sight::data::boolean> m_binary {this, "config.binary", true};
};

} // namespace sight::module::filter::image
