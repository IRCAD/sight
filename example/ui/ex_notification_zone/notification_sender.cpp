/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "notification_sender.hpp"

namespace ex_notification_zone
{

notification_sender::notification_sender() :
    has_notifications(has_signals::signals())
{
    this->new_slot(slots::INSTRUCTION, &notification_sender::instruction, this);
    this->new_slot(slots::INFORMATION, &notification_sender::information, this);
    this->new_slot(slots::WARNING, &notification_sender::warning, this);
    this->new_slot(slots::ERROR, &notification_sender::error, this);
    this->new_slot(slots::CLOSE_ERRORS, &notification_sender::close_errors, this);
}

//------------------------------------------------------------------------------

void notification_sender::instruction()
{
    static std::atomic_int count = 0;

    // Instructions are a singleton: a new one replaces the currently displayed one in place.
    this->instruct("Instruction #" + std::to_string(++count));
}

//------------------------------------------------------------------------------

void notification_sender::information()
{
    static std::atomic_int count = 0;

    this->inform("<html><h2>Information <b>#" + std::to_string(++count) + "</b></h2></html>");
}

//------------------------------------------------------------------------------

void notification_sender::warning()
{
    static std::atomic_int count = 0;

    this->warn("Warning #" + std::to_string(++count));
}

//------------------------------------------------------------------------------

void notification_sender::error()
{
    static std::atomic_int count = 0;

    // Errors never time out and must be acknowledged by the user.
    const auto error = this->fail("Error #" + std::to_string(++count));

    // Made cancelable so close_errors() can close it programmatically via cancel(), in addition to the
    // usual click-to-acknowledge in the notification zone.
    error->set_cancelable(true);
    m_errors.push_back(error);
}

//------------------------------------------------------------------------------

void notification_sender::close_errors()
{
    for(const auto& error : m_errors)
    {
        error->cancel();
    }

    m_errors.clear();
}

} // namespace ex_notification_zone
