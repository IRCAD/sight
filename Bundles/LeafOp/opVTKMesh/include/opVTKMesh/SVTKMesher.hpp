/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPVTKMESH_SVTKMESHER_HPP__
#define __OPVTKMESH_SVTKMESHER_HPP__

#include <string>

#include "opVTKMesh/IMesher.hpp"
#include "opVTKMesh/config.hpp"

namespace opVTKMesh
{

/**
 * @brief This service generates meshes from an image using VTK library.
 *
 * This service works on a composite containing a ::fwData::Image and a ::fwMedData::ModelSeries.
 * The generated meshes are stored in the given ModelSeries.
 */
class OPVTKMESH_CLASS_API SVTKMesher : public ::opVTKMesh::IMesher
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SVTKMesher)(::opVTKMesh::IMesher) );

    OPVTKMESH_API SVTKMesher() throw();

    OPVTKMESH_API virtual ~SVTKMesher() throw();

protected:

    OPVTKMESH_API virtual void starting() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void stopping() throw ( ::fwTools::Failed );

    /**
     * @brief Declare the configuration to define percent of reduction, image source and ModelSeries destination.
     *
     * Example of this service configuration
     * @code{.xml}
       <service impl="::opVTKMesh::SVTKMesher" type="::opVTKMesh::IMesher">
           <config>
                <percentReduction>80</percentReduction>
                <image>IMAGE_KEY</image>
                <modelSeries>MODELSERIES_KEY</modelSeries>
            </config>
       </service>
        @endcode
     * - \b percentReduction: Specify the desired reduction in the total number of polygons (e.g., if
     *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)
     * - \b image: key of the source image into the composite
     * - \b modelSeries: key of the target ModelSeries into the composite
     */
    OPVTKMESH_API virtual void configuring() throw ( ::fwTools::Failed );

    OPVTKMESH_API virtual void updating() throw ( ::fwTools::Failed );

private:

    unsigned int m_reduction;
    std::string m_imageKey;
    std::string m_modelSeriesKey;

};

} // namespace opVTKMesh


#endif // __OPVTKMESH_SVTKMESHER_HPP__
