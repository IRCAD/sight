/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

#include <vector>

namespace sight::ui::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   selector_base allows the choice of an element among several (_selections)
 */
class UI_CLASS_API selector_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(selector_base, ui::object);

    typedef std::string factory_registry_key_t;
    using choices_t        = std::vector<std::string>;
    using choices_preset_t = std::vector<std::pair<std::string, bool> >;
    using selections_t     = std::vector<std::string>;

    /// this *unique* key should  be used *for all* factory for specific selector(qt,wx,...)
    UI_API static const factory_registry_key_t REGISTRY_KEY;

    /// Constructor builds a selector proposing a string list
    UI_API selector_base() = default;

    /// Destructor. Do nothing.
    UI_API ~selector_base() override = default;

    ///  The string list that can be chosen by the selector.
    UI_API void set_choices(choices_t _choices);

    ///  The string list that can be chosen by the selector, which can be optionally preselected.
    UI_API virtual void set_choices_preset(choices_preset_t _choices) = 0;

    /// Sets the selector title.
    UI_API virtual void setTitle(std::string _title) = 0;

    /// Allows multiple selections (default = false).
    UI_API virtual void set_multiple(bool _multiple) = 0;

    ///  Show the selector and return the selection.
    UI_API virtual selections_t show() = 0;

    /// Set the message
    UI_API virtual void setMessage(const std::string& _msg) = 0;

    /// Add a custom button to this dialog
    UI_API virtual void addCustomButton(const std::string& _label, std::function<void()> _clicked_fn) = 0;
};

} // namespace sight::ui::dialog
