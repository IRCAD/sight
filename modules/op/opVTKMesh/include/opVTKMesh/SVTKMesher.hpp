/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "opVTKMesh/config.hpp"

#include <fwServices/IOperator.hpp>

#include <string>

namespace opVTKMesh
{

/**
 * @brief This service generates meshes from an ImageSeries using VTK library.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::opVTKMesh::SVTKMesher" >
       <in key="imageSeries" uid="..."/>
       <out key="modelSeries" uid="..." />
       <config>
            <percentReduction>80</percentReduction>
        </config>
   </service>
    @endcode
 *
 * @subsection Input Input
 * - \b imageSeries [::fwMedData::ImageSeries] : image used to generate the output mesh
 *
 * @subsection Output Output
 * - \b modelSeries [::fwMedData::ModelSeries]: mesh generated from ImageSeries
 *
 * @subsection Configuration Configuration
 * - \b percentReduction : Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)
 */
class OPVTKMESH_CLASS_API SVTKMesher final : public ::fwServices::IOperator
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SVTKMesher, ::fwServices::IOperator)

    /// Registers input and output objects.
    OPVTKMESH_API SVTKMesher() noexcept;

    /// Destroys the service.
    OPVTKMESH_API ~SVTKMesher() noexcept override;

protected:

    /// Does nothing.
    OPVTKMESH_API void starting() override;

    /// Does nothing.
    OPVTKMESH_API void stopping() override;

    /// Configures the services.
    OPVTKMESH_API void configuring() override;

    /// Processes the mesh creation from the image series.
    OPVTKMESH_API void updating() override;

private:

    /// image size reduction value.
    unsigned int m_reduction;
};

} // namespace opVTKMesh.
