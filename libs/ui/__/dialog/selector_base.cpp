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

#include "ui/__/dialog/selector_base.hpp"

namespace sight::ui::dialog
{

const selector_base::factory_registry_key_t selector_base::REGISTRY_KEY = "sight::ui::dialog::selector";

//------------------------------------------------------------------------------

void selector_base::set_choices(choices_t _choices)
{
    choices_preset_t choices;
    std::ranges::transform(
        _choices,
        std::back_inserter(choices),
        [](const auto& _x){return std::make_pair(_x, false);});
    set_choices_preset(choices);
}

} // namespace sight::ui::dialog
