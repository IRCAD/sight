/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <core/location/single_file.hpp>

#include <ui/__/dialog/location_base.hpp>

#include <queue>

namespace sight::ui::dialog
{

class UI_CLASS_API location_dummy : public sight::ui::dialog::location_base
{
public:

    UI_API void set_type(types _type) override;
    UI_API void set_option(location_dummy::options _option) override;
    UI_API void add_filter(const std::string& _filter_name, const std::string& _wildcard_list) override;
    UI_API std::string get_current_selection() const override;

    UI_API sight::core::location::base::sptr show() override;

    UI_API static void set_paths(const std::vector<std::filesystem::path>& _files);

    UI_API static void push_paths(const std::vector<std::filesystem::path>& _files);

    UI_API static bool clear();

private:

    location_dummy::types m_type {location_dummy::single_file};

    static std::queue<std::vector<std::filesystem::path> > s_paths_list;
};

}
