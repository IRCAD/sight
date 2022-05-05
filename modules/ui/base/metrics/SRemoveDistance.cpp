/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "modules/ui/base/metrics/SRemoveDistance.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>

#include <ui/base/dialog/SelectorDialog.hpp>

#include <sstream>

namespace sight::module::ui::base::metrics
{

static const core::com::Slots::SlotKeyType s_REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

//------------------------------------------------------------------------------

SRemoveDistance::SRemoveDistance() noexcept
{
    newSlot(s_REMOVE_LAST_DISTANCE_SLOT, &SRemoveDistance::removeLastDistance, this);
}

//------------------------------------------------------------------------------

SRemoveDistance::~SRemoveDistance() noexcept
{
}

//------------------------------------------------------------------------------

void SRemoveDistance::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SRemoveDistance::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SRemoveDistance::updating()
{
    const auto image = m_image.lock();

    auto vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        bool requestAll;
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

            this->notifyDeleteDistance(image.get_shared(), distToRemove);
        }

        if(requestAll)
        {
            data::PointList::sptr backupDistance = image->getField<data::PointList>("m_imageDistancesId");

            image->removeField("m_imageDistancesId");
            const auto sig = image->signal<data::Image::DistanceAddedSignalType>(
                data::Image::s_DISTANCE_ADDED_SIG
            );
            sig->asyncEmit(backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void SRemoveDistance::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
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
        selections.push_back("ALL");
        std::map<std::string, data::PointList::sptr> correspondence;

        for(const data::Object::sptr& obj : *vectDist)
        {
            const data::PointList::sptr pl = data::PointList::dynamicCast(obj);
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
            const sight::ui::base::dialog::SelectorDialog::sptr selector =
                sight::ui::base::dialog::SelectorDialog::New();
            selector->setTitle("Select a distance to remove");
            selector->setSelections(selections);
            std::string selection = selector->show();
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
) const
{
    const auto sig =
        _image->signal<data::Image::DistanceRemovedSignalType>(data::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(_distance);
}

//------------------------------------------------------------------------------

void SRemoveDistance::removeLastDistance()
{
    const auto image = m_image.lock();

    const data::Vector::sptr vectDist = data::helper::MedicalImage::getDistances(*image);

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared())
       && vectDist)
    {
        const data::PointList::sptr distToRemove = data::PointList::dynamicCast(*(*vectDist).rbegin());

        if(distToRemove)
        {
            auto newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->erase(newEnd, vectDist->end());
            this->notifyDeleteDistance(image.get_shared(), distToRemove);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::metrics.
