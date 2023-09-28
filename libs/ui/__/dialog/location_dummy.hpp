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

    UI_API void setType(Types type) override;
    UI_API void setOption(location_dummy::Options option) override;
    UI_API void addFilter(const std::string& filterName, const std::string& wildcardList) override;
    UI_API std::string getCurrentSelection() const override;

    UI_API sight::core::location::base::sptr show() override;

    UI_API static void setPaths(const std::vector<std::filesystem::path>& files);

    UI_API static void pushPaths(const std::vector<std::filesystem::path>& files);

    UI_API static bool clear();

private:

    location_dummy::Types m_type {location_dummy::SINGLE_FILE};

    static std::queue<std::vector<std::filesystem::path> > pathsList;
};

}
