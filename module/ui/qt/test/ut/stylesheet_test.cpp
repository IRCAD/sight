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

#include <core/runtime/path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <boost/dll.hpp> // NOLINT(misc-include-cleaner)

#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QRegularExpression>
#include <QTextStream>
#include <QWidget>

#include <array>
#include <string>
#include <utility>

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

/// A theme of the module, read whole.
static QString theme(const std::string& _name)
{
    const auto path = sight::core::runtime::get_module_resource_file_path("sight::module::ui::qt/" + _name + ".qss");
    QFile file(QString::fromStdString(path.string()));
    REQUIRE(file.open(QFile::ReadOnly));
    return QTextStream(&file).readAll();
}

//------------------------------------------------------------------------------

/// The background a theme gives a label: of a style class, or named but of none.
static QColor background(const QString& _theme, const char* const _class, const QString& _name = QString())
{
    QWidget parent;
    parent.setStyleSheet(_theme);
    auto* const label = new QLabel(&parent);
    label->setObjectName(_name);
    if(_class != nullptr)
    {
        label->setProperty("class", QString::fromLatin1(_class));
    }

    label->ensurePolished();
    return label->palette().color(QPalette::Window);
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

    //------------------------------------------------------------------------------

    TEST_CASE("themes_select_no_object_name")
    {
        // Object names are the locators of tests: a theme selects types and style classes only.
        const QRegularExpression comment(
            QStringLiteral(R"(/\*.*?\*/)"),
            QRegularExpression::DotMatchesEverythingOption
        );
        const QRegularExpression selectors(QStringLiteral(R"(([^{}]*)\{[^}]*\})"));
        for(const std::string name : {"noctura", "flatdark"})
        {
            CAPTURE(name);
            const QString text = theme(name).remove(comment);
            for(auto match = selectors.globalMatch(text) ; match.hasNext() ; )
            {
                const QString selector = match.next().captured(1).trimmed();
                CAPTURE(selector.toStdString());
                CHECK_FALSE(selector.contains(u'#'));
            }
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("themes_style_notifications_by_class")
    {
        sight::core::runtime::load_module("sight::module::ui::qt");

        // The class of a notification gives its colour; the name it once had gives none any more.
        const std::array<std::pair<const char*, QColor>, 8> noctura {{
            {"notification_dialog_instruction", QColor("#51AF3B")},
            {"notification_dialog_error", QColor("#BA292A")},
            {"notification_dialog_information", QColor("#51566b")},
            {"notification_dialog_warning", QColor("#D98C22")},
            {"notification_label_instruction", QColor("#56608B")},
            {"notification_label_information", QColor("#51566b")},
            {"notification_label_warning", QColor("#D98C22")},
            {"notification_label_error", QColor("#BA292A")}
        }
        };
        const QString text = theme("noctura");
        for(const auto& [style_class, colour] : noctura)
        {
            CAPTURE(std::string(style_class));
            CHECK(background(text, style_class) == colour);
        }

        CHECK(background(text, nullptr, "NotificationDialog_Error") == background(text, nullptr));
        CHECK(background(text, nullptr, "notification_label_error") == background(text, nullptr));

        const QString flat = theme("flatdark");
        CHECK(background(flat, "notification_dialog_error") == QColor(255, 107, 104));
        CHECK(background(flat, nullptr, "NotificationDialog_Error") == background(flat, nullptr));
    }
}
