/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "service/generator.hpp"

#include <data/camera.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <service/controller.hpp>

#include <opencv2/core.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief Service for creating a 3D chessboard model.
 *
 * This class generates a 3D model of a chessboard based on configurable width, height,
 * and square size parameters. The generated model is output as a list of 3D points.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::geometry::vision::chessboard_creator">
        <out key="chessboard_model" uid="..." />
        <properties board_width="..." board_height="..." board_square_size="..." />
    </service>
 * @endcode

 * @subsection Output Output
 * - \b chessboard_model [sight::data::point_list] (mandatory): 3D chessboard model as described by the `board`
 *
 * @subsection Properties Properties
 * - \b board_width: Width of the chessboard. Default is 11.
 * - \b board_height: Height of the chessboard. Default is 8.
 * - \b board_square_size: Size of each square on the chessboard. Default is 1.0.
 */

class chessboard_creator final : public service::generator
{
public:

    SIGHT_DECLARE_SERVICE(chessboard_creator, service::generator);

    ///Constructor
    chessboard_creator() = default;

    ///Destructor
    ~chessboard_creator() final = default;

protected:

    /// Does nothing
    void configuring() final;

    /// Calls update
    void starting() final;

    /// Creates the 3d model of the chessboard.
    void updating() final;

    /// Resets the chessboard model.
    void stopping() final;

    /// Returns proposals to update the service when the input image is modified.
    connections_t auto_connections() const final;

private:

    /// Output chessboard model (3d points).
    data::ptr<data::point_list, data::access::inout> m_chessboard_model {this, "chessboard_model"};

    /// Width of the chessboard.
    sight::data::property<sight::data::integer> m_width {this, "board_width", 11};

    /// Height of the chessboard.
    sight::data::property<sight::data::integer> m_height {this, "board_height", 8};

    /// Square size of the chessboard.
    sight::data::property<sight::data::real> m_square_size {this, "board_square_size", 1.};
};

} //namespace sight::module::geometry::vision
