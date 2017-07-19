/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/ShowLandmark.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::ShowLandmark, ::fwData::Image);

static const ::fwCom::Slots::SlotKeyType s_SHOW_LANDMARK_SLOT = "showLandmark";

//------------------------------------------------------------------------------

ShowLandmark::ShowLandmark( ) noexcept
{
    newSlot(s_SHOW_LANDMARK_SLOT, &ShowLandmark::showLandmark, this);
}

//------------------------------------------------------------------------------

ShowLandmark::~ShowLandmark() noexcept
{
}

//------------------------------------------------------------------------------

void ShowLandmark::info(std::ostream& _sstream )
{
    _sstream << "Action for show distance" << std::endl;
}

//------------------------------------------------------------------------------

void ShowLandmark::updating()
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if (   !::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image) ||
           !image->getField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId ))
    {
        this->::fwGui::IActionSrv::setIsActive(false);
        return;
    }

    ::fwData::Boolean::sptr showLandmarks = image->getField< ::fwData::Boolean >("ShowLandmarks", ::fwData::Boolean::New(
                                                                                     true));
    bool isShown = showLandmarks->value();

    bool toShow = !isShown;
    image->setField("ShowLandmarks",  ::fwData::Boolean::New(toShow));

    auto services = ::fwServices::OSR::getServices < ::fwServices::IService > (image);

    this->::fwGui::IActionSrv::setIsActive(isShown);

    // notify
    auto sig = image->signal< ::fwData::Image::LandmarkDisplayedSignalType >(::fwData::Image::s_LANDMARK_DISPLAYED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_SHOW_LANDMARK_SLOT)));
        sig->asyncEmit(isShown);
    }
}

//------------------------------------------------------------------------------

void ShowLandmark::swapping()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::csptr img            = this->getObject< ::fwData::Image >();
    ::fwData::Boolean::sptr showLandmarks =
        img->getField< ::fwData::Boolean >("ShowLandmarks", ::fwData::Boolean::New(true));

    // set check correctly
    this->::fwGui::IActionSrv::setIsActive( !(showLandmarks->value()) );
}

//------------------------------------------------------------------------------

void ShowLandmark::showLandmark(bool isShown)
{
    this->swapping();
}

//------------------------------------------------------------------------------

void ShowLandmark::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ShowLandmark::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ShowLandmark::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_LANDMARK_DISPLAYED_SIG, s_SHOW_LANDMARK_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ShowLandmark::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( "image", ::fwData::Image::s_LANDMARK_DISPLAYED_SIG, s_SHOW_LANDMARK_SLOT );

    return connections;
}

//------------------------------------------------------------------------------


} // namespace action
} // namespace uiMeasurement
