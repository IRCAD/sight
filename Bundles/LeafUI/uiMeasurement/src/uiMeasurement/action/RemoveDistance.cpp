/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/RemoveDistance.hpp"

#include <fwCore/base.hpp>

#include <fwCom/Signal.hxx>

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

//------------------------------------------------------------------------------

RemoveDistance::RemoveDistance( ) noexcept
{
}

//------------------------------------------------------------------------------

RemoveDistance::~RemoveDistance() noexcept
{
}

//------------------------------------------------------------------------------

void RemoveDistance::info(std::ostream& _sstream )
{
    _sstream << "Action for remove distance" << std::endl;
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
            double delta = pt1->getCRefCoord()[0] - pt2->getCRefCoord()[0];
            dist += delta*delta;
            delta = pt1->getCRefCoord()[1] - pt2->getCRefCoord()[1];
            dist += delta*delta;
            delta = pt1->getCRefCoord()[2] - pt2->getCRefCoord()[2];
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
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

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
