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

    typedef std::string FactoryRegistryKeyType;

    typedef std::function<void ()> Stuff;

    typedef std::uint64_t MilliSecond;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    UI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    UI_API ~pulse_progress_base() override;
    UI_API pulse_progress_base();

    ///set the title for the dialog
    UI_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    UI_API virtual void setMessage(const std::string& msg) = 0;

    ///set the stuff
    UI_API virtual void setStuff(Stuff stuff);

    ///set the frequence
    UI_API virtual void setFrequence(MilliSecond frequence);

    ///show the pulse progress dialog
    UI_API virtual void show() = 0;

protected:

    Stuff m_stuff;
    MilliSecond m_frequence {1};
};

} // namespace sight::ui::dialog
