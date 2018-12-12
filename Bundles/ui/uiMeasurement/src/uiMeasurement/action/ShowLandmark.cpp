/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

void ShowLandmark::showLandmark(bool)
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
    FW_DEPRECATED_MSG("This service is deprecated", "18.0.");
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
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
