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

#include <ui/qt/action_callback.hpp>
#include <ui/qt/container/menu.hpp>
#include <ui/qt/container/menubar.hpp>
#include <ui/qt/container/toolbar.hpp>
#include <ui/qt/container/widget.hpp>
#include <ui/qt/layout/menu.hpp>
#include <ui/qt/layout/menubar.hpp>
#include <ui/qt/layout/object_name.hpp>
#include <ui/qt/layout/toolbar.hpp>
#include <ui/qt/layout/toolbox.hpp>
#include <ui/qt/naming.hpp>

#include <doctest/doctest.h>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

#include <memory>

namespace
{

constexpr auto XML_NAME = R"(Show/hide #1\mesh)";

//------------------------------------------------------------------------------

/// Checks real layout output, not just the escaping helper.
struct layout_fixture
{
    //------------------------------------------------------------------------------

    static sight::ui::config_t config(const char* const _tag)
    {
        sight::ui::config_t item;
        item.put("<xmlattr>.name", XML_NAME);
        sight::ui::config_t result;
        result.add_child(_tag, item);
        return result;
    }

    sight::ui::test::qt_application application {"ui_qt_test"};
    QWidget parent;
    std::shared_ptr<sight::ui::qt::action_callback> callback {std::make_shared<sight::ui::qt::action_callback>()};
};

} // namespace

