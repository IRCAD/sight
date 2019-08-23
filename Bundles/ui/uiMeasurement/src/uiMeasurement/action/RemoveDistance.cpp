/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "uiMeasurement/action/RemoveDistance.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <exception>
#include <sstream>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::RemoveDistance, ::fwData::Image );

static const ::fwCom::Slots::SlotKeyType s_REMOVE_LAST_DISTANCE_SLOT = "removeLastDistance";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

RemoveDistance::RemoveDistance( ) noexcept
{
    newSlot(s_REMOVE_LAST_DISTANCE_SLOT, &RemoveDistance::removeLastDistance, this);
}

//------------------------------------------------------------------------------

RemoveDistance::~RemoveDistance() noexcept
{
}

//------------------------------------------------------------------------------

std::string distanceToStr(double dist)
{
    std::stringstream ss;
    ss.precision(3);
    ss << dist << " mm";
    return ss.str();
}

//------------------------------------------------------------------------------

::fwData::PointList::sptr  getDistanceToRemove(::fwData::Image::sptr image, bool& removeAll)
{
    ::fwData::PointList::sptr distToRemove;
    removeAll = false;
    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if(vectDist)
    {
        std::vector< std::string > selections;
        selections.push_back("ALL");
        std::map< std::string, ::fwData::PointList::sptr > correspondance;

        for(::fwData::Object::sptr obj :  *vectDist)
        {
            ::fwData::PointList::sptr pl = ::fwData::PointList::dynamicCast(obj);

            if ( pl->getPoints().size() != 2 )
            {
                continue;
            }                                              // we skip no paired pointList
            ::fwData::Point::sptr pt1 = pl->getPoints().front();
            ::fwData::Point::sptr pt2 = pl->getPoints().back();

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

        if ( !selections.empty() )
        {
            ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();
            selector->setTitle("Select a distance to remove");
            selector->setSelections(selections);
            std::string selection = selector->show();
            if( !selection.empty() )
            {
                if (selection == "ALL")
                {
                    removeAll = true;
                }
                else
                {
                    removeAll    = false;
                    distToRemove = correspondance[selection];
                }
            }
        }
    }
    return distToRemove;
}

//------------------------------------------------------------------------------

void RemoveDistance::notifyDeleteDistance(const ::fwData::Image::csptr& image,
                                          const ::fwData::PointList::csptr& distance) const
{
    auto sig = image->signal< ::fwData::Image::DistanceRemovedSignalType >(::fwData::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(distance);
}

//------------------------------------------------------------------------------

void RemoveDistance::notifyNewDistance(const ::fwData::Image::csptr& image,
                                       const ::fwData::PointList::sptr& distance) const
{
    auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(distance);
}

//------------------------------------------------------------------------------

void RemoveDistance::updating( )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
        && vectDist)
    {
        bool requestAll;
        ::fwData::PointList::sptr distToRemove = getDistanceToRemove(image, requestAll );

        // perform action only available distance
        if ( distToRemove )
        {
            SLM_ASSERT("No Field ImageDistancesId", vectDist);
            ::fwData::Vector::IteratorType newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());

            this->notifyDeleteDistance(image, distToRemove);
        }
        if ( requestAll )
        {
            // backup
            ::fwData::PointList::sptr backupDistance = image->getField< ::fwData::PointList >(
                ::fwDataTools::fieldHelper::Image::m_imageDistancesId );

            image->removeField( ::fwDataTools::fieldHelper::Image::m_imageDistancesId );
            this->notifyNewDistance(image, backupDistance);
        }
    }
}

//------------------------------------------------------------------------------

void RemoveDistance::removeLastDistance()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
        && vectDist)
    {
        ::fwData::PointList::sptr distToRemove = ::fwData::PointList::dynamicCast(*(*vectDist).rbegin());

        if ( distToRemove)
        {
            ::fwData::Vector::IteratorType newEnd = std::remove(vectDist->begin(), vectDist->end(), distToRemove);
            vectDist->getContainer().erase(newEnd, vectDist->end());
            this->notifyDeleteDistance(image, distToRemove);
        }
    }
}

//------------------------------------------------------------------------------

void RemoveDistance::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void RemoveDistance::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void RemoveDistance::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
