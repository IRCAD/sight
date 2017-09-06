/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMeasurement/action/ShowDistance.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::ShowDistance, ::fwData::Image );

static const ::fwCom::Slots::SlotKeyType s_SHOW_DISTANCE_SLOT = "showDistance";

//------------------------------------------------------------------------------

ShowDistance::ShowDistance( ) noexcept
{
    newSlot(s_SHOW_DISTANCE_SLOT, &ShowDistance::showDistance, this);
}

//------------------------------------------------------------------------------

ShowDistance::~ShowDistance() noexcept
{
}

//------------------------------------------------------------------------------

void ShowDistance::info(std::ostream& _sstream )
{
    _sstream << "Action for show distance" << std::endl;
}

//------------------------------------------------------------------------------

void ShowDistance::updating()
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    if ( !::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image) )
    {
        this->::fwGui::IActionSrv::setIsActive(false);
    }
    else
    {
        ::fwData::Boolean::sptr showDistances = image->getField< ::fwData::Boolean >("ShowDistances", ::fwData::Boolean::New(
                                                                                         true));
        bool isShown = showDistances->value();

        bool toShow = !isShown;
        image->setField("ShowDistances", ::fwData::Boolean::New(toShow));

        // auto manage hide/show : use Field Information instead let gui manage checking
        this->::fwGui::IActionSrv::setIsActive(!toShow);

        auto sig = image->signal< ::fwData::Image::DistanceDisplayedSignalType >(
            ::fwData::Image::s_DISTANCE_DISPLAYED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_SHOW_DISTANCE_SLOT)));
            sig->asyncEmit(toShow);
        }
    }
}

//------------------------------------------------------------------------------

void ShowDistance::swapping()
{
    ::fwData::Image::csptr img            = this->getObject< ::fwData::Image >();
    ::fwData::Boolean::sptr showDistances =
        img->getField< ::fwData::Boolean >("ShowDistances", ::fwData::Boolean::New(true));

    this->::fwGui::IActionSrv::setIsActive( !(showDistances->value()) );
}

//------------------------------------------------------------------------------

void ShowDistance::showDistance(bool isShown)
{
    ::fwData::Image::csptr img            = this->getObject< ::fwData::Image >();
    ::fwData::Boolean::sptr showDistances =
        img->getField< ::fwData::Boolean >("ShowDistances", ::fwData::Boolean::New(true));

    this->::fwGui::IActionSrv::setIsActive( !(showDistances->value()) );
}

//------------------------------------------------------------------------------

void ShowDistance::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ShowDistance::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowDistance::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ShowDistance::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( "image", ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_SHOW_DISTANCE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
