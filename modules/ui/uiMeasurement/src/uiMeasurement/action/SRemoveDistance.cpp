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

#include "uiMeasurement/action/SRemoveDistance.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include <sstream>

namespace uiMeasurement
{
namespace action
{

static const ::fwCom::Slots::SlotKeyType s_REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

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

std::string SRemoveDistance::distanceToStr(double _dist)
{
    std::stringstream ss;
    ss.precision(3);
    ss << _dist << " mm";
    return ss.str();
}

//------------------------------------------------------------------------------

::fwData::PointList::sptr SRemoveDistance::getDistanceToRemove(const ::fwData::Image::csptr _image, bool& _removeAll)
{
    ::fwData::PointList::sptr distToRemove;
    _removeAll = false;
    ::fwData::Vector::sptr vectDist
        = _image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if(vectDist)
    {
        std::vector< std::string > selections;
        selections.push_back("ALL");
        std::map< std::string, ::fwData::PointList::sptr > correspondance;

        for(const ::fwData::Object::sptr obj : *vectDist)
        {
            const ::fwData::PointList::sptr pl = ::fwData::PointList::dynamicCast(obj);
            SLM_ASSERT("The distance should be a point list", pl);
            SLM_ASSERT("The distance must contains two points", pl->getPoints().size() == 2);

            const ::fwData::Point::sptr pt1 = pl->getPoints().front();
            const ::fwData::Point::sptr pt2 = pl->getPoints().back();

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
            const ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();
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

void SRemoveDistance::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SRemoveDistance::updating( )
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    const ::fwData::mt::ObjectWriteLock lock(image);

    ::fwData::Vector::sptr vectDist
        = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
        && vectDist)
    {
        bool requestAll;
        ::fwData::PointList::sptr distToRemove = getDistanceToRemove(image, requestAll );

        // perform action only available distance
        if(distToRemove)
        {
            SLM_ASSERT("No field image distances id", vectDist);
            const ::fwData::Vector::ConstIteratorType newEnd = std::remove(vectDist->begin(),
                                                                           vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());

            this->notifyDeleteDistance(image, distToRemove);
        }
        if(requestAll)
        {
            // backup
            ::fwData::PointList::sptr backupDistance = image->getField< ::fwData::PointList >(
                ::fwDataTools::fieldHelper::Image::m_imageDistancesId );

            image->removeField(::fwDataTools::fieldHelper::Image::m_imageDistancesId );
            const auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(
                ::fwData::Image::s_DISTANCE_ADDED_SIG);
            sig->asyncEmit(backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void SRemoveDistance::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SRemoveDistance::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SRemoveDistance::notifyDeleteDistance(const ::fwData::Image::csptr& image,
                                           const ::fwData::PointList::csptr& distance) const
{
    const auto sig =
        image->signal< ::fwData::Image::DistanceRemovedSignalType >(::fwData::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(distance);
}

//------------------------------------------------------------------------------

void SRemoveDistance::removeLastDistance()
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("'" + s_IMAGE_INOUT + "' does not exist.", image);
    const ::fwData::mt::ObjectWriteLock lock(image);

    const ::fwData::Vector::sptr vectDist
        = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
        && vectDist)
    {
        const ::fwData::PointList::sptr distToRemove = ::fwData::PointList::dynamicCast(*(*vectDist).rbegin());

        if(distToRemove)
        {
            ::fwData::Vector::IteratorType newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());
            this->notifyDeleteDistance(image, distToRemove);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
