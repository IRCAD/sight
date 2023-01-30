/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/location/ILocation.hpp>
#include <core/location/SingleFile.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>

namespace sight::ui::base::dialog
{

class DummyLocationDialog : public sight::ui::base::dialog::ILocationDialog
{
public:

    UI_BASE_API DummyLocationDialog(sight::ui::base::factory::Key key);

    void setType(Types type) override;
    sight::ui::base::dialog::ILocationDialog& setOption(Options option) override;
    void addFilter(const std::string& filterName, const std::string& wildcardList) override;
    std::string getCurrentSelection() const override;

    sight::core::location::ILocation::sptr show() override;

    UI_BASE_API static void setPaths(const std::vector<std::filesystem::path>& files);

private:

    sight::ui::base::dialog::ILocationDialog::Types m_type {sight::ui::base::dialog::ILocationDialog::SINGLE_FILE};

    static std::vector<std::filesystem::path> paths;
};

}
