/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/ui/__/metrics/SShowDistance.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/Boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <service/macros.hpp>

#include <exception>

namespace sight::module::ui::metrics
{

static const core::com::slots::key_t SHOW_DISTANCE_SLOT = "showDistance";

//------------------------------------------------------------------------------

SShowDistance::SShowDistance() noexcept
{
    new_slot(SHOW_DISTANCE_SLOT, &SShowDistance::showDistance, this);
}

//------------------------------------------------------------------------------

SShowDistance::~SShowDistance() noexcept =
    default;

//------------------------------------------------------------------------------

void SShowDistance::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void SShowDistance::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SShowDistance::updating()
{
    const auto image = m_image.lock();

    if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        this->sight::ui::action::setChecked(false);
    }
    else
    {
        const bool isShown = data::helper::MedicalImage::getDistanceVisibility(*image);

        const bool toShow = !isShown;
        data::helper::MedicalImage::setDistanceVisibility(*image, toShow);

        // Manage hide/show from the field information.
        this->sight::ui::action::setChecked(!toShow);

        const auto sig = image->signal<data::Image::DistanceDisplayedSignalType>(
            data::Image::DISTANCE_DISPLAYED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(this->slot(SHOW_DISTANCE_SLOT)));
            sig->async_emit(toShow);
        }
    }
}

//------------------------------------------------------------------------------

void SShowDistance::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

service::connections_t SShowDistance::getAutoConnections() const
{
    connections_t connections;
    connections.push(s_IMAGE, data::Image::DISTANCE_DISPLAYED_SIG, SHOW_DISTANCE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SShowDistance::showDistance(bool /*unused*/)
{
    const auto image          = m_image.lock();
    const auto show_distances = data::helper::MedicalImage::getDistanceVisibility(*image);

    this->sight::ui::action::setChecked(!(show_distances));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics
