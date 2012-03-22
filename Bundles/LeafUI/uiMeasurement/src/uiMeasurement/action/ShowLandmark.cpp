/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <exception>

#include <fwData/PatientDB.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>


#include "uiMeasurement/action/ShowLandmark.hpp"


namespace uiMeasurement
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiMeasurement::action::ShowLandmark , ::fwData::Image) ;


//------------------------------------------------------------------------------

ShowLandmark::ShowLandmark( ) throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::LANDMARK );
}

//------------------------------------------------------------------------------

ShowLandmark::~ShowLandmark() throw()
{
}

//------------------------------------------------------------------------------

void ShowLandmark::info(std::ostream &_sstream )
{
    _sstream << "Action for show distance" << std::endl;
}

//------------------------------------------------------------------------------

void ShowLandmark::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (    image->getBuffer()==NULL ||
            ! image->getField( ::fwComEd::Dictionary::m_imageLandmarksId ))
    {
        this->::fwGui::IActionSrv::setIsActive(false);
        return;
    }

    ::fwData::Boolean::sptr showLandmarks = image->getField< ::fwData::Boolean >("ShowLandmarks", ::fwData::Boolean::New(true));
    bool isShown = showLandmarks->value();

    bool toShow = !isShown;
    image->setField("ShowLandmarks",  ::fwData::Boolean::NewSptr(toShow));

    std::vector< ::fwServices::IService::sptr > services = ::fwServices::OSR::getServices < ::fwServices::IService > (image);

    this->::fwGui::IActionSrv::setIsActive(isShown);

    // notify
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::LANDMARK );
    ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
}

//------------------------------------------------------------------------------

void ShowLandmark::swapping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Image::csptr img = this->getObject< ::fwData::Image >();
    ::fwData::Boolean::sptr showLandmarks = img->getField< ::fwData::Boolean >("ShowLandmarks", ::fwData::Boolean::New(true));

    // set check correctly
    this->::fwGui::IActionSrv::setIsActive( !(showLandmarks->value()) );
}

//------------------------------------------------------------------------------

void ShowLandmark::updating(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imgMsg =  ::fwComEd::ImageMsg::dynamicConstCast( msg );
    if ( imgMsg && imgMsg->hasEvent( ::fwComEd::ImageMsg::LANDMARK ) )
    {
        this->swapping();
    }
}

//------------------------------------------------------------------------------

void ShowLandmark::configuring() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ShowLandmark::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowLandmark::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------


} // namespace action
} // namespace uiMeasurement
