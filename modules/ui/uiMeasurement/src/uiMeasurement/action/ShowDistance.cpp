/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <services/macros.hpp>

#include <exception>

namespace uiMeasurement
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::uiMeasurement::action::ShowDistance, ::sight::data::Image )

static const core::com::Slots::SlotKeyType s_SHOW_DISTANCE_SLOT = "showDistance";

static const services::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

ShowDistance::ShowDistance( ) noexcept
{
    FW_DEPRECATED("::uiMeasurement::action::ShowDistance", "::uiMeasurement::action::SShowDistance", "22.0");
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
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    if ( !::data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image) )
    {
        this->ui::base::IActionSrv::setIsActive(false);
    }
    else
    {
        data::Boolean::sptr showDistances =
            image->getField< data::Boolean >(data::tools::fieldHelper::Image::m_distanceVisibility, data::Boolean::New(
                                                 true));
        bool isShown = showDistances->value();

        bool toShow = !isShown;
        image->setField(data::tools::fieldHelper::Image::m_distanceVisibility, data::Boolean::New(toShow));

        // auto manage hide/show : use Field Information instead let gui manage checking
        this->ui::base::IActionSrv::setIsActive(!toShow);

        auto sig = image->signal< data::Image::DistanceDisplayedSignalType >(
            data::Image::s_DISTANCE_DISPLAYED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(this->slot(s_SHOW_DISTANCE_SLOT)));
            sig->asyncEmit(toShow);
        }
    }
}

//------------------------------------------------------------------------------

void ShowDistance::showDistance(bool)
{
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    data::Boolean::sptr showDistances =
        image->getField< data::Boolean >(data::tools::fieldHelper::Image::m_distanceVisibility, data::Boolean::New(
                                             true));

    this->ui::base::IActionSrv::setIsActive( !(showDistances->value()) );
}

//------------------------------------------------------------------------------

void ShowDistance::configuring()
{
    this->ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ShowDistance::starting()
{
    this->ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowDistance::stopping()
{
    this->ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

services::IService::KeyConnectionsMap ShowDistance::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, data::Image::s_DISTANCE_DISPLAYED_SIG, s_SHOW_DISTANCE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMeasurement
