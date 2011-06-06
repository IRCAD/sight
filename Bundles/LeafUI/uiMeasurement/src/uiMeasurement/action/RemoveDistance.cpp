/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>
#include <fwCore/base.hpp>

#include <exception>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Composite.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <fwGui/dialog/SelectorDialog.hpp>

#include "uiMeasurement/action/RemoveDistance.hpp"

#include <sstream>


namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::RemoveDistance , ::fwData::Image ) ;


//------------------------------------------------------------------------------


RemoveDistance::RemoveDistance( ) throw()
{}

//------------------------------------------------------------------------------

RemoveDistance::~RemoveDistance() throw()
{}

//------------------------------------------------------------------------------

void RemoveDistance::info(std::ostream &_sstream )
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

::fwData::PointList::sptr  getDistanceToRemove(::fwData::Image::sptr image, bool &removeAll)
{
    ::fwData::PointList::sptr distToRemove;
    removeAll = false;
    typedef ::fwData::ContainerCaster< ::fwData::PointList >::iterator Iterator;
    Iterator plIter( image->getField(::fwComEd::Dictionary::m_imageDistancesId)->children().begin() );
    Iterator end( image->getField(::fwComEd::Dictionary::m_imageDistancesId)->children().end() );

    std::vector< std::string > selections;
    selections.push_back("ALL");
    std::map< std::string , ::fwData::PointList::sptr > correspondance;

    for (  ; plIter != end; ++plIter )
    {
        ::fwData::PointList::sptr pl = *plIter;

        if ( pl->getPoints().size()!=2 ) { continue; } // we skip no paired pointList
        ::fwData::Point::sptr pt1 =  pl->getPoints().front();
        ::fwData::Point::sptr pt2 =  pl->getPoints().back();

        double dist=0;
        double delta = pt1->getCRefCoord()[0] - pt2->getCRefCoord()[0];
        dist += delta*delta;
        delta = pt1->getCRefCoord()[1] - pt2->getCRefCoord()[1];
        dist += delta*delta;
        delta = pt1->getCRefCoord()[2] - pt2->getCRefCoord()[2];
        dist += delta*delta;
        dist = sqrt(dist);

        selections.push_back( distanceToStr(dist) );
        correspondance[ selections.back() ] = pl;
    }

    if ( selections.size() )
    {
        ::fwGui::dialog::SelectorDialog::NewSptr selector;
        selector->setTitle("Select a distance to remove");
        selector->setSelections(selections);
        std::string selection = selector->show();
        if( ! selection.empty() )
        {
            if (selection=="ALL")
            {
                removeAll=true;
            }
            else
            {
                removeAll=false;
                distToRemove = correspondance[selection];
            }
        }
    }

    return distToRemove;
}

//------------------------------------------------------------------------------

void RemoveDistance::notifyNewDistance( ::fwData::Image::sptr image , ::fwData::Object::sptr backup)
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::DISTANCE, backup );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void RemoveDistance::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE("RemoveDistance::updating");

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (image->getBuffer()==NULL) {return;}

    if ( image->getFieldSize(::fwComEd::Dictionary::m_imageDistancesId) == false )
    {
        return;
    }

    bool requestAll;
    ::fwData::PointList::sptr distToRemove = getDistanceToRemove(image, requestAll );

    SLM_TRACE("RemoveDistance::distToRemove");
    // perform action only available distance
    if ( distToRemove )
    {
        SLM_TRACE("RemoveDistance::distToRemove");
        image->removeFieldElement( ::fwComEd::Dictionary::m_imageDistancesId , distToRemove);
        notifyNewDistance(image, distToRemove);
    }
    if ( requestAll )
    {
        SLM_TRACE("RemoveDistance::distToRemove ALL");
        // backup
        ::fwTools::Field::sptr imageOldLandMaskField = image->getField( ::fwComEd::Dictionary::m_imageDistancesId );
        ::fwData::Object::NewSptr dummy;
        dummy->children().push_back(imageOldLandMaskField);
        image->removeField( ::fwComEd::Dictionary::m_imageDistancesId ); // erase lan field
        notifyNewDistance(image, dummy);
    }
}

//------------------------------------------------------------------------------

void RemoveDistance::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void RemoveDistance::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void RemoveDistance::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void RemoveDistance::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