TEST_SUITE("sight::ui::qt::layout::object_name")
{
    TEST_CASE("escapes_each_component_without_changing_other_characters")
    {
        using sight::ui::qt::layout::detail::composed_object_name;

        CHECK(composed_object_name(u"owner", u"plain name").toStdString() == "owner/plain name");
        CHECK(composed_object_name(u"owner", {}).toStdString() == "owner/");
        CHECK(composed_object_name(u"a/b#c\\d", u"x/y#z\\w").toStdString() == R"(a\/b\#c\\d/x\/y\#z\\w)");
        CHECK(composed_object_name(u"owner", u"é").toStdString() == "owner/\xc3\xa9");
        CHECK(composed_object_name(u"owner", u"a/b") != composed_object_name(u"owner", u"a\\/b"));
    }

    TEST_CASE_FIXTURE(layout_fixture, "toolbar_escapes_items_and_keeps_their_text")
    {
        QToolBar toolbar(&parent);
        auto container = sight::ui::qt::container::toolbar::make();
        container->set_qt_tool_bar(&toolbar);

        const auto layout  = std::make_shared<sight::ui::qt::layout::toolbar>();
        auto configuration = config("menu_item");
        configuration.add_child("menu", configuration.get_child("menu_item"));
        layout->initialize(configuration);
        layout->set_callbacks({callback});
        layout->create_layout(container, "toolbar");

        const QString expected = QStringLiteral(R"(toolbar/Show\/hide \#1\\mesh)");
        REQUIRE(toolbar.actions().size() == 2);
        CHECK(toolbar.actions().front()->objectName() == expected);
        CHECK(toolbar.actions().front()->text() == XML_NAME);
        const auto buttons = toolbar.findChildren<QToolButton*>(expected);
        REQUIRE(buttons.size() == 1);
        CHECK(buttons.front()->text() == XML_NAME);

        // Names read from the XML: the toolbar, its action and the button of its menu.
        using sight::ui::qt::detail::has_sanctioned_name;
        CHECK(has_sanctioned_name(toolbar));
        CHECK(has_sanctioned_name(*toolbar.actions().front()));
        CHECK(has_sanctioned_name(*buttons.front()));

        layout->destroy_layout();
    }

    TEST_CASE_FIXTURE(layout_fixture, "menu_escapes_items_and_keeps_their_text")
    {
        QMenu menu(&parent);
        auto container = sight::ui::qt::container::menu::make();
        container->set_qt_menu(&menu);

        const auto layout = std::make_shared<sight::ui::qt::layout::menu>();
        layout->initialize(config("menu_item"));
        layout->set_callbacks({callback});
        layout->create_layout(container, "menu");

        REQUIRE(menu.actions().size() == 1);
        CHECK(menu.actions().front()->objectName().toStdString() == R"(menu/Show\/hide \#1\\mesh)");
        CHECK(menu.actions().front()->text() == XML_NAME);
        CHECK(sight::ui::qt::detail::has_sanctioned_name(menu));
        CHECK(sight::ui::qt::detail::has_sanctioned_name(*menu.actions().front()));

        layout->destroy_layout();
    }

    TEST_CASE_FIXTURE(layout_fixture, "menubar_escapes_menus_and_keeps_their_title")
    {
        QMenuBar menubar(&parent);
        auto container = sight::ui::qt::container::menubar::make();
        container->set_qt_menu_bar(&menubar);

        const auto layout = std::make_shared<sight::ui::qt::layout::menubar>();
        layout->initialize(config("menu"));
        layout->create_layout(container, "menubar");

        const auto menus = menubar.findChildren<QMenu*>(QString(), Qt::FindDirectChildrenOnly);
        REQUIRE(menus.size() == 1);
        CHECK(menus.front()->objectName().toStdString() == R"(menubar/Show\/hide \#1\\mesh)");
        CHECK(menus.front()->title() == XML_NAME);
        CHECK(sight::ui::qt::detail::has_sanctioned_name(menubar));
        CHECK(sight::ui::qt::detail::has_sanctioned_name(*menus.front()));

        // The action of a menu is that menu: it has no name of its own.
        CHECK(menus.front()->menuAction()->objectName().isEmpty());

        layout->destroy_layout();
    }

    TEST_CASE_FIXTURE(layout_fixture, "toolbox_names_each_toggle_after_its_view")
    {
        auto container = sight::ui::qt::container::widget::make();
        container->set_qt_container(new QWidget(&parent));

        // A caption is not a name, and a section in a scroll area is named the same.
        sight::ui::config_t configuration;
        sight::ui::config_t plain;
        plain.put("<xmlattr>.caption", "Cropping settings");
        configuration.add_child("view", plain);
        sight::ui::config_t scrolled;
        scrolled.put("<xmlattr>.caption", "Advanced settings");
        scrolled.put("<xmlattr>.useScrollBar", "true");
        configuration.add_child("view", scrolled);

        const auto layout = std::make_shared<sight::ui::qt::layout::toolbox>();
        layout->initialize(configuration);
        layout->create_layout(container, "toolbox");

        QList<QPushButton*> toggles;
        for(auto* const button : container->get_qt_container()->findChildren<QPushButton*>())
        {
            if(button->property("class") == "toolbox_button")
            {
                toggles.push_back(button);
            }
        }

        REQUIRE(toggles.size() == 2);
        const auto views = layout->get_sub_views();
        REQUIRE(views.size() == 2);

        // No name before the view has one, as view::manage gives it.
        CHECK(toggles[0]->objectName().isEmpty());

        views[0]->set_name("cropping_tools_container");
        views[1]->set_name("volume_parameter_srv");
        CHECK(toggles[0]->objectName().toStdString() == "cropping_tools_container/toggle");
        CHECK(toggles[1]->objectName().toStdString() == "volume_parameter_srv/toggle");
        CHECK(toggles[0]->text() == "Cropping settings");
        CHECK(sight::ui::qt::detail::has_sanctioned_name(*toggles[0]));
        CHECK(sight::ui::qt::detail::has_sanctioned_name(*toggles[1]));

        // The toggle follows its view.
        views[0]->set_name("renamed_view");
        CHECK(toggles[0]->objectName().toStdString() == "renamed_view/toggle");
        CHECK(sight::ui::qt::detail::has_sanctioned_name(*toggles[0]));

        layout->destroy_layout();
        container->destroy_container();
    }
}
