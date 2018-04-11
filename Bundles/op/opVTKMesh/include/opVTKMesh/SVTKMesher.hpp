/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "opVTKMesh/config.hpp"
#include "opVTKMesh/IMesher.hpp"

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
class OPVTKMESH_CLASS_API SVTKMesher : public ::opVTKMesh::IMesher
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVTKMesher)(::opVTKMesh::IMesher) );

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
