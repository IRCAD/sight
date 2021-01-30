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

#include "uiMeasurement/action/SRemoveDistance.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Image.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Vector.hpp>

#include <ui/base/dialog/SelectorDialog.hpp>

#include <sstream>

namespace uiMeasurement
{
namespace action
{

static const core::com::Slots::SlotKeyType s_REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

static const services::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SRemoveDistance::SRemoveDistance( ) noexcept
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
    this->ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SRemoveDistance::starting()
{
    this->ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SRemoveDistance::updating( )
{
    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    data::Vector::sptr vectDist
        = image->getField< data::Vector >(data::tools::fieldHelper::Image::m_imageDistancesId);

    if (data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared())
        && vectDist)
    {
        bool requestAll;
        data::PointList::sptr distToRemove = getDistanceToRemove(image.get_shared(), requestAll );

        if(distToRemove)
        {
            SLM_ASSERT("No field image distances id", vectDist);
            const data::Vector::ConstIteratorType newEnd = std::remove(vectDist->begin(),
                                                                       vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());

            this->notifyDeleteDistance(image.get_shared(), distToRemove);
        }
        if(requestAll)
        {
            data::PointList::sptr backupDistance = image->getField< data::PointList >(
                data::tools::fieldHelper::Image::m_imageDistancesId );

            image->removeField(data::tools::fieldHelper::Image::m_imageDistancesId );
            const auto sig = image->signal< data::Image::DistanceAddedSignalType >(
                data::Image::s_DISTANCE_ADDED_SIG);
            sig->asyncEmit(backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void SRemoveDistance::stopping()
{
    this->ui::base::IActionSrv::actionServiceStopping();
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
    data::Vector::sptr vectDist
        = _image->getField< data::Vector >(data::tools::fieldHelper::Image::m_imageDistancesId);

    if(vectDist)
    {
        std::vector< std::string > selections;
        selections.push_back("ALL");
        std::map< std::string, data::PointList::sptr > correspondance;

        for(const data::Object::sptr& obj : *vectDist)
        {
            const data::PointList::sptr pl = data::PointList::dynamicCast(obj);
            SLM_ASSERT("The distance should be a point list", pl);
            SLM_ASSERT("The distance must contains two points", pl->getPoints().size() == 2);

            const data::Point::sptr pt1 = pl->getPoints().front();
            const data::Point::sptr pt2 = pl->getPoints().back();

            double dist  = 0;
            double delta = pt1->getCoord()[0] - pt2->getCoord()[0];
            dist += delta*delta;
            delta = pt1->getCoord()[1] - pt2->getCoord()[1];
            dist += delta*delta;
            delta = pt1->getCoord()[2] - pt2->getCoord()[2];
            dist += delta*delta;
            dist  = sqrt(dist);

            selections.push_back( distanceToStr(dist) );
            correspondance[ selections.back() ] = pl;
        }

        if(!selections.empty())
        {
            const sight::ui::base::dialog::SelectorDialog::sptr selector =
                sight::ui::base::dialog::SelectorDialog::New();
            selector->setTitle("Select a distance to remove");
            selector->setSelections(selections);
            std::string selection = selector->show();
            if( !selection.empty() )
            {
                if (selection == "ALL")
                {
                    _removeAll = true;
                }
                else
                {
                    _removeAll   = false;
                    distToRemove = correspondance[selection];
                }
            }
        }
    }
    return distToRemove;
}

//------------------------------------------------------------------------------

void SRemoveDistance::notifyDeleteDistance(const data::Image::csptr& _image,
                                           const data::PointList::csptr& _distance) const
{
    const auto sig =
        _image->signal< data::Image::DistanceRemovedSignalType >(data::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(_distance);
}

//------------------------------------------------------------------------------

void SRemoveDistance::removeLastDistance()
{
    const auto image = this->getLockedInOut< data::Image >(s_IMAGE_INOUT);

    const data::Vector::sptr vectDist
        = image->getField< data::Vector >(data::tools::fieldHelper::Image::m_imageDistancesId);

    if (data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared())
        && vectDist)
    {
        const data::PointList::sptr distToRemove = data::PointList::dynamicCast(*(*vectDist).rbegin());

        if(distToRemove)
        {
            data::Vector::IteratorType newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());
            this->notifyDeleteDistance(image.get_shared(), distToRemove);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace action.

} // namespace uiMeasurement.
