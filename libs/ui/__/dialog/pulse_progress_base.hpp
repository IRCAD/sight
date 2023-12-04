/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

#include <functional>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic Progress dialog for IHM.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class UI_CLASS_API pulse_progress_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(pulse_progress_base, ui::object);

    using factory_registry_key_t = std::string;

    using Stuff = std::function<void ()>;

    using MilliSecond = std::uint64_t;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    UI_API static const factory_registry_key_t REGISTRY_KEY;

    UI_API ~pulse_progress_base() override = default;
    UI_API pulse_progress_base()           = default;

    ///set the title for the dialog
    UI_API virtual void set_title(const std::string& _title) = 0;

    ///set the message for the dialog
    UI_API virtual void set_message(const std::string& _msg) = 0;

    ///set the stuff
    UI_API virtual void set_stuff(Stuff _stuff);

    ///set the frequence
    UI_API virtual void set_frequence(MilliSecond _frequence);

    ///allow to cancel the dialog
    UI_API virtual void set_cancellable(bool _cancellable);

    ///show the pulse progress dialog
    UI_API virtual void show() = 0;

protected:

    Stuff m_stuff;
    MilliSecond m_frequence {1};
    bool m_cancellable {true};
};

} // namespace sight::ui::dialog
