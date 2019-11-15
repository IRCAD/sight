/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "opPicking/STransformPickedPoint.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

namespace opPicking
{

static const ::fwCom::Slots::SlotKeyType s_PICKED_POINT_SLOT              = "pickedPoint";
static const ::fwCom::Signals::SignalKeyType PICKED_POINT_TRANSFORMED_SIG = "pickedPointTransformed";

static const std::string s_MATRIX_INPUT = "matrix";

// -----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IService, ::opPicking::STransformPickedPoint )

// -----------------------------------------------------------------------------

STransformPickedPoint::STransformPickedPoint()
{
    newSlot(s_PICKED_POINT_SLOT, &STransformPickedPoint::pickedPoint, this);
    newSignal<PickedPointTransformedSignalType>(PICKED_POINT_TRANSFORMED_SIG);
}

// -----------------------------------------------------------------------------

STransformPickedPoint::~STransformPickedPoint()
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
    const ::fwData::TransformationMatrix3D::csptr matrix
        = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INPUT);
    SLM_ASSERT("'" + s_MATRIX_INPUT + "' does not exist.", matrix);

    const ::fwData::mt::ObjectReadLock lock(matrix);

    const double* const pickedPos  = pickingInfo.m_worldPos;
    const ::glm::dvec4 pickedPoint = ::glm::dvec4 {pickedPos[0], pickedPos[1], pickedPos[2], 1.0};
    const ::glm::dmat4x4 mat       = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(matrix);

    const ::glm::dvec4 modifiedPoint = mat*pickedPoint;

    for(uint8_t i = 0; i < 3; ++i)
    {
        pickingInfo.m_worldPos[i] = modifiedPoint[i];
    }

    this->signal< PickedPointTransformedSignalType>(PICKED_POINT_TRANSFORMED_SIG)->asyncEmit(pickingInfo);
}

// -----------------------------------------------------------------------------

} // opPicking
