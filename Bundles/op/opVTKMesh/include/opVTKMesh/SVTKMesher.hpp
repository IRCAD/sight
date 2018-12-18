/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
 * @subsection Output Output
 * - \b modelSeries [::fwMedData::ModelSeries]: mesh generated from ImageSeries
 * @subsection Configuration Configuration
 * - \b percentReduction : Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)
 */
class OPVTKMESH_CLASS_API SVTKMesher : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVTKMesher)(::fwServices::IOperator) );

    OPVTKMESH_API SVTKMesher() noexcept;

    OPVTKMESH_API virtual ~SVTKMesher() noexcept;

protected:

    OPVTKMESH_API virtual void starting() override;

    OPVTKMESH_API virtual void stopping() override;

    OPVTKMESH_API virtual void configuring() override;

    OPVTKMESH_API virtual void updating() override;

private:

    unsigned int m_reduction;
};

} // namespace opVTKMesh
