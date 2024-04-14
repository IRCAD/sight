/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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
// cspell:ignore qputenv

#include "plugin.hpp"

#include <QApplication>

namespace sight::module::ui::test
{

SIGHT_REGISTER_PLUGIN("sight::module::ui::test::plugin");

//------------------------------------------------------------------------------

void plugin::start() noexcept
{
    SIGHT_ASSERT("QApplication must be started for MacroSaver to infect it", qApp);
    qputenv("GUI_TESTS_ARE_RUNNING", "1");
    m_macro.infect(qApp);
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    m_macro.save();
}

} // namespace sight::module::ui::test
