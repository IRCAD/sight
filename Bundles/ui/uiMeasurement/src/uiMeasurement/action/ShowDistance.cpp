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

#include "uiMeasurement/action/ShowDistance.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
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

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

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
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    if ( !::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image) )
    {
        this->::fwGui::IActionSrv::setIsActive(false);
    }
    else
    {
        ::fwData::Boolean::sptr showDistances =
            image->getField< ::fwData::Boolean >("ShowDistances", ::fwData::Boolean::New(true));
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

void ShowDistance::showDistance(bool)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    ::fwData::Boolean::sptr showDistances =
        image->getField< ::fwData::Boolean >("ShowDistances", ::fwData::Boolean::New(true));

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
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_DISTANCE_DISPLAYED_SIG, s_SHOW_DISTANCE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
