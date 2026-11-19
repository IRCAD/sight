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

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/qt/container/widget.hpp>
#include <ui/qt/namer.hpp>
#include <ui/qt/naming.hpp>
#include <ui/qt/widget/non_linear_slider.hpp>
#include <ui/qt/widget/progress_bar.hpp>
#include <ui/qt/widget/switch_button.hpp>
#include <ui/qt/widget/tickmarks_slider.hpp>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QTabBar>
#include <QTabWidget>
#include <QToolButton>
#include <QTreeWidget>

#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace sight::ui::qt::ut
{

/// Opens a dialog from its update, as a service does; only the name given to it while it runs is of interest.
// NOLINTNEXTLINE(misc-use-internal-linkage) The factory registers it under this name.
class dialog_service final : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(dialog_service, sight::service::base);

    QPointer<QDialog> m_dialog;
    QPointer<QLineEdit> m_field;
    QPointer<QFileDialog> m_file_dialog;

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        m_dialog = new QDialog();
        m_field  = new QLineEdit(m_dialog);
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_dialog);
        m_dialog->show();

        m_file_dialog = new QFileDialog();
        m_file_dialog->setOption(QFileDialog::DontUseNativeDialog);
        m_file_dialog->ensurePolished();
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
        delete m_dialog;
        delete m_file_dialog;
    }
};

SIGHT_REGISTER_SERVICE(sight::service::base, sight::ui::qt::ut::dialog_service);

namespace
{

/// A service container, as a <view sid="..."/> gives one, and the widgets of a test.
struct namer_fixture
{
    sight::ui::test::qt_application application {"ui_qt_test"};
    QWidget container;

    namer_fixture()
    {
        container.setObjectName("annotation_srv");
        name_widgets_below(container);
    }
};

} // namespace

//------------------------------------------------------------------------------

/// The name a widget has once polished.
template<typename Widget>
static std::string polished_name(Widget& _widget)
{
    _widget.ensurePolished();
    return _widget.objectName().toStdString();
}

