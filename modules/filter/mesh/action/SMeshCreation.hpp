/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/filter/mesh/config.hpp"

#include <ui/base/IActionSrv.hpp>

namespace sight::modules::filter::mesh
{

namespace action
{

/**
 * @brief   Action to create a mesh from an image using VTK library
 *
 * @section Signals Signals
 * - \b sent(int): Emitted when .
 *
 * @section Slots Slots
 * - \b receive(int): .

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::sight::modules::filter::mesh::action::SMeshCreation">
            <in key="image" uid="..." />
            <inout key="mesh" uid="..." />
            <percentReduction value="0" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b image [sight::data::Image]: source image.
 * @subsection In-Out In-Out:
 * - \b mesh [sight::data::Mesh]: target mesh.
 * @subsection Configuration Configuration:
 * - \b percentReduction: Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)..
 */
class MODULE_FILTER_MESH_CLASS_API SMeshCreation : public ::sight::ui::base::IActionSrv
{

public:

    fwCoreServiceMacro(SMeshCreation, ::sight::ui::base::IActionSrv)

    MODULE_FILTER_MESH_API SMeshCreation() noexcept;

    MODULE_FILTER_MESH_API virtual ~SMeshCreation() noexcept;

protected:

    MODULE_FILTER_MESH_API void starting() override;

    MODULE_FILTER_MESH_API void stopping() override;

    /// Configure the service.
    MODULE_FILTER_MESH_API void configuring() override;

    /// Process the mesh creation from the image.
    MODULE_FILTER_MESH_API void updating() override;

private:

    unsigned int m_reduction;
};

} // namespace action
} // namespace sight::modules::filter::mesh
