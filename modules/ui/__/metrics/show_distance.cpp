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

#include "modules/ui/__/metrics/show_distance.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>

#include <service/macros.hpp>

#include <exception>

namespace sight::module::ui::metrics
{

static const core::com::slots::key_t SHOW_DISTANCE_SLOT = "showDistance";

//------------------------------------------------------------------------------

show_distance::show_distance() noexcept
{
    new_slot(SHOW_DISTANCE_SLOT, &show_distance::show, this);
}

//------------------------------------------------------------------------------

show_distance::~show_distance() noexcept =
    default;

//------------------------------------------------------------------------------

void show_distance::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void show_distance::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void show_distance::updating()
{
    const auto image = m_image.lock();

    if(!data::helper::medical_image::check_image_validity(image.get_shared()))
    {
        this->sight::ui::action::set_checked(false);
    }
    else
    {
        const bool is_shown = data::helper::medical_image::get_distance_visibility(*image);

        const bool to_show = !is_shown;
        data::helper::medical_image::set_distance_visibility(*image, to_show);

        // Manage hide/show from the field information.
        this->sight::ui::action::set_checked(!to_show);

        const auto sig = image->signal<data::image::distance_displayed_signal_t>(
            data::image::DISTANCE_DISPLAYED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(this->slot(SHOW_DISTANCE_SLOT)));
            sig->async_emit(to_show);
        }
    }
}

//------------------------------------------------------------------------------

void show_distance::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

service::connections_t show_distance::auto_connections() const
{
    connections_t connections;
    connections.push(IMAGE, data::image::DISTANCE_DISPLAYED_SIG, SHOW_DISTANCE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void show_distance::show(bool /*unused*/)
{
    const auto image          = m_image.lock();
    const auto show_distances = data::helper::medical_image::get_distance_visibility(*image);

    this->sight::ui::action::set_checked(!(show_distances));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics
