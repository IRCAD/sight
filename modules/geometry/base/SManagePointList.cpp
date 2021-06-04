/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "SManagePointList.hpp"

#include <core/com/Connection.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/fieldHelper/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

#include <geometry/data/Matrix4.hpp>
#include <geometry/data/PointList.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry::base
{

static const core::com::Slots::SlotKeyType s_PICK_SLOT         = "pick";
static const core::com::Slots::SlotKeyType s_CLEAR_POINTS_SLOT = "clearPoints";

static const std::string s_MATRIX_INPUT = "matrix";

static const std::string s_POINTLIST_INOUT = "pointList";

static const std::string s_MAX_CONFIG       = "max";
static const std::string s_REMOVABLE_CONFIG = "removable";
static const std::string s_LABEL_CONFIG     = "label";
static const std::string s_TOLERANCE_CONFIG = "tolerance";

//------------------------------------------------------------------------------

SManagePointList::SManagePointList() noexcept
{
    newSlot(s_PICK_SLOT, &SManagePointList::pick, this);
    newSlot(s_CLEAR_POINTS_SLOT, &SManagePointList::clearPoints, this);
}

//------------------------------------------------------------------------------

SManagePointList::~SManagePointList() noexcept
{
}

//------------------------------------------------------------------------------

void SManagePointList::configuring()
{
    const auto tree   = this->getConfigTree();
    const auto config = tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_max       = config->get<size_t>(s_MAX_CONFIG, m_max);
        m_removable = config->get<bool>(s_REMOVABLE_CONFIG, m_removable);
        m_label     = config->get<bool>(s_LABEL_CONFIG, m_label);
        m_tolerance = config->get<float>(s_TOLERANCE_CONFIG, m_tolerance);
    }
}

//------------------------------------------------------------------------------

void SManagePointList::starting()
{
}

//------------------------------------------------------------------------------

void SManagePointList::updating()
{
}

//------------------------------------------------------------------------------

void SManagePointList::stopping()
{
}

//------------------------------------------------------------------------------

void SManagePointList::pick(data::tools::PickingInfo _info) const
{
    if(_info.m_modifierMask & data::tools::PickingInfo::CTRL)
    {
        const data::Point::sptr point = data::Point::New();

        const auto matrixW = this->getWeakInput<data::Matrix4>(s_MATRIX_INPUT);
        const auto matrix  = matrixW.lock();

        if(matrix)
        {
            const double* const pickedCoord = _info.m_worldPos;
            const ::glm::dvec4 pickedPoint  = ::glm::dvec4 {pickedCoord[0], pickedCoord[1], pickedCoord[2], 1.0};
            const ::glm::dmat4x4 mat        = sight::geometry::data::getMatrixFromTF3D(matrix.get_shared());

            const ::glm::dvec4 modifiedPickedPoint = mat * pickedPoint;
            point->setCoord({modifiedPickedPoint[0], modifiedPickedPoint[1], modifiedPickedPoint[2]});
        }
        else
        {
            point->setCoord({_info.m_worldPos[0], _info.m_worldPos[1], _info.m_worldPos[2]});
        }

        if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_LEFT_UP)
        {
            this->addPoint(point);
        }
        else if(_info.m_eventId == data::tools::PickingInfo::Event::MOUSE_RIGHT_UP)
        {
            this->removePoint(point);
        }
    }
}

//------------------------------------------------------------------------------

void SManagePointList::addPoint(const data::Point::sptr _point) const
{
    const auto pointList = this->getLockedInOut<data::PointList>(s_POINTLIST_INOUT);

    if(m_label)
    {
        const auto counter             = pointList->getPoints().size();
        const data::String::sptr label = data::String::New(std::to_string(counter));
        _point->setField(data::fieldHelper::Image::m_labelId, label);
    }

    pointList->pushBack(_point);
    const auto& sigAdded = pointList->signal<data::PointList::PointAddedSignalType>(
        data::PointList::s_POINT_ADDED_SIG
    );
    sigAdded->asyncEmit(_point);

    if(m_max != 0 && pointList->getPoints().size() > m_max)
    {
        const data::Point::sptr removedPoint = pointList->getPoints().front();
        pointList->remove(0);
        const auto& sigRemoved = pointList->signal<data::PointList::PointRemovedSignalType>(
            data::PointList::s_POINT_REMOVED_SIG
        );
        sigRemoved->asyncEmit(removedPoint);
    }
}

//------------------------------------------------------------------------------

void SManagePointList::removePoint(const data::Point::csptr _point) const
{
    if(m_removable)
    {
        const auto pointList             = this->getLockedInOut<data::PointList>(s_POINTLIST_INOUT);
        const data::Point::sptr pointRes =
            sight::geometry::data::PointList::removeClosestPoint(pointList.get_shared(), _point, m_tolerance);

        if(pointRes != nullptr)
        {
            const auto& sigRemoved = pointList->signal<data::PointList::PointRemovedSignalType>(
                data::PointList::s_POINT_REMOVED_SIG
            );
            sigRemoved->asyncEmit(pointRes);
        }
    }
}

//------------------------------------------------------------------------------

void SManagePointList::clearPoints() const
{
    const auto pointList = this->getLockedInOut<data::PointList>(s_POINTLIST_INOUT);

    using PLContainer = data::PointList::PointListContainer;
    const PLContainer container = pointList->getPoints();
    pointList->clear();

    for(PLContainer::size_type i = 0 ; i < container.size() ; ++i)
    {
        const data::Point::sptr point = container[i];

        const auto& sigRemoved = pointList->signal<data::PointList::PointRemovedSignalType>(
            data::PointList::s_POINT_REMOVED_SIG
        );
        sigRemoved->asyncEmit(point);
    }
}

} // namespace sight::module::geometry::base
