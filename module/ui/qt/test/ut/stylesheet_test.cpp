/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <doctest/doctest.h>

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <boost/dll.hpp> // NOLINT(misc-include-cleaner)

#include <QApplication>
#include <QFile>
#include <QTextStream>

//------------------------------------------------------------------------------

inline static std::filesystem::path get_test_resources_path()
{
    static const auto s_ROOT           = boost::dll::this_line_location().parent_path().parent_path().string();
    static const auto s_RESOURCES_PATH = std::filesystem::canonical(
        std::filesystem::path(s_ROOT) / "share" / "sight" / "module_ui_qt_ut"
    );

    return s_RESOURCES_PATH;
}

TEST_SUITE("sight::module::ui::qt::stylesheet")
{
    TEST_CASE("basic")
    {
        // Backup the original stylesheet
        const auto original_stylesheet = qApp->styleSheet();

        // Necessary to start with a fresh qApp, as we need to use a fake profile path
        sight::core::runtime::unload_module("sight::module::ui::qt");

        // Use a fake profile path that will be used to load all .qss files within it
        auto profile = sight::core::runtime::get_current_profile();
        profile->set_file_path(get_test_resources_path() / "profile.xml");

        // Load the module
        auto module = sight::core::runtime::load_module("sight::module::ui::qt");
        CHECK(module);
        CHECK_NOTHROW(module->start());

        // Check if the stylesheet is applied
        const auto current_stylesheet = qApp->styleSheet();
        CHECK(current_stylesheet != original_stylesheet);

        const auto test_qss_path = get_test_resources_path() / "test.qss";
        QFile test_qss_file(QString::fromStdString(test_qss_path.string()));
        CHECK(test_qss_file.open(QFile::ReadOnly));

        const auto test_qss = QTextStream(&test_qss_file).readAll().trimmed();
        CHECK(!test_qss.isEmpty());
        CHECK(current_stylesheet.indexOf(test_qss) >= 0);
    }
}
