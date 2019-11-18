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

#include "opPicking/SManagePoint.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/helper/PointList.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace opPicking
{

static const ::fwCom::Slots::SlotKeyType s_PICK_SLOT         = "pick";
static const ::fwCom::Slots::SlotKeyType s_CLEAR_POINTS_SLOT = "clearPoints";

static const std::string s_MATRIX_INPUT = "matrix";

static const std::string s_POINTLIST_INOUT = "pointList";

static const std::string s_MAX_CONFIG       = "max";
static const std::string s_REMOVABLE_CONFIG = "removable";
static const std::string s_LABEL_CONFIG     = "label";

fwServicesRegisterMacro( ::fwServices::IController, ::opPicking::SManagePoint, ::fwData::PointList)

//------------------------------------------------------------------------------

SManagePoint::SManagePoint() noexcept
{
    newSlot(s_PICK_SLOT, &SManagePoint::pick, this );
    newSlot(s_CLEAR_POINTS_SLOT, &SManagePoint::clearPoints, this );
}

//------------------------------------------------------------------------------

SManagePoint::~SManagePoint() noexcept
{
}

//------------------------------------------------------------------------------

void SManagePoint::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_max       = config.get<size_t>(s_MAX_CONFIG, m_max);
    m_removable = config.get<bool>(s_REMOVABLE_CONFIG, m_removable);
    m_label     = config.get<bool>(s_LABEL_CONFIG, m_label);
}

//------------------------------------------------------------------------------

void SManagePoint::starting()
{
}

//------------------------------------------------------------------------------

void SManagePoint::updating()
{
}

//------------------------------------------------------------------------------

void SManagePoint::stopping()
{
}

//------------------------------------------------------------------------------

void SManagePoint::pick(::fwDataTools::PickingInfo _info) const
{
    if(_info.m_modifierMask & ::fwDataTools::PickingInfo::CTRL)
    {
        const ::fwData::Point::sptr point = ::fwData::Point::New();

        const ::fwData::TransformationMatrix3D::csptr matrix
            = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INPUT);

        if(matrix)
        {
            const ::fwData::mt::ObjectReadLock lock(matrix);

            const double* const pickedCoord = _info.m_worldPos;
            const ::glm::dvec4 pickedPoint  = ::glm::dvec4 {pickedCoord[0], pickedCoord[1], pickedCoord[2], 1.0};
            const ::glm::dmat4x4 mat        = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(matrix);

            const ::glm::dvec4 modifiedPickedPoint = mat*pickedPoint;
            point->setCoord({modifiedPickedPoint[0], modifiedPickedPoint[1], modifiedPickedPoint[2]});
        }
        else
        {
            point->setCoord({_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]});
        }

        if(_info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP)
        {
            this->addPoint(point);
        }
        else if(_info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_UP)
        {
            this->removePoint(point);
        }
    }
}

//------------------------------------------------------------------------------

void SManagePoint::addPoint(const ::fwData::Point::sptr _point) const
{
    const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);
    const ::fwData::mt::ObjectWriteLock lock(pointList);

    if(m_label)
    {
        const auto counter                 = pointList->getPoints().size();
        const ::fwData::String::sptr label = ::fwData::String::New(std::to_string(counter));
        _point->setField(::fwDataTools::fieldHelper::Image::m_labelId, label );
    }

    pointList->pushBack(_point);
    const auto& sigAdded = pointList->signal< ::fwData::PointList::PointAddedSignalType >(
        ::fwData::PointList::s_POINT_ADDED_SIG);
    sigAdded->asyncEmit(_point);

    if(m_max != 0 && pointList->getPoints().size() > m_max)
    {
        const ::fwData::Point::sptr removedPoint = pointList->getPoints().front();
        pointList->remove(0);
        const auto& sigRemoved = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
            ::fwData::PointList::s_POINT_REMOVED_SIG);
        sigRemoved->asyncEmit(removedPoint);
    }
}

//------------------------------------------------------------------------------

void SManagePoint::removePoint(const ::fwData::Point::csptr _point) const
{
    if(m_removable)
    {
        const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
        SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);
        const ::fwData::mt::ObjectWriteLock lock(pointList);

        const ::fwData::Point::sptr pointRes =
            ::fwDataTools::helper::PointList::removeClosestPoint(pointList, _point, 10);

        if(pointRes != nullptr)
        {
            const auto& sigRemoved = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
                ::fwData::PointList::s_POINT_REMOVED_SIG);
            sigRemoved->asyncEmit(pointRes);
        }
    }
}

//------------------------------------------------------------------------------

void SManagePoint::clearPoints() const
{
    const auto pointList = this->getInOut< ::fwData::PointList >(s_POINTLIST_INOUT);
    SLM_ASSERT("'" + s_POINTLIST_INOUT + "' does not exist.", pointList);
    const ::fwData::mt::ObjectWriteLock lock(pointList);

    using PLContainer = ::fwData::PointList::PointListContainer;
    const PLContainer container = pointList->getPoints();
    pointList->clear();

    for(PLContainer::size_type i = 0; i < container.size(); ++i)
    {
        const ::fwData::Point::sptr point = container[i];

        const auto& sigRemoved = pointList->signal< ::fwData::PointList::PointRemovedSignalType >(
            ::fwData::PointList::s_POINT_REMOVED_SIG);
        sigRemoved->asyncEmit(point);
    }
}

} // namespace opPicking
