/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <exception>

#include <fwData/Boolean.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Acquisition.hpp>

#include <fwServices/ObjectMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "uiMeasurement/action/ShowDistance.hpp"


namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::ShowDistance , ::fwData::Image ) ;


//------------------------------------------------------------------------------

ShowDistance::ShowDistance( ) throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::DISTANCE );
}

//------------------------------------------------------------------------------

ShowDistance::~ShowDistance() throw()
{}

//------------------------------------------------------------------------------

void ShowDistance::info(std::ostream &_sstream )
{
    _sstream << "Action for show distance" << std::endl;
}

//------------------------------------------------------------------------------

void ShowDistance::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if ( !::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image) )
    {
        this->::fwGui::IActionSrv::setIsActive(false);
    }
    else
    {
        bool isShown
        = (!image->getFieldSize("ShowDistances")) ? true : image->getFieldSingleElement< ::fwData::Boolean > ("ShowDistances")->value();

        bool toShow = !isShown;
        image->setFieldSingleElement("ShowDistances", ::fwData::Boolean::NewSptr(toShow));

        // auto manage hide/show : use Field Information instead let gui manage checking
        this->::fwGui::IActionSrv::setIsActive(!toShow);

        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::ImageMsg::DISTANCE );
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
    }
}

//------------------------------------------------------------------------------

void ShowDistance::swapping() throw(::fwTools::Failed)
{
    ::fwServices::ObjectMsg::sptr dummy;
    this->updating(dummy);
}

//------------------------------------------------------------------------------

void ShowDistance::updating( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::ImageMsg::csptr imgMsg =  ::fwComEd::ImageMsg::dynamicConstCast( msg );
    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::DISTANCE ) )
    {
        ::fwData::Image::csptr img = this->getObject< ::fwData::Image >();
        bool showDistances = (img->getFieldSize("ShowDistances"))==0 ? true : img->getFieldSingleElement< ::fwData::Boolean > ("ShowDistances")->value() ;
        // set WX check correctly
        this->::fwGui::IActionSrv::setIsActive( !showDistances);
    }
}

//------------------------------------------------------------------------------

void ShowDistance::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ShowDistance::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowDistance::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
