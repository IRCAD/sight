/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "stylesheet_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <boost/dll.hpp>

#include <QApplication>
#include <QFile>
#include <QTextStream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::stylesheet_test);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

inline static std::filesystem::path get_test_resources_path()
{
    static const auto s_ROOT           = boost::dll::this_line_location().parent_path().parent_path().string();
    static const auto s_RESOURCES_PATH = std::filesystem::canonical(
        std::filesystem::path(s_ROOT) / "share" / "sight" / "module_ui_qt_ut"
    );

    return s_RESOURCES_PATH;
}

//------------------------------------------------------------------------------

void stylesheet_test::setUp()
{
}

//------------------------------------------------------------------------------

void stylesheet_test::tearDown()
{
}

//------------------------------------------------------------------------------

void stylesheet_test::basic_test()
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
    CPPUNIT_ASSERT(module);
    CPPUNIT_ASSERT_NO_THROW(module->start());

    // Check if the stylesheet is applied
    const auto current_stylesheet = qApp->styleSheet();
    CPPUNIT_ASSERT(current_stylesheet != original_stylesheet);

    const auto test_qss_path = get_test_resources_path() / "test.qss";
    QFile test_qss_file(QString::fromStdString(test_qss_path.string()));
    CPPUNIT_ASSERT(test_qss_file.open(QFile::ReadOnly));

    const auto test_qss = QTextStream(&test_qss_file).readAll().trimmed();
    CPPUNIT_ASSERT(!test_qss.isEmpty());
    CPPUNIT_ASSERT(current_stylesheet.indexOf(test_qss) >= 0);
}

} // namespace sight::module::ui::qt::ut
