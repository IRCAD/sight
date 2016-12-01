/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPVTKMESH_ACTION_SMESHCREATION_HPP__
#define __OPVTKMESH_ACTION_SMESHCREATION_HPP__

#include "opVTKMesh/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace opVTKMesh
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
        <service type="::opVTKMesh::action::SMeshCreation">
            <in key="image" uid="..." />
            <inout key="mesh" uid="..." />
            <percentReduction value="0" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: source image.
 * @subsection In-Out In-Out:
 * - \b mesh [::fwData::Mesh]: target mesh.
 * @subsection Configuration Configuration:
 * - \b percentReduction: Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)..
 */
class OPVTKMESH_CLASS_API SMeshCreation : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SMeshCreation)(::fwGui::IActionSrv) );

    OPVTKMESH_API SMeshCreation() throw();

    OPVTKMESH_API virtual ~SMeshCreation() throw();

protected:

    OPVTKMESH_API void starting() throw ( ::fwTools::Failed );

    OPVTKMESH_API void stopping() throw ( ::fwTools::Failed );

    /// Configure the service.
    OPVTKMESH_API void configuring() throw ( ::fwTools::Failed );

    /// Process the mesh creation from the image.
    OPVTKMESH_API void updating() throw ( ::fwTools::Failed );

private:

    unsigned int m_reduction;
};


} // namespace action
} // namespace opVTKMesh


#endif // __OPVTKMESH_ACTION_SMESHCREATION_HPP__
