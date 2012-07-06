/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <exception>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/Vector.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>


#include "uiMeasurement/action/AddDistance.hpp"

namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::AddDistance , ::fwData::Image ) ;


//------------------------------------------------------------------------------


AddDistance::AddDistance( ) throw():
    m_actionCheckId(-1)
{}

//------------------------------------------------------------------------------

AddDistance::~AddDistance() throw()
{}

//------------------------------------------------------------------------------

void AddDistance::info(std::ostream &_sstream )
{
    _sstream << "Action for remove distance" << std::endl;
}

//------------------------------------------------------------------------------

void AddDistance::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("AddDistance::updating");
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (!image) {return; }

    ::fwData::Point::NewSptr pt1;
    std::copy( image->getOrigin().begin(),  image->getOrigin().begin() +3, pt1->getRefCoord().begin() );

    ::fwData::Point::NewSptr pt2;
    std::copy( image->getSize().begin(),  image->getSize().begin() +3, pt2->getRefCoord().begin() );

    std::transform( pt2->getRefCoord().begin(),pt2->getRefCoord().end(),
            image->getSpacing().begin(),
            pt2->getRefCoord().begin(),
            std::multiplies<double>() );
    std::transform( pt2->getRefCoord().begin(),pt2->getRefCoord().end(),
            image->getOrigin().begin(),
            pt2->getRefCoord().begin(),
            std::plus<double>() );

    ::fwData::PointList::NewSptr pl;

    pl->getRefPoints().push_back( pt1 );
    pl->getRefPoints().push_back( pt2 );

    ::fwData::Vector::sptr vectDist;
    vectDist = image->setDefaultField(::fwComEd::Dictionary::m_imageDistancesId, ::fwData::Vector::New());

    vectDist->getContainer().push_back(pl);

    // force distance to be shown
    image->setField("ShowDistances",  ::fwData::Boolean::NewSptr(true));


    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::DISTANCE );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void AddDistance::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void AddDistance::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void AddDistance::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void AddDistance::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
