/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "videoTools/SImagePicker.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Camera.hpp>

#include <fwServices/macros.hpp>

namespace videoTools
{

fwServicesRegisterMacro( ::fwServices::IController, ::videoTools::SImagePicker, data::PointList)

//-----------------------------------------------------------------------------

const core::com::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

const ::fwServices::IService::KeyType s_POINTLIST_INOUT       = "pointList";
const ::fwServices::IService::KeyType s_PIXEL_POINTLIST_INOUT = "pixelPointList";
const ::fwServices::IService::KeyType s_CAMERA_INOUT          = "camera";

SImagePicker::SImagePicker() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &SImagePicker::getInteraction, this);

    m_videoRefMap["top_left"] = VideoReference::TOP_LEFT;
    m_videoRefMap["center"]   = VideoReference::CENTER;
}

//-----------------------------------------------------------------------------

SImagePicker::~SImagePicker() noexcept
{
}

//-----------------------------------------------------------------------------

void SImagePicker::starting()
{

}

//-----------------------------------------------------------------------------

void SImagePicker::stopping()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string videoRef = config.get< std::string >("videoReference", "center");

    auto it = m_videoRefMap.find(videoRef);

    if(it == m_videoRefMap.end())
    {
        SLM_WARN("'videoReference' of value '" + videoRef + "' is not handled.");
    }
    else
    {
        m_videoRef = it->second;
    }

}

//-----------------------------------------------------------------------------

void SImagePicker::updating()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::getInteraction(::fwDataTools::PickingInfo info)
{
    if (info.m_modifierMask == ::fwDataTools::PickingInfo::CTRL)
    {
        if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN)
        {
            const double x = info.m_worldPos[0];
            const double y = info.m_worldPos[1];
            const double z = info.m_worldPos[2];

            const std::array<double, 3> position = {{ x, y, z }};

            this->addPoint(position);
        }
        else if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_DOWN)
        {
            this->removeLastPoint();
        }

    }
}

//-----------------------------------------------------------------------------

void SImagePicker::addPoint(const std::array<double, 3>& currentPoint )
{
    // Set z to 0 as it is an image.

    auto pointList      = this->getLockedInOut< data::PointList >(s_POINTLIST_INOUT);
    auto pixelPointList = this->getLockedInOut< data::PointList >(s_PIXEL_POINTLIST_INOUT);

    data::Point::sptr point = data::Point::New(currentPoint[0], currentPoint[1], 0.);

    const auto camera = this->getLockedInput< data::Camera >(s_CAMERA_INOUT);

    data::Point::sptr pixel;

    switch (m_videoRef)
    {
        case VideoReference::CENTER:
        {
            const float offset[2] = { static_cast<float>(camera->getWidth()) / 2.f,
                                      static_cast<float>(camera->getHeight()) / 2.f };

            // Shift point to set reference at top_left corner & inverse y axis.
            pixel = data::Point::New(currentPoint[0] + offset[0],
                                     offset[1] - currentPoint[1], 0.);
        }
        break;
        case VideoReference::TOP_LEFT:
        {
            pixel = data::Point::New(currentPoint[0], currentPoint[1], 0.);
        }
        break;
        default:
            SLM_ERROR("Only video reference CENTER and TOP_LEFT are handled");
            break;
    }

    // "World" points.
    {
        pointList->getPoints().push_back(point);
        auto sig = pointList->signal< data::PointList::ModifiedSignalType >(
            data::PointList::s_MODIFIED_SIG);
        sig->asyncEmit();

        auto sig2 = pointList->signal< data::PointList::PointAddedSignalType >(
            data::PointList::s_POINT_ADDED_SIG);
        sig2->asyncEmit(point);
    }

    // Pixel points.
    {
        pixelPointList->getPoints().push_back(pixel);
        auto sig = pixelPointList->signal< data::PointList::ModifiedSignalType >(
            data::PointList::s_MODIFIED_SIG);
        sig->asyncEmit();

        auto sig2 = pixelPointList->signal< data::PointList::PointAddedSignalType >(
            data::PointList::s_POINT_ADDED_SIG);
        sig2->asyncEmit(point);
    }
}

//-----------------------------------------------------------------------------

void SImagePicker::removeLastPoint()
{
    auto pointList      = this->getLockedInOut< data::PointList >(s_POINTLIST_INOUT);
    auto pixelPointList = this->getLockedInOut< data::PointList >(s_PIXEL_POINTLIST_INOUT);
    data::Point::sptr point;

    if (!pointList->getPoints().empty() && !pixelPointList->getPoints().empty())
    {
        {
            point = pointList->getPoints().back();
            pointList->getPoints().pop_back();

            auto sig = pointList->signal< data::PointList::ModifiedSignalType >(
                data::PointList::s_MODIFIED_SIG);
            sig->asyncEmit();

            auto sig2 = pointList->signal< data::PointList::PointRemovedSignalType >(
                data::PointList::s_POINT_REMOVED_SIG);
            sig2->asyncEmit(point);
        }

        {
            point = pixelPointList->getPoints().back();
            pixelPointList->getPoints().pop_back();

            auto sig = pixelPointList->signal< data::PointList::ModifiedSignalType >(
                data::PointList::s_MODIFIED_SIG);
            sig->asyncEmit();

            auto sig2 = pixelPointList->signal< data::PointList::PointRemovedSignalType >(
                data::PointList::s_POINT_REMOVED_SIG);
            sig2->asyncEmit(point);
        }
    }

}

//-----------------------------------------------------------------------------

} // namespace videoTools
