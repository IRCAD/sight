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

#include "modules/ui/__/metrics/remove_distance.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/vector.hpp>

#include <ui/__/dialog/selector.hpp>

#include <sstream>

namespace sight::module::ui::metrics
{

static const core::com::slots::key_t REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

//------------------------------------------------------------------------------

remove_distance::remove_distance() noexcept
{
    new_slot(REMOVE_LAST_DISTANCE_SLOT, &remove_distance::removeLastDistance, this);
}

//------------------------------------------------------------------------------

void remove_distance::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void remove_distance::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void remove_distance::updating()
{
    const auto image = m_image.lock();

    auto vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        bool requestAll                     = false;
        data::point_list::sptr distToRemove = getDistanceToRemove(image.get_shared(), requestAll);

        if(distToRemove)
        {
            SIGHT_ASSERT("No field image distances id", vectDist);
            const auto newEnd = std::remove(
                vectDist->begin(),
                vectDist->end(),
                distToRemove
            );
            vectDist->erase(newEnd, vectDist->end());

            sight::module::ui::metrics::remove_distance::notifyDeleteDistance(image.get_shared(), distToRemove);
        }

        if(requestAll)
        {
            auto backupDistance = image->getField<data::point_list>("m_imageDistancesId");

            image->removeField("m_imageDistancesId");
            const auto sig = image->signal<data::image::DistanceRemovedSignalType>(
                data::image::DISTANCE_REMOVED_SIG
            );
            sig->async_emit(backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void remove_distance::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

std::string remove_distance::distanceToStr(double _dist)
{
    std::stringstream ss;
    ss.precision(3);
    ss << _dist << " mm";
    return ss.str();
}

//------------------------------------------------------------------------------

data::point_list::sptr remove_distance::getDistanceToRemove(const data::image::csptr _image, bool& _removeAll)
{
    data::point_list::sptr distToRemove;
    _removeAll = false;
    data::vector::sptr vectDist = data::helper::MedicalImage::getDistances(*_image);

    if(vectDist)
    {
        std::vector<std::string> selections;
        selections.emplace_back("ALL");
        std::map<std::string, data::point_list::sptr> correspondence;

        for(const data::object::sptr& obj : *vectDist)
        {
            const data::point_list::sptr pl = std::dynamic_pointer_cast<data::point_list>(obj);
            SIGHT_ASSERT("The distance should be a point list", pl);
            SIGHT_ASSERT("The distance must contains two points", pl->getPoints().size() == 2);

            const data::point::sptr pt1 = pl->getPoints().front();
            const data::point::sptr pt2 = pl->getPoints().back();

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
            sight::ui::dialog::selector selector;
            selector.setTitle("Select a distance to remove");
            selector.set_choices(selections);
            if(const auto& choices = selector.show(); !choices.empty())
            {
                const auto choice = choices.front();
                if(choice == "ALL")
                {
                    _removeAll = true;
                }
                else
                {
                    _removeAll   = false;
                    distToRemove = correspondence[choice];
                }
            }
        }
    }

    return distToRemove;
}

//------------------------------------------------------------------------------

void remove_distance::notifyDeleteDistance(
    const data::image::csptr& _image,
    const data::point_list::csptr& _distance
)
{
    const auto sig =
        _image->signal<data::image::DistanceRemovedSignalType>(data::image::DISTANCE_REMOVED_SIG);
    sig->async_emit(_distance);
}

//------------------------------------------------------------------------------

void remove_distance::removeLastDistance()
{
    const auto image = m_image.lock();

    const data::vector::sptr vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        const data::point_list::sptr distToRemove = std::dynamic_pointer_cast<data::point_list>(*(*vectDist).rbegin());

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
