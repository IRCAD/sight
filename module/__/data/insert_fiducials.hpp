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

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service inserts fiducials into an image series.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::data::insert_fiducials" >
           <in key="fiducials" uid="..." />
           <inout key="image" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b fiducials [sight::data::fiducials_series]: fiducials to insert into the image.
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image_series]: target image series to update with fiducials.
 */
class insert_fiducials final : public service::controller
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(insert_fiducials, service::controller);

    /// Creates the service.
    insert_fiducials() = default;

    /// Destroys the service.
    ~insert_fiducials() final = default;

protected:

    /// Does nothing.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /// Inserts fiducials into the image series.
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    sight::data::ptr<sight::data::fiducials_series, sight::data::access::in> m_fiducials {this, "fiducials"};
    sight::data::ptr<sight::data::image_series, sight::data::access::inout> m_image {this, "image"};
};

} // namespace sight::module::data.
