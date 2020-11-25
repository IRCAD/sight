/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "maths/STransformPickedPoint.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace maths
{

static const ::fwCom::Slots::SlotKeyType s_PICKED_POINT_SLOT              = "pickedPoint";
static const ::fwCom::Signals::SignalKeyType PICKED_POINT_TRANSFORMED_SIG = "pickedPointTransformed";
// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::maths::STransformPickedPoint )

// -----------------------------------------------------------------------------

STransformPickedPoint::STransformPickedPoint() noexcept
{
    FW_DEPRECATED("::maths::STransformPickedPoint", "::ctrlPicking::SManagePointList", "21.0");

    newSlot(s_PICKED_POINT_SLOT, &STransformPickedPoint::pickedPoint, this);
    newSignal<PickedPointTransformedSignalType>(PICKED_POINT_TRANSFORMED_SIG);
}

// -----------------------------------------------------------------------------

STransformPickedPoint::~STransformPickedPoint() noexcept
{
}

// -----------------------------------------------------------------------------

void STransformPickedPoint::starting()
{
}

// -----------------------------------------------------------------------------

void STransformPickedPoint::stopping()
{
}

// -----------------------------------------------------------------------------

void STransformPickedPoint::configuring()
{
}

// -----------------------------------------------------------------------------

void STransformPickedPoint::updating()
{

}

// -----------------------------------------------------------------------------
void STransformPickedPoint::pickedPoint(::fwDataTools::PickingInfo pickingInfo)
{
    const auto matrix = this->getLockedInput< ::fwData::TransformationMatrix3D >(
        "matrix");

    double* pickedPos = pickingInfo.m_worldPos;
    ::glm::dvec4 pickedPoint = ::glm::dvec4 {pickedPos[0], pickedPos[1], pickedPos[2], 1.0};
    ::glm::dmat4x4 mat       = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(matrix.get_shared());

    ::glm::dvec4 modifiedPoint = mat*pickedPoint;

    for(int i = 0; i < 3; ++i)
    {
        pickingInfo.m_worldPos[i] = modifiedPoint[i];
    }

    this->signal< PickedPointTransformedSignalType>(PICKED_POINT_TRANSFORMED_SIG)->asyncEmit(pickingInfo);

}

// -----------------------------------------------------------------------------

} // maths
