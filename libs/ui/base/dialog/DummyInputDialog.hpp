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

#include "IInputDialog.hpp"

#include <queue>

namespace sight::ui::base::dialog
{

class UI_BASE_CLASS_API DummyInputDialog : public sight::ui::base::dialog::IInputDialog
{
public:

    SIGHT_DECLARE_CLASS(
        DummyInputDialog,
        sight::ui::base::dialog::IInputDialog,
        sight::ui::base::factory::New<DummyInputDialog>
    );

    UI_BASE_API explicit DummyInputDialog(sight::ui::base::GuiBaseObject::Key key);

    ~DummyInputDialog() override = default;
    void setTitle(const std::string& title) override;
    void setMessage(const std::string& msg) override;
    void setEchoMode(sight::ui::base::dialog::IInputDialog::EchoMode echoMode) override;
    void setInput(const std::string& text) override;

    std::pair<std::string, bool> getInput() override;

    UI_BASE_API static void pushInput(const std::string& input);

private:

    static std::queue<std::string> inputs;
};

}