TEST_SUITE("sight::ui::qt::namer")
{
    TEST_CASE_FIXTURE(namer_fixture, "names_a_widget_after_its_service_and_its_role")
    {
        CHECK(polished_name(*new QPushButton(&container)) == "annotation_srv/button");
        CHECK(polished_name(*new QToolButton(&container)) == "annotation_srv/button");
        CHECK(polished_name(*new QCheckBox(&container)) == "annotation_srv/check_box");
        CHECK(polished_name(*new QSlider(&container)) == "annotation_srv/slider");
        CHECK(polished_name(*new QLineEdit(&container)) == "annotation_srv/line_edit");
        CHECK(polished_name(*new QComboBox(&container)) == "annotation_srv/combo_box");
        CHECK(polished_name(*new QSpinBox(&container)) == "annotation_srv/spin_box");
        CHECK(polished_name(*new QTreeWidget(&container)) == "annotation_srv/tree");
        CHECK(polished_name(*new QListWidget(&container)) == "annotation_srv/list");
        CHECK(polished_name(*new QLabel(&container)) == "annotation_srv/label");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "gives_a_repeated_role_one_name")
    {
        // A locator picks one with an ordinal, annotation_srv/slider#2: a name would drift when a service rebuilds.
        CHECK(polished_name(*new QSlider(&container)) == "annotation_srv/slider");
        CHECK(polished_name(*new QSlider(&container)) == "annotation_srv/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "reaches_a_widget_through_unnamed_parents")
    {
        auto* const frame = new QWidget(&container);
        CHECK(polished_name(*frame).empty());
        CHECK(polished_name(*new QSlider(frame)) == "annotation_srv/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_widget_created_after_the_service_is_shown")
    {
        container.show();
        auto* const button = new QPushButton(&container);
        button->show();
        CHECK(button->objectName() == "annotation_srv/button");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_widget_polished_before_it_joined_its_service")
    {
        auto* const button = new QPushButton();
        CHECK(polished_name(*button).empty());

        button->setParent(&container);
        container.show();
        CHECK(button->objectName() == "annotation_srv/button");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "never_renames_a_widget")
    {
        auto* const button = new QPushButton(&container);
        button->setObjectName("annotation_srv/Delete");
        CHECK(polished_name(*button) == "annotation_srv/Delete");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "takes_the_nearest_service")
    {
        auto* const inner = new QWidget(&container);
        inner->setObjectName("inner_srv");
        name_widgets_below(*inner);
        CHECK(polished_name(*new QSlider(inner)) == "inner_srv/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_nothing_outside_a_service")
    {
        QWidget outside;
        CHECK(polished_name(*new QPushButton(&outside)).empty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "leaves_the_inner_widgets_of_a_control_unnamed")
    {
        auto* const spin_box = new QSpinBox(&container);
        spin_box->ensurePolished();
        // Qt names this one itself.
        CHECK(spin_box->findChild<QLineEdit*>()->objectName() == "qt_spinbox_lineedit");

        auto* const tree = new QTreeWidget(&container);
        tree->ensurePolished();
        CHECK(polished_name(*new QPushButton(tree->viewport())).empty());

        auto* const combo_box = new QComboBox(&container);
        combo_box->setEditable(true);
        combo_box->ensurePolished();
        CHECK(combo_box->lineEdit()->objectName().isEmpty());

        // The scroll bars of a list have no role of their own.
        auto* const list = new QListWidget(&container);
        list->ensurePolished();
        CHECK(list->verticalScrollBar()->objectName().isEmpty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "leaves_the_button_of_a_named_action_unnamed")
    {
        // The action bears the name: one element, one name.
        auto* const action = new QAction(&container);
        action->setObjectName("toolbar/volume");
        auto* const button = new QToolButton(&container);
        button->setDefaultAction(action);
        CHECK(polished_name(*button).empty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_shared_widget_and_its_parts_after_its_service")
    {
        auto* const button = new widget::switch_button(&container);
        CHECK(polished_name(*button) == "annotation_srv/switch_button");

        auto* const slider = new widget::tickmarks_slider(&container);
        CHECK(polished_name(*slider) == "annotation_srv/tickmarks_slider");
        CHECK(slider->findChild<QSlider*>()->objectName() == "annotation_srv/tickmarks_slider/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_the_slider_of_each_non_linear_slider_after_it")
    {
        // Two settings keys of one service: each inner slider is told apart by its owner.
        auto* const first  = new widget::non_linear_slider(&container);
        auto* const second = new widget::non_linear_slider(&container);
        detail::set_name(*first, "annotation_srv/first");
        detail::set_name(*second, "annotation_srv/second");
        container.show();
        CHECK(first->findChild<QSlider*>()->objectName() == "annotation_srv/first/slider");
        CHECK(second->findChild<QSlider*>()->objectName() == "annotation_srv/second/slider");
        CHECK(detail::has_sanctioned_name(*first->findChild<QSlider*>()));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_the_widgets_of_a_progress_bar_after_its_service")
    {
        // As in a notification zone: the bar lies in an unnamed page of the service.
        auto* const page = new QWidget(&container);
        const auto bar   = std::make_shared<widget::progress_bar>(page, true, false, false, false);
        container.show();
        const auto* const progress = bar->widget()->findChild<QProgressBar*>();
        REQUIRE(progress != nullptr);
        CHECK(progress->objectName() == "annotation_srv/progress_bar");
        CHECK(bar->widget()->objectName().isEmpty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "records_the_names_it_gives")
    {
        auto* const role = new QPushButton(&container);
        auto* const part = new QPushButton(&container);
        qt::name_part(*part, "new_group");
        QDialog dialog(&container);
        container.show();
        CHECK(polished_name(*role) == "annotation_srv/button");
        CHECK(detail::has_sanctioned_name(*role));
        CHECK(detail::has_sanctioned_name(*part));
        CHECK(polished_name(dialog) == "annotation_srv/dialog");
        CHECK(detail::has_sanctioned_name(dialog));

        // A name set by hand is kept, and stays unsanctioned.
        auto* const by_hand = new QPushButton("New Group");
        by_hand->setObjectName("annotation_srv/New Group");
        by_hand->setParent(&container);
        CHECK(polished_name(*by_hand) == "annotation_srv/New Group");
        CHECK_FALSE(detail::has_sanctioned_name(*by_hand));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "keeps_the_name_qt_gives_the_tab_bar_of_a_tab_widget")
    {
        // Why tab layouts give it a part: without one, it keeps Qt's name, which is none for a test.
        auto* const tabs = new QTabWidget(&container);
        container.show();
        CHECK(tabs->tabBar()->objectName() == "qt_tabwidget_tabbar");

        name_part(*tabs->tabBar(), "tab_bar");
        CHECK(tabs->tabBar()->objectName() == "annotation_srv/tab_bar");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "refuses_a_part_that_is_not_snake_case")
    {
        auto* const button = new QPushButton(&container);
        CHECK_THROWS_AS(qt::name_part(*button, "New Group"), std::invalid_argument);
        CHECK_THROWS_AS(qt::name_part(*button, "isCalibrated"), std::invalid_argument);
        CHECK_THROWS_AS(qt::name_part(*button, ""), std::invalid_argument);

        // Its role then, as if no part had been asked.
        CHECK(polished_name(*button) == "annotation_srv/button");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "ignores_a_part_property_set_by_hand")
    {
        auto* const button = new QPushButton(&container);
        button->setProperty("sight_part", QStringLiteral("New Group"));
        CHECK(polished_name(*button).empty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "gives_a_part_its_name")
    {
        auto* const before_polish = new QPushButton(&container);
        name_part(*before_polish, "new_group");
        CHECK(polished_name(*before_polish) == "annotation_srv/new_group");

        // A widget already named after its role takes the part.
        auto* const after_polish = new QPushButton(&container);
        CHECK(polished_name(*after_polish) == "annotation_srv/button");
        name_part(*after_polish, "remove");
        CHECK(after_polish->objectName() == "annotation_srv/remove");

        // Inside a control too: it is named on purpose.
        auto* const tree = new QTreeWidget(&container);
        auto* const cell = new QPushButton(tree->viewport());
        name_part(*cell, "cell");
        CHECK(cell->objectName() == "annotation_srv/cell");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_part_once_it_joins_its_service")
    {
        auto* const button = new QPushButton();
        name_part(*button, "new_group");
        CHECK(polished_name(*button).empty());

        button->setParent(&container);
        container.show();
        CHECK(button->objectName() == "annotation_srv/new_group");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_dialog_after_its_parents_without_a_running_service")
    {
        QDialog dialog(&container);
        CHECK(polished_name(dialog) == "annotation_srv/dialog");
        CHECK(polished_name(*new QLineEdit(&dialog)) == "annotation_srv/dialog/line_edit");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "leaves_a_dialog_of_nobody_unnamed")
    {
        QDialog dialog;
        auto* const button = new QPushButton(&dialog);
        dialog.ensurePolished();
        CHECK(dialog.objectName().isEmpty());
        CHECK(button->objectName().isEmpty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_standard_buttons_by_their_role")
    {
        QDialog dialog(&container);
        auto* const box = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
            &dialog
        );
        auto* const other = box->addButton("Other", QDialogButtonBox::ActionRole);

        // A button box polishes its buttons while it lays them out, before its dialog is polished.
        dialog.show();

        CHECK(box->button(QDialogButtonBox::Ok)->objectName() == "annotation_srv/dialog/ok");
        CHECK(box->button(QDialogButtonBox::Cancel)->objectName() == "annotation_srv/dialog/cancel");
        CHECK(box->button(QDialogButtonBox::Apply)->objectName() == "annotation_srv/dialog/apply");
        CHECK(other->objectName() == "annotation_srv/dialog/button");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_message_box_of_a_service")
    {
        QMessageBox box(QMessageBox::Question, "title", "text", QMessageBox::Yes | QMessageBox::No, &container);
        box.show();
        CHECK(box.objectName() == "annotation_srv/dialog");
        CHECK(box.button(QMessageBox::Yes)->objectName() == "annotation_srv/dialog/yes");
        CHECK(box.button(QMessageBox::No)->objectName() == "annotation_srv/dialog/no");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "gives_a_dialog_a_part")
    {
        QDialog dialog(&container);
        name_part(dialog, "settings");
        CHECK(polished_name(dialog) == "annotation_srv/settings");
        CHECK(polished_name(*new QSlider(&dialog)) == "annotation_srv/settings/slider");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "keeps_the_names_qt_gives_its_dialogs")
    {
        QFileDialog file_dialog(&container);
        file_dialog.setOption(QFileDialog::DontUseNativeDialog);
        file_dialog.ensurePolished();
        CHECK(file_dialog.objectName().toStdString().find("annotation_srv") == std::string::npos);
        CHECK(file_dialog.findChild<QLineEdit*>("fileNameEdit") != nullptr);
        CHECK(file_dialog.findChild<QComboBox*>("fileTypeCombo") != nullptr);
        for(const QWidget* const child : file_dialog.findChildren<QWidget*>())
        {
            CHECK(child->objectName().toStdString().find("annotation_srv") == std::string::npos);
        }

        QColorDialog color_dialog(&container);
        color_dialog.setOption(QColorDialog::DontUseNativeDialog);
        color_dialog.ensurePolished();
        for(const QWidget* const child : color_dialog.findChildren<QWidget*>())
        {
            CHECK(child->objectName().toStdString().find("annotation_srv") == std::string::npos);
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(namer_fixture, "names_a_dialog_after_the_service_that_opens_it")
    {
        const auto service = std::dynamic_pointer_cast<dialog_service>(
            sight::service::add("sight::ui::qt::ut::dialog_service", "preferences_srv")
        );
        REQUIRE(service != nullptr);

        // Without a worker it runs here: the thread of the GUI, as a GUI service does.
        service->set_worker(nullptr);
        service->configure();
        service->start().get();
        service->update().get();

        REQUIRE(service->m_dialog != nullptr);
        CHECK(service->m_dialog->objectName() == "preferences_srv/dialog");
        CHECK(service->m_field->objectName() == "preferences_srv/dialog/line_edit");

        const auto* const box = service->m_dialog->findChild<QDialogButtonBox*>();
        CHECK(box->button(QDialogButtonBox::Ok)->objectName() == "preferences_srv/dialog/ok");
        CHECK(box->button(QDialogButtonBox::Cancel)->objectName() == "preferences_srv/dialog/cancel");

        // Out of its update, nothing runs: a dialog then has no owner.
        QDialog later;
        CHECK(polished_name(later).empty());

        // A dialog of Qt keeps its names, even opened by a service.
        REQUIRE(service->m_file_dialog != nullptr);
        CHECK(service->m_file_dialog->objectName().toStdString().find("preferences_srv") == std::string::npos);

        service->stop().get();
        sight::service::remove(service);
    }

    //------------------------------------------------------------------------------

    TEST_CASE("names_widgets_below_a_container_after_its_xml_name")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        auto* const root     = new QWidget();
        const auto container = ui::qt::container::widget::make();
        container->set_qt_container(root);
        container->set_name("main_scene_view");
        CHECK(polished_name(*new QSlider(root)) == "main_scene_view/slider");
        container->destroy_container();
    }
}

} // namespace sight::ui::qt::ut
