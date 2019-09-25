/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/PickingInfo.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <glm/mat4x4.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief Computes a world transform matrix either from a viewport coordinate and a rotation angle or
 *        from successive picking positions.
 *
 * @section Slots Slots
 *
 * - \b rotateTransform(double, double, double): applies a rotation to the current
 * transform using wheel information.
 * - \b translateTransform(::fwDataTools::PickingInfo): applies a translation using the last two
 * picking informations.
 * - \b updateSliceOrientation(int, int): updates slice orientation.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::STransformFromWheel" >
       <inout key="transform" uid="..." />
       <config renderer="default" picker="myPicker" mode="2d" orientation="axial"/>
   </service>
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: outputed world space transform.
 *
 * @subsection Configuration Configuration
 * - \b renderer (mandatory): renderer in which the image is displayed.
 * - \b picker (mandatory): used to find image voxel positions from a viewport pixel position.
 * - \b mode (optional, defaults to 2d) : determines how the position of the coordinates are computed.
 *           In 2d, the coordinates are permutated depending on the orientation, whereas in 3d,
 *           we do not change the picked position.
 * - \b orientation (mandatory, values=axial|sagital|frontal): rotation axis direction.
 *
 */
class VISUVTKADAPTOR_CLASS_API STransformFromWheel : public ::fwRenderVTK::IAdaptor
{
public:

    fwCoreServiceMacro(STransformFromWheel, ::fwRenderVTK::IAdaptor);

    /// Constructor.
    VISUVTKADAPTOR_API STransformFromWheel();

    /// Destructor.
    VISUVTKADAPTOR_API virtual ~STransformFromWheel();

protected:

    VISUVTKADAPTOR_API virtual void configuring() override;
    VISUVTKADAPTOR_API virtual void starting() override;
    VISUVTKADAPTOR_API virtual void updating() override;
    VISUVTKADAPTOR_API virtual void stopping() override;

private:

    /// Slot: Computes the transform based on the (cx, cy) wheel center and the wheel angle.
    void rotateTransform(double cx, double cy, double wheelAngle);

    /// Slot: Computes a translation using the difference between the last two picking informations.
    void translateTransform(::fwDataTools::PickingInfo info);

    /// Applies the parameter transform to the 'transform' object.
    void applyTransformToOutput(const ::glm::dmat4& transform) const;

    /// Slot: set the correct orientation to compute the right transform.
    void updateSliceOrientation(int from, int to);

    /// Rotation axis direction.
    ::fwDataTools::helper::MedicalImage::Orientation m_orientation;

    /// Interaction mode: determines how the picked point is handled (default to 2d)
    unsigned char m_interactionMode;

    /// Initial wheel position. Updated each time updateTransform() is called.
    double m_initAngle;

    /// Stores the last picking position, used to compute translations.
    double m_lastPickedPos[3];

    /// Flag set when in translation mode.
    bool m_translate;
};

} // namespace visuVTKAdaptor
