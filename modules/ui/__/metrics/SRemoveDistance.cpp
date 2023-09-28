/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/ui/__/metrics/SRemoveDistance.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>

#include <ui/__/dialog/selector.hpp>

#include <sstream>

namespace sight::module::ui::metrics
{

static const core::com::slots::key_t REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

//------------------------------------------------------------------------------

SRemoveDistance::SRemoveDistance() noexcept
{
    new_slot(REMOVE_LAST_DISTANCE_SLOT, &SRemoveDistance::removeLastDistance, this);
}

//------------------------------------------------------------------------------

SRemoveDistance::~SRemoveDistance() noexcept =
    default;

//------------------------------------------------------------------------------

void SRemoveDistance::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void SRemoveDistance::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SRemoveDistance::updating()
{
    const auto image = m_image.lock();

    auto vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        bool requestAll                    = false;
        data::PointList::sptr distToRemove = getDistanceToRemove(image.get_shared(), requestAll);

        if(distToRemove)
        {
            SIGHT_ASSERT("No field image distances id", vectDist);
            const auto newEnd = std::remove(
                vectDist->begin(),
                vectDist->end(),
                distToRemove
            );
            vectDist->erase(newEnd, vectDist->end());

            sight::module::ui::metrics::SRemoveDistance::notifyDeleteDistance(image.get_shared(), distToRemove);
        }

        if(requestAll)
        {
            data::PointList::sptr backupDistance = image->getField<data::PointList>("m_imageDistancesId");

            image->removeField("m_imageDistancesId");
            const auto sig = image->signal<data::Image::DistanceRemovedSignalType>(
                data::Image::DISTANCE_REMOVED_SIG
            );
            sig->async_emit(backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void SRemoveDistance::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

std::string SRemoveDistance::distanceToStr(double _dist)
{
    std::stringstream ss;
    ss.precision(3);
    ss << _dist << " mm";
    return ss.str();
}

//------------------------------------------------------------------------------

data::PointList::sptr SRemoveDistance::getDistanceToRemove(const data::Image::csptr _image, bool& _removeAll)
{
    data::PointList::sptr distToRemove;
    _removeAll = false;
    data::Vector::sptr vectDist = data::helper::MedicalImage::getDistances(*_image);

    if(vectDist)
    {
        std::vector<std::string> selections;
        selections.emplace_back("ALL");
        std::map<std::string, data::PointList::sptr> correspondence;

        for(const data::Object::sptr& obj : *vectDist)
        {
            const data::PointList::sptr pl = std::dynamic_pointer_cast<data::PointList>(obj);
            SIGHT_ASSERT("The distance should be a point list", pl);
            SIGHT_ASSERT("The distance must contains two points", pl->getPoints().size() == 2);

            const data::Point::sptr pt1 = pl->getPoints().front();
            const data::Point::sptr pt2 = pl->getPoints().back();

            double dist  = 0;
            double delta = pt1->getCoord()[0] - pt2->getCoord()[0];
            dist += delta * delta;
            delta = pt1->getCoord()[1] - pt2->getCoord()[1];
            dist += delta * delta;
            delta = pt1->getCoord()[2] - pt2->getCoord()[2];
            dist += delta * delta;
            dist  = sqrt(dist);

            selections.push_back(distanceToStr(dist));
            correspondence[selections.back()] = pl;
        }

        if(!selections.empty())
        {
            const sight::ui::dialog::selector::sptr selector =
                std::make_shared<sight::ui::dialog::selector>();
            selector->setTitle("Select a distance to remove");
            selector->set_choices(selections);
            std::string selection = selector->show()[0];
            if(!selection.empty())
            {
                if(selection == "ALL")
                {
                    _removeAll = true;
                }
                else
                {
                    _removeAll   = false;
                    distToRemove = correspondence[selection];
                }
            }
        }
    }

    return distToRemove;
}

//------------------------------------------------------------------------------

void SRemoveDistance::notifyDeleteDistance(
    const data::Image::csptr& _image,
    const data::PointList::csptr& _distance
)
{
    const auto sig =
        _image->signal<data::Image::DistanceRemovedSignalType>(data::Image::DISTANCE_REMOVED_SIG);
    sig->async_emit(_distance);
}

//------------------------------------------------------------------------------

void SRemoveDistance::removeLastDistance()
{
    const auto image = m_image.lock();

    const data::Vector::sptr vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        const data::PointList::sptr distToRemove = std::dynamic_pointer_cast<data::PointList>(*(*vectDist).rbegin());

        if(distToRemove)
        {
            auto newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->erase(newEnd, vectDist->end());
            this->notifyDeleteDistance(image.get_shared(), distToRemove);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics.
