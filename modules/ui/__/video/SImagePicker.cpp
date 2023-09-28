/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "SImagePicker.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

namespace sight::module::ui::video
{

//-----------------------------------------------------------------------------

const core::com::slots::key_t GET_INTERACTION_SLOT = "getInteraction";

SImagePicker::SImagePicker() noexcept
{
    new_slot(GET_INTERACTION_SLOT, &SImagePicker::getInteraction, this);

    m_videoRefMap["top_left"] = VideoReference::TOP_LEFT;
    m_videoRefMap["center"]   = VideoReference::CENTER;
}

//-----------------------------------------------------------------------------

SImagePicker::~SImagePicker() noexcept =
    default;

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
    const ConfigType config = this->getConfiguration().get_child("config.<xmlattr>");

    const std::string videoRef = config.get<std::string>("videoReference", "center");

    auto it = m_videoRefMap.find(videoRef);

    if(it == m_videoRefMap.end())
    {
        SIGHT_WARN("'videoReference' of value '" + videoRef + "' is not handled.");
    }
    else
    {
        m_videoRef = it->second;
    }

    m_useCtrlModifier = config.get<bool>("useCtrlModifier", true);
    m_singlePointMode = config.get<bool>("singlePointMode", false);
}

//-----------------------------------------------------------------------------

void SImagePicker::updating()
{
}

//-----------------------------------------------------------------------------

void SImagePicker::getInteraction(data::tools::PickingInfo info)
{
    if(info.m_modifierMask == data::tools::PickingInfo::CTRL || !m_useCtrlModifier)
    {
        if(info.m_eventId == data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN)
        {
            const double x = info.m_worldPos[0];
            const double y = info.m_worldPos[1];
            const double z = info.m_worldPos[2];

            const std::array<double, 3> position = {{x, y, z}};

            if(m_singlePointMode)
            {
                std::size_t nPoints = 0;
                {
                    auto pointList = m_pointList.lock();
                    nPoints = pointList->getPoints().size();
                }

                if(nPoints > 0)
                {
                    this->removeLastPoint();
                }
                else
                {
                    this->addPoint(position);
                }
            }
            else
            {
                this->addPoint(position);
            }
        }
        else if(info.m_eventId == data::tools::PickingInfo::Event::MOUSE_RIGHT_DOWN)
        {
            this->removeLastPoint();
        }
    }
}

//-----------------------------------------------------------------------------

void SImagePicker::addPoint(const std::array<double, 3>& currentPoint)
{
    // Set z to 0 as it is an image.

    auto pointList      = m_pointList.lock();
    auto pixelPointList = m_pixelPointList.lock();

    data::Point::sptr point = std::make_shared<data::Point>(currentPoint[0], currentPoint[1], 0.);

    const auto camera = m_camera.lock();

    data::Point::sptr pixel;

    switch(m_videoRef)
    {
        case VideoReference::CENTER:
        {
            const std::array offset {static_cast<float>(camera->getWidth()) / 2.F,
                                     static_cast<float>(camera->getHeight()) / 2.F
            };

            // Shift point to set reference at top_left corner & inverse y axis.
            pixel = std::make_shared<data::Point>(
                currentPoint[0] + offset[0],
                offset[1] - currentPoint[1],
                0.
            );
            break;
        }

        case VideoReference::TOP_LEFT:
            pixel = std::make_shared<data::Point>(currentPoint[0], currentPoint[1], 0.);
            break;

        default:
            SIGHT_ERROR("Only video reference CENTER and TOP_LEFT are handled");
            break;
    }

    // "World" points.
    {
        pointList->getPoints().push_back(point);
        auto sig = pointList->signal<data::PointList::ModifiedSignalType>(
            data::PointList::MODIFIED_SIG
        );
        sig->async_emit();

        auto sig2 = pointList->signal<data::PointList::PointAddedSignalType>(
            data::PointList::POINT_ADDED_SIG
        );
        sig2->async_emit(point);
    }

    // Pixel points.
    {
        pixelPointList->getPoints().push_back(pixel);
        auto sig = pixelPointList->signal<data::PointList::ModifiedSignalType>(
            data::PointList::MODIFIED_SIG
        );
        sig->async_emit();

        auto sig2 = pixelPointList->signal<data::PointList::PointAddedSignalType>(
            data::PointList::POINT_ADDED_SIG
        );
        sig2->async_emit(point);
    }
}

//-----------------------------------------------------------------------------

void SImagePicker::removeLastPoint()
{
    auto pointList      = m_pointList.lock();
    auto pixelPointList = m_pixelPointList.lock();
    data::Point::sptr point;

    if(!pointList->getPoints().empty() && !pixelPointList->getPoints().empty())
    {
        {
            point = pointList->getPoints().back();
            pointList->getPoints().pop_back();

            auto sig = pointList->signal<data::PointList::ModifiedSignalType>(
                data::PointList::MODIFIED_SIG
            );
            sig->async_emit();

            auto sig2 = pointList->signal<data::PointList::PointRemovedSignalType>(
                data::PointList::POINT_REMOVED_SIG
            );
            sig2->async_emit(point);
        }

        {
            point = pixelPointList->getPoints().back();
            pixelPointList->getPoints().pop_back();

            auto sig = pixelPointList->signal<data::PointList::ModifiedSignalType>(
                data::PointList::MODIFIED_SIG
            );
            sig->async_emit();

            auto sig2 = pixelPointList->signal<data::PointList::PointRemovedSignalType>(
                data::PointList::POINT_REMOVED_SIG
            );
            sig2->async_emit(point);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::video
