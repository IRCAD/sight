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

#include "gui_test_support.hpp"

#include <ui/test/detail/locator.hpp>
#include <ui/test/locator.hpp>

#include <doctest/doctest.h>

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>

#include <cstddef>
#include <stdexcept>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::locator")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "resolves_visible_widgets_by_scope_and_role_occurrence")
    {
        QWidget window;
        window.setObjectName("view");
        window.resize(360, 200);

        QWidget service(&window);
        service.setObjectName("service");
        service.setGeometry(10, 10, 260, 140);

        QWidget first(&service);
        first.setObjectName("service/slider");
        first.setGeometry(10, 10, 80, 30);

        QWidget second(&service);
        second.setObjectName("service/slider");
        second.setGeometry(10, 60, 80, 30);

        QWidget outside_scope(&window);
        outside_scope.setObjectName("service/slider");
        outside_scope.setGeometry(290, 10, 60, 30);
        window.show();

        std::size_t scoped_count    = 0;
        bool role_selects_second    = false;
        bool hidden_role_is_missing = false;
        bool rejects_off_thread     = false;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));

                try
                {
                    static_cast<void>(locator("view").resolve_widgets());
                }
                catch(const std::logic_error&)
                {
                    rejects_off_thread = true;
                }

                scoped_count = gateway.call_until(
                    deadline,
                    [&]{return locator("view >> service >> service/slider").resolve_widgets().size();});
                role_selects_second = gateway.call_until(
                    deadline,
                    [&]
                {
                    const auto matches = locator("view >> service >> service/slider#2").resolve_widgets();
                    return matches.size() == 1 && matches.front() == &second;
                });
                hidden_role_is_missing = gateway.call_until(
                    deadline,
                    [&]
                {
                    second.hide();
                    return locator("view >> service >> service/slider#2").resolve_widgets().empty();
                });
            });

        CHECK(rejects_off_thread);
        CHECK(scoped_count == 2);
        CHECK(role_selects_second);
        CHECK(hidden_role_is_missing);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "resolves_a_covered_container_but_not_a_clipped_widget")
    {
        QWidget window;
        window.setObjectName("view");
        window.resize(360, 200);

        // A service whose opaque child fills it, as a render view or a scroll area's viewport does.
        QWidget service(&window);
        service.setObjectName("service");
        service.setGeometry(10, 10, 160, 100);
        QWidget content(&service);
        content.setAutoFillBackground(true);
        content.setGeometry(service.rect());

        // A collapsed section: its child exists and is shown, but nothing of it is left on screen.
        QWidget section(&window);
        section.setGeometry(200, 10, 120, 0);
        QWidget collapsed(&section);
        collapsed.setObjectName("collapsed");
        collapsed.setGeometry(0, 0, 120, 40);
        window.show();

        std::size_t service_matches   = 0;
        std::size_t collapsed_matches = 1;
        run_scenario(
            *this,
            [&]
            {
                const auto deadline = operation_deadline();
                REQUIRE(wait_for_exposure(*this, window, deadline));
                service_matches = gateway.call_until(
                    deadline,
                    [&]
                {
                    return locator("service").resolve_widgets().size();
                });
                collapsed_matches =
                    gateway.call_until(deadline, [&]{return locator("collapsed").resolve_widgets().size();});
            });

        CHECK(service_matches == 1);
        CHECK(collapsed_matches == 0);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "resolves_model_items_by_displayed_text_and_column")
    {
        QTreeWidget tree;
        tree.setColumnCount(2);

        auto* const root = new QTreeWidgetItem(&tree);
        root->setText(0, QStringLiteral("Root"));
        auto* const child = new QTreeWidgetItem(root);
        child->setText(0, QStringLiteral("Child/one"));
        auto* const duplicate = new QTreeWidgetItem(root);
        duplicate->setText(0, QStringLiteral("Child/one"));

        const auto matches = item("tree", R"(Root/Child\/one)").column(1).matching_indexes(tree);
        REQUIRE(matches.size() == 2);
        CHECK(matches[0].column() == 1);
        CHECK(matches[0].parent().data().toString() == QStringLiteral("Root"));
        CHECK(matches[0].row() == tree.indexFromItem(child).row());
        CHECK(matches[1].row() == tree.indexFromItem(duplicate).row());
        CHECK(item("tree", "Root/missing").matching_indexes(tree).empty());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::test::detail
