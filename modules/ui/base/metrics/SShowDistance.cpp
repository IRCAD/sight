/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/ui/base/metrics/SShowDistance.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <service/macros.hpp>

#include <exception>

namespace sight::module::ui::base::metrics
{

static const core::com::Slots::SlotKeyType s_SHOW_DISTANCE_SLOT = "showDistance";

//------------------------------------------------------------------------------

SShowDistance::SShowDistance() noexcept
{
    newSlot(s_SHOW_DISTANCE_SLOT, &SShowDistance::showDistance, this);
}

//------------------------------------------------------------------------------

SShowDistance::~SShowDistance() noexcept
{
}

//------------------------------------------------------------------------------

void SShowDistance::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SShowDistance::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SShowDistance::updating()
{
    const auto image = m_image.lock();

    if(!data::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
    {
        this->sight::ui::base::IAction::setIsActive(false);
    }
    else
    {
        const data::Boolean::sptr showDistances =
            image->getField<data::Boolean>(
                data::fieldHelper::Image::m_distanceVisibility,
                data::Boolean::New(
                    true
                )
            );
        const bool isShown = showDistances->value();

        const bool toShow = !isShown;
        image->setField(data::fieldHelper::Image::m_distanceVisibility, data::Boolean::New(toShow));

        // Manage hide/show from the field information.
        this->sight::ui::base::IAction::setIsActive(!toShow);

        const auto sig = image->signal<data::Image::DistanceDisplayedSignalType>(
            data::Image::s_DISTANCE_DISPLAYED_SIG
        );
        {
            core::com::Connection::Blocker block(sig->getConnection(this->slot(s_SHOW_DISTANCE_SLOT)));
            sig->asyncEmit(toShow);
        }
    }
}

//------------------------------------------------------------------------------

void SShowDistance::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SShowDistance::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE, data::Image::s_DISTANCE_DISPLAYED_SIG, s_SHOW_DISTANCE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SShowDistance::showDistance(bool)
{
    const auto image = m_image.lock();

    data::Boolean::sptr SShowDistances =
        image->getField<data::Boolean>(
            data::fieldHelper::Image::m_distanceVisibility,
            data::Boolean::New(
                true
            )
        );

    this->sight::ui::base::IAction::setIsActive(!(SShowDistances->value()));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::metrics
