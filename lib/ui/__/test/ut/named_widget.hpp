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

#pragma once

#include <ui/__/container/widget.hpp>

#include <string>

namespace sight::ui::ut
{

/// A container that records the name its registry gives it; nothing else is used.
class named_widget final : public ui::container::widget
{
public:

    //------------------------------------------------------------------------------

    void clean() override
    {
    }

    //------------------------------------------------------------------------------

    void destroy_container() override
    {
    }

    //------------------------------------------------------------------------------

    bool is_shown_on_screen() override
    {
        return false;
    }

    //------------------------------------------------------------------------------

    void set_visible(bool /*_is_visible*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_enabled(bool /*_is_enabled*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_blurred(bool /*_is_blurred*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_name(const std::string& _name) override
    {
        m_name = _name;
    }

    std::string m_name;
};

} // namespace sight::ui::ut
