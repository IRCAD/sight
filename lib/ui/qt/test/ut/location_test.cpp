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

#include <ui/test/qt_application.hpp>

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/preferences.hpp>
#include <ui/qt/dialog/location.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMetaObject>
#include <QScopeGuard>
#include <QTimer>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace
{

struct location_fixture
{
    sight::ui::test::qt_application application {"ui_qt_test"};
    sight::core::os::temp_dir temporary;
    std::filesystem::path directory {temporary / std::filesystem::path(u"folder_\u00e9_\u65e5")};
    bool driven {false};
    bool default_directory_preserved {false};
    bool native_dialogs_disabled {QApplication::testAttribute(Qt::AA_DontUseNativeDialogs)};

    location_fixture()
    {
        QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
        std::filesystem::create_directories(directory);
    }

    ~location_fixture()
    {
        QApplication::setAttribute(Qt::AA_DontUseNativeDialogs, native_dialogs_disabled);
    }

    //------------------------------------------------------------------------------

    sight::core::location::base::sptr show(
        sight::ui::dialog::location_base::types _type,
        const std::vector<std::filesystem::path>& _paths,
        bool _cancel = false
)
    {
        sight::ui::qt::dialog::location location;
        location.set_preferences_key(temporary.path().filename().string());
        auto initial = std::make_shared<sight::core::location::single_folder>();
        initial->set_folder(directory);
        location.set_default_location(initial);
        location.set_type(_type);
        location.set_option(sight::ui::dialog::location_base::read);

        // Drive the real dialog in its own modal loop; conversion, not input injection, is under test here.
        QTimer driver;
        driver.setInterval(10);
        QObject::connect(
            &driver,
            &QTimer::timeout,
            [&]
            {
                auto* dialog = qobject_cast<QFileDialog*>(QApplication::activeModalWidget());
                if(dialog == nullptr)
                {
                    return;
                }

                driver.stop();
                driven                      = true;
                default_directory_preserved = std::filesystem::equivalent(
                    std::filesystem::path(dialog->directory().absolutePath().toStdU16String()),
                    directory
                );
                if(_cancel)
                {
                    dialog->reject();
                    return;
                }

                if(_type == sight::ui::dialog::location_base::folder)
                {
                    dialog->selectFile(QString::fromStdU16String(_paths.front().u16string()));
                }
                else
                {
                    QString names;
                    for(const auto& path : _paths)
                    {
                        names += "\"" + QString::fromStdU16String(path.u16string()) + "\" ";
                    }

                    dialog->findChild<QLineEdit*>("fileNameEdit")->setText(names.trimmed());
                }

                QMetaObject::invokeMethod(dialog, "accept", Qt::QueuedConnection);
            });

        // A broken selection must fail the test, not leave it waiting for a click.
        QTimer watchdog;
        watchdog.setSingleShot(true);
        QObject::connect(
            &watchdog,
            &QTimer::timeout,
            []
            {
                if(auto* modal = qobject_cast<QFileDialog*>(QApplication::activeModalWidget()))
                {
                    modal->reject();
                }
            });
        driver.start();
        watchdog.start(30000);
        auto result = location.show();
        CHECK(driven);
        CHECK(default_directory_preserved);
        return result;
    }
};

} // namespace

TEST_SUITE("sight::ui::qt::dialog::location")
{
    TEST_CASE_FIXTURE(location_fixture, "preserves_unicode_files_and_default_directory")
    {
        const auto first  = directory / std::filesystem::path(u"first_\u65e5.txt");
        const auto second = directory / std::filesystem::path(u"second_\u00e9.txt");
        std::ofstream(first).put('1');
        std::ofstream(second).put('2');

        SUBCASE("one file")
        {
            const auto result = std::dynamic_pointer_cast<sight::core::location::single_file>(
                show(sight::ui::dialog::location_base::single_file, {first})
            );
            REQUIRE(result);
            CHECK(std::filesystem::equivalent(result->get_file(), first));
        }
        SUBCASE("multiple files")
        {
            const auto result = std::dynamic_pointer_cast<sight::core::location::multiple_files>(
                show(sight::ui::dialog::location_base::multi_files, {first, second})
            );
            REQUIRE(result);
            const auto files = result->get_files();
            REQUIRE(files.size() == 2);
            CHECK(std::filesystem::equivalent(files[0], first));
            CHECK(std::filesystem::equivalent(files[1], second));
        }
    }

    TEST_CASE_FIXTURE(location_fixture, "preserves_unicode_folder")
    {
        const auto result = std::dynamic_pointer_cast<sight::core::location::single_folder>(
            show(sight::ui::dialog::location_base::folder, {directory})
        );
        REQUIRE(result);
        CHECK(std::filesystem::equivalent(result->get_folder(), directory));
    }

    TEST_CASE_FIXTURE(location_fixture, "remembers_a_unicode_location")
    {
        // In memory only: the preferences of the machine are not those of the test.
        sight::core::runtime::init();
        sight::core::runtime::get_current_profile()->set_name(sight::core::tools::uuid::generate());
        sight::ui::preferences::set_enabled(true);
        sight::ui::preferences::ignore_filesystem(true);
        const auto restore = qScopeGuard(
            []
        {
            sight::ui::preferences::clear();
            sight::ui::preferences::set_enabled(false);
            sight::ui::preferences::ignore_filesystem(false);
        });

        const auto file_path = directory / std::filesystem::path(u"first_日_😀.txt");
        sight::ui::qt::dialog::location location;
        location.set_preferences_key(temporary.path().filename().string());

        SUBCASE("file")
        {
            auto file = std::make_shared<sight::core::location::single_file>();
            file->set_file(file_path);
            location.save_default_location(file);
            const auto remembered = std::dynamic_pointer_cast<sight::core::location::single_file>(
                location.get_default_location()
            );
            REQUIRE(remembered);
            CHECK(remembered->get_file() == file_path);
        }
        SUBCASE("folder")
        {
            auto folder = std::make_shared<sight::core::location::single_folder>();
            folder->set_folder(directory);
            location.save_default_location(folder);
            const auto remembered = std::dynamic_pointer_cast<sight::core::location::single_folder>(
                location.get_default_location()
            );
            REQUIRE(remembered);
            CHECK(remembered->get_folder() == directory);
        }
        SUBCASE("written in a code page by an older version")
        {
            const std::string key = sight::ui::dialog::location_base::SOFTWARE_UI + "."
                                    + temporary.path().filename().string() + "."
                                    + sight::ui::dialog::location_base::DLG_DEFAULT_FILE;
            sight::ui::preferences().put(key, std::string("\"caf\xE9.txt\""));
            const auto remembered = std::dynamic_pointer_cast<sight::core::location::single_file>(
                location.get_default_location()
            );
            REQUIRE(remembered);
            CHECK(remembered->get_file() == std::filesystem::path(std::string("caf\xE9.txt")));
        }
    }

    TEST_CASE_FIXTURE(location_fixture, "cancellation_returns_no_location")
    {
        CHECK_FALSE(show(sight::ui::dialog::location_base::single_file, {}, true));
    }
}
