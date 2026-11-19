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

#include "../ut/gui_test_support.hpp"

#include <ui/test/detail/locator.hpp>
#include <ui/test/locator.hpp>
#include <ui/test/scenario.hpp>

#include <doctest/doctest.h>

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <stdexcept>
#include <string>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::locator_builder")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "filters_by_type_and_by_text")
    {
        QWidget window;
        QVBoxLayout layout(&window);
        QLabel label("same", &window);
        QPushButton button("same", &window);
        QPushButton other("other", &window);
        label.setObjectName("named");
        button.setObjectName("named");
        other.setObjectName("named");
        layout.addWidget(&label);
        layout.addWidget(&button);
        layout.addWidget(&other);
        window.show();
        REQUIRE(QTest::qWaitForWindowExposed(&window, 30000));

        CHECK(locator("named").resolve_widgets().size() == 3);
        CHECK(locator(loc("named").type<QPushButton>()).resolve_widgets().size() == 2);
        CHECK(locator(loc("named").type<QAbstractButton>()).resolve_widgets().size() == 2);
        CHECK(locator(loc("named").text("same")).resolve_widgets().size() == 2);

        const auto both = locator(loc("named").type<QPushButton>().text("same")).resolve_widgets();
        REQUIRE(both.size() == 1);
        CHECK(both.front() == &button);

        // An ordinal counts what the filters keep.
        const auto second = locator("named#2 >> @type(QPushButton)").resolve_widgets();
        REQUIRE(second.size() == 1);
        CHECK(second.front() == &other);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_a_hidden_widget_only_when_asked")
    {
        QWidget window;
        QVBoxLayout layout(&window);
        QLabel shown("shown", &window);
        QLabel concealed("concealed", &window);
        shown.setObjectName("label");
        concealed.setObjectName("label");
        layout.addWidget(&shown);
        layout.addWidget(&concealed);
        concealed.hide();
        window.show();
        REQUIRE(QTest::qWaitForWindowExposed(&window, 30000));

        const auto hidden = locator(loc("label").hidden()).resolve_widgets();
        REQUIRE(hidden.size() == 1);
        CHECK(hidden.front() == &concealed);
        REQUIRE(locator("label").resolve_widgets().size() == 1);

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK(ui.read<QLabel>(loc("label").hidden(), &QLabel::text) == "concealed");
                ui.expect(loc("label").hidden()).text("concealed");
                ui.expect("label").text("shown");
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "looks_inside_the_innermost_modal_dialog")
    {
        QWidget window;
        QLabel behind("behind", &window);
        behind.setObjectName("field");
        window.show();
        REQUIRE(QTest::qWaitForWindowExposed(&window, 30000));

        // No modal dialog: nothing matches, whatever the rest says.
        CHECK(locator(dialog()).resolve_widgets().empty());
        CHECK(locator(dialog() >> "field").resolve_widgets().empty());

        QDialog outer(&window);
        QVBoxLayout outer_layout(&outer);
        QLabel in_outer("in outer", &outer);
        in_outer.setObjectName("field");
        outer_layout.addWidget(&in_outer);
        outer.setModal(true);
        outer.show();
        REQUIRE(wait_for_active_modal(&outer));
        REQUIRE(QTest::qWaitForWindowExposed(&outer, 30000));

        const auto first = locator(dialog() >> "field").resolve_widgets();
        REQUIRE(first.size() == 1);
        CHECK(first.front() == &in_outer);
        CHECK(locator(dialog()).resolve_widgets() == std::vector<QWidget*> {&outer});

        QDialog inner(&outer);
        QVBoxLayout inner_layout(&inner);
        QLabel in_inner("in inner", &inner);
        in_inner.setObjectName("field");
        inner_layout.addWidget(&in_inner);
        inner.setModal(true);
        inner.show();
        REQUIRE(wait_for_active_modal(&inner));
        REQUIRE(QTest::qWaitForWindowExposed(&inner, 30000));

        const auto second = locator(dialog() >> "field").resolve_widgets();
        REQUIRE(second.size() == 1);
        CHECK(second.front() == &in_inner);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "clicks_an_unnamed_button_of_a_qt_dialog")
    {
        QWidget window;
        window.show();
        QMessageBox box(QMessageBox::Question, "Question", "Sure?", QMessageBox::Yes | QMessageBox::No, &window);
        box.setModal(true);
        box.show();
        REQUIRE(wait_for_active_modal(&box));

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                ui.click(dialog().type<QPushButton>().text("&Yes"));
                ui.expect(dialog() >> "qt_msgbox_label").never_existed_ok().gone();
            });
        CHECK(box.result() == QMessageBox::Yes);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_to_guess_between_dialog_buttons_of_the_same_type")
    {
        QWidget window;
        window.show();
        QMessageBox box(QMessageBox::Question, "Question", "Sure?", QMessageBox::Yes | QMessageBox::No, &window);
        box.setModal(true);
        box.show();
        REQUIRE(wait_for_active_modal(&box));

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout){ui.click(dialog().type<QPushButton>(), {}, _timeout);},
                    [](const std::string& _message){return _message.find('2') != std::string::npos;});
                CHECK(failure.find("@type(QPushButton)") != std::string::npos);
                box.close();
            });
        CHECK(box.result() != QMessageBox::Yes);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "locates_an_item_by_its_rows")
    {
        QTreeWidget tree;
        tree.setObjectName("tree");
        tree.setColumnCount(2);
        auto* const first  = new QTreeWidgetItem(&tree, {"same", "a"});
        auto* const second = new QTreeWidgetItem(&tree, {"same", "b"});
        auto* const nested = new QTreeWidgetItem(second, {"child", "c"});
        tree.show();

        CHECK(item("tree", "same").matching_indexes(tree).size() == 2);

        const auto by_row = item_at("tree", "1").matching_indexes(tree);
        REQUIRE(by_row.size() == 1);
        CHECK(tree.itemFromIndex(by_row.front()) == second);

        const auto by_rows = item_at("tree", "1/0").column(1).matching_indexes(tree);
        REQUIRE(by_rows.size() == 1);
        CHECK(tree.itemFromIndex(by_rows.front()) == nested);
        CHECK(by_rows.front().column() == 1);

        CHECK(item_at("tree", "0").matching_indexes(tree).front().data().toString() == first->text(0));
        CHECK(item_at("tree", "2").matching_indexes(tree).empty());

        CHECK_THROWS_AS(std::ignore = item_at("tree", "01"), std::invalid_argument);
        CHECK_THROWS_AS(std::ignore = item_at("tree", "-1"), std::invalid_argument);
        CHECK_THROWS_AS(std::ignore = item_at("tree", "first"), std::invalid_argument);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "selects_an_item_by_its_rows")
    {
        QTreeWidget tree;
        tree.setObjectName("tree");
        auto* const parent = new QTreeWidgetItem(&tree, {"same"});
        new QTreeWidgetItem(&tree, {"same"});
        auto* const nested = new QTreeWidgetItem(parent, {"child"});
        tree.show();

        scenario ui(application);
        run_scenario(*this, [&]{ui.select(item_at("tree", "0/0"));});
        CHECK(tree.currentItem() == nested);
    }
}

} // namespace sight::ui::test::detail
