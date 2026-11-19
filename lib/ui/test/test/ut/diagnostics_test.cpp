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

// cspell:ignore volum

#include "gui_test_support.hpp"

#include <ui/qt/container/widget.hpp>
#include <ui/qt/namer.hpp>
#include <ui/qt/naming.hpp>
#include <ui/test/scenario.hpp>

#include <QAction>
#include <QColorDialog>
#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace sight::ui::test::detail
{

using namespace std::chrono_literals;

/// Short on purpose: these tests are about what a deadline reports, once the target is known to be visible.
constexpr auto SHORT_DEADLINE = 200ms;

//------------------------------------------------------------------------------

static bool any_line_contains(const std::vector<std::string>& _lines, const std::string& _text)
{
    return std::ranges::any_of(
        _lines,
        [&_text](const std::string& _line)
        {
            return _line.find(_text) != std::string::npos;
        });
}

//------------------------------------------------------------------------------

static std::string file_content(const std::filesystem::path& _path)
{
    std::ifstream file(_path);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

//------------------------------------------------------------------------------

namespace
{

/// A folder for one test, emptied when it ends.
struct artifact_folder
{
    std::filesystem::path path {
        std::filesystem::temp_directory_path() / (
            "sight_ui_test_diagnostics_" + std::to_string(QCoreApplication::applicationPid())
        )
    };

    artifact_folder()
    {
        std::filesystem::remove_all(path);
    }

    artifact_folder(const artifact_folder&)            = delete;
    artifact_folder(artifact_folder&&)                 = delete;
    artifact_folder& operator=(const artifact_folder&) = delete;
    artifact_folder& operator=(artifact_folder&&)      = delete;

    ~artifact_folder()
    {
        std::error_code ignored;
        std::filesystem::remove_all(path, ignored);
    }
};

/// The main window of an application, named from its XML as the naming contract says.
struct application_window
{
    QMainWindow window;
    QAction* volume {new QAction("Volume", &window)};
    QAction* mesh {new QAction("Mesh", &window)};
    QWidget* service {new QWidget()};
    QLabel* label {new QLabel("67 / 133", service)};
    QSlider* first_slider {new QSlider(Qt::Horizontal, service)};
    QSlider* second_slider {new QSlider(Qt::Horizontal, service)};
    QPushButton* unnamed_button {new QPushButton("Go", service)};
    QPushButton* named_button {new QPushButton("Run", service)};

    application_window()
    {
        window.setWindowTitle("Viewer");

        auto* const tool_bar = window.addToolBar("tools");
        sight::ui::qt::detail::set_name(*tool_bar, "top_toolbar_left");
        sight::ui::qt::detail::set_name(*volume, "top_toolbar_left/volume");
        volume->setCheckable(true);
        tool_bar->addAction(volume);
        sight::ui::qt::detail::set_name(*mesh, "top_toolbar_left/mesh");
        tool_bar->addAction(mesh);

        sight::ui::qt::detail::set_name(*service, "annotation_srv");
        sight::ui::qt::detail::set_name(*label, "annotation_srv/label");
        sight::ui::qt::detail::set_name(*first_slider, "annotation_srv/slider");
        sight::ui::qt::detail::set_name(*second_slider, "annotation_srv/slider");
        sight::ui::qt::detail::set_name(*named_button, "annotation_srv/button");

        auto* const layout = new QVBoxLayout(service);
        layout->addWidget(label);
        layout->addWidget(first_slider);
        layout->addWidget(second_slider);
        layout->addWidget(unnamed_button);
        layout->addWidget(named_button);
        window.setCentralWidget(service);
        window.show();
    }
};

} // namespace

TEST_SUITE("sight::ui::test::diagnostics")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "logs_each_step_with_what_it_resolved_to")
    {
        application_window application_under_test;
        scenario ui(application);
        std::vector<std::string> steps;
        run_scenario(
            *this,
            [&]
            {
                ui.click("annotation_srv/button");
                ui.expect("annotation_srv/label").text("67 / 133");
                steps = ui.step_log();
            });

        CHECK(
            any_line_contains(
                steps,
                "step 1: click(\"annotation_srv/button\") -> annotation_srv >> annotation_srv/button,"
            )
        );
        CHECK(any_line_contains(steps, "step 2: expect(\"annotation_srv/label\"):"));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "logs_one_step_for_an_operation_that_uses_another")
    {
        application_window application_under_test;
        scenario ui(application);
        std::vector<std::string> steps;
        run_scenario(
            *this,
            [&]
            {
                [[maybe_unused]] auto watch = ui.watch_signal("annotation_srv/button", &QPushButton::clicked);
                steps                       = ui.step_log();
            });

        CHECK(any_line_contains(steps, "step 1: watch_signal(\"annotation_srv/button\")"));
        CHECK_FALSE(any_line_contains(steps, "step 2"));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "logs_what_a_step_waited_for")
    {
        application_window application_under_test;
        application_under_test.named_button->setEnabled(false);
        scenario ui(application);
        std::vector<std::string> steps;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/label").visible();
                const std::string failure = conclusive_failure(
                    [&ui](std::chrono::milliseconds _deadline){ui.click("annotation_srv/button", {}, _deadline);},
                    [](const std::string& _failure){return _failure.find("is disabled") != std::string::npos;});
                CHECK(failure.find("is disabled") != std::string::npos);
                steps = ui.step_log();
            });

        CHECK(any_line_contains(steps, "  waiting: widget 'annotation_srv/button' is disabled"));
        CHECK(any_line_contains(steps, ": click(\"annotation_srv/button\") failed after "));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_the_closest_names_of_a_missing_target")
    {
        application_window application_under_test;
        scenario ui(application);
        std::string near_failure;
        std::string far_failure;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/label").visible();
                near_failure = failure_of([&]{ui.click("top_toolbar_left/volum", {}, SHORT_DEADLINE);});

                far_failure = failure_of([&]{ui.expect("nothing like them").within(SHORT_DEADLINE).visible();});
            });

        CHECK(near_failure.find("no visible widget named 'top_toolbar_left/volum'") != std::string::npos);
        CHECK(
            near_failure.find("closest names: 'top_toolbar_left/volume', 'top_toolbar_left/mesh'")
            != std::string::npos
        );
        CHECK(far_failure.find("no widget named 'nothing like them'") != std::string::npos);
        CHECK(far_failure.find("closest names") == std::string::npos);
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_a_failed_step_with_its_artifacts")
    {
        application_window application_under_test;
        const artifact_folder folder;
        QDialog dialog(&application_under_test.window);
        scenario ui(application);
        std::string report;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/label").visible();
                const std::string what = conclusive_failure(
                    [&ui](std::chrono::milliseconds _deadline)
                {
                    ui.expect("annotation_srv/label").within(_deadline).text("4");
                },
                    [](const std::string& _failure){return _failure.find("last value") != std::string::npos;});
                REQUIRE(what.find("last value") != std::string::npos);
                gateway.call_until(operation_deadline(), [&dialog]{dialog.show(); return true;});
                REQUIRE(
                    wait_for(
                        *this,
                        operation_deadline(),
                        [&dialog]
                {
                    return dialog.isVisible() && dialog.windowHandle() != nullptr && dialog.windowHandle()->isExposed();
                })
                );
                report = ui.report_failure(folder.path, what);
            });

        CHECK(report.starts_with("step "));
        CHECK(report.find(": expect(\"annotation_srv/label\"): ") != std::string::npos);
        CHECK(report.find(" failed after ") != std::string::npos);
        CHECK(report.find("last value: \"67 / 133\"") != std::string::npos);
        CHECK(report.find("artifacts: ") != std::string::npos);

        CHECK_FALSE(QImage(QString::fromStdString((folder.path / "failure.png").string())).isNull());

        // The dialog on top is what a reader of the failure needs to see.
        CHECK_FALSE(QImage(QString::fromStdString((folder.path / "failure_1.png").string())).isNull());
        CHECK(file_content(folder.path / "ui_tree.txt").find("annotation_srv/label") != std::string::npos);
        CHECK(
            file_content(folder.path / "steps.txt").find(": expect(\"annotation_srv/label\"): ")
            != std::string::npos
        );
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_a_failure_outside_a_step")
    {
        application_window application_under_test;
        const artifact_folder folder;
        scenario ui(application);
        std::string report;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/label").visible();
                report = ui.report_failure(folder.path, "a check of the scenario failed");
            });

        CHECK(report.starts_with("after step 1: a check of the scenario failed"));
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "tells_when_the_artifacts_could_not_be_written")
    {
        application_window application_under_test;
        const artifact_folder folder;
        scenario ui(application);
        run_scenario(*this, [&]{ui.expect("annotation_srv/label").visible();});

        // The event loop has stopped: nothing on the GUI thread will answer, and the report must not wait for it.
        std::string report;
        std::thread other([&]{report = ui.report_failure(folder.path, "the application is gone");});
        other.join();
        CHECK(report.find("the application is gone") != std::string::npos);
        CHECK(report.find("artifacts not written: ") != std::string::npos);
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "dumps_the_visible_objects_with_their_locators")
    {
        application_window application_under_test;
        application_under_test.first_slider->setValue(40);
        scenario ui(application);
        std::string tree;
        int copied_slider_value = 0;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/label").visible();
                tree = ui.dump();

                // A name copied from the dump resolves.
                copied_slider_value = ui.read<QSlider>("annotation_srv/slider#1", &QSlider::value);
                ui.expect("annotation_srv/button").visible();
            });

        CHECK(tree.find("main window \"Viewer\"") != std::string::npos);
        CHECK(
            tree.find("action \"top_toolbar_left/volume\" checkable checked=false enabled=true")
            != std::string::npos
        );
        CHECK(tree.find("action \"top_toolbar_left/mesh\" enabled=true") != std::string::npos);
        CHECK(tree.find("QLabel \"annotation_srv/label\" text=\"67 / 133\"") != std::string::npos);
        CHECK(tree.find("QSlider \"annotation_srv/slider#1\" value=40   <- weak name") != std::string::npos);
        CHECK(tree.find("QSlider \"annotation_srv/slider#2\" value=0   <- weak name") != std::string::npos);
        CHECK(tree.find("QPushButton text=\"Go\"   <- no name: breaks the naming contract") != std::string::npos);
        CHECK(tree.find("QPushButton \"annotation_srv/button\" text=\"Run\"\n") != std::string::npos);
        CHECK(copied_slider_value == 40);
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "accepts_names_from_the_sanctioned_paths")
    {
        QWidget window;
        auto* const layout = new QVBoxLayout(&window);

        // A menu bar of the XML: a menu is reached through its action, which bears no name of its own.
        auto* const menu_bar = new QMenuBar(&window);
        sight::ui::qt::detail::set_name(*menu_bar, "menu_bar_srv");
        sight::ui::qt::detail::set_name(*menu_bar->addMenu("File"), "menu_bar_srv/File");
        layout->addWidget(menu_bar);

        // A service root and a sub-view, named by their containers, with roles and parts named by the namer.
        auto* const service_root   = new QWidget(&window);
        auto* const service_layout = new QVBoxLayout(service_root);
        layout->addWidget(service_root);
        const auto service_container = sight::ui::qt::container::widget::make();
        service_container->set_qt_container(service_root);
        service_container->set_name("annotation_srv");
        auto* const role_button = new QPushButton("Run", service_root);
        auto* const part_button = new QPushButton("Save", service_root);
        sight::ui::qt::name_part(*part_button, "save");
        service_layout->addWidget(role_button);
        service_layout->addWidget(new QLabel("ready", service_root));
        service_layout->addWidget(part_button);

        // A part of a shared widget follows the renames of its owner.
        auto* const owner = new QWidget(service_root);
        auto* const bound = new QPushButton("Toggle", owner);
        (new QVBoxLayout(owner))->addWidget(bound);
        sight::ui::qt::detail::set_name(*owner, "annotation_srv/shared");
        sight::ui::qt::detail::bind_part_name(*bound, *owner, u"toggle");
        sight::ui::qt::detail::set_name(*owner, "annotation_srv/renamed");
        service_layout->addWidget(owner);

        // A settings key, as the settings service names its widgets.
        auto* const setting = new QLineEdit(service_root);
        sight::ui::qt::detail::set_name(*setting, "settings_srv/nonlin");
        service_layout->addWidget(setting);

        auto* const view_root = new QWidget(service_root);
        service_layout->addWidget(view_root);
        const auto view_container = sight::ui::qt::container::widget::make();
        view_container->set_qt_container(view_root);
        view_container->set_name("axial_scene_view");
        auto* const view_button = new QPushButton("View", view_root);
        (new QVBoxLayout(view_root))->addWidget(view_button);

        // A dialog of the service, with its standard buttons.
        QDialog dialog(service_root);
        auto* const buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        (new QVBoxLayout(&dialog))->addWidget(buttons);
        window.show();
        dialog.show();

        scenario ui(application);
        std::vector<std::string> violations;
        std::string tree;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/button").visible();
                ui.expect("annotation_srv/label").visible();
                ui.expect("annotation_srv/save").visible();
                ui.expect("annotation_srv/renamed/toggle").visible();
                ui.expect("settings_srv/nonlin").visible();
                ui.expect("axial_scene_view/button").visible();
                ui.expect("annotation_srv/dialog/ok").visible();
                violations = ui.naming_violations();
                tree       = ui.dump();
                ui.expect_valid_names();
            });

        dialog.close();
        view_container->destroy_container();
        service_container->destroy_container();
        INFO(tree);
        CHECK(violations.empty());
        CHECK(tree.find("breaks the naming contract") == std::string::npos);
        CHECK(tree.find("menu \"menu_bar_srv/File\" enabled=true") != std::string::npos);
        CHECK(role_button->objectName() == "annotation_srv/button");
        CHECK(view_button->objectName() == "axial_scene_view/button");
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reports_each_object_breaking_the_contract")
    {
        QWidget window;
        auto* const layout         = new QVBoxLayout(&window);
        auto* const service_root   = new QWidget(&window);
        auto* const service_layout = new QVBoxLayout(service_root);
        layout->addWidget(service_root);
        const auto service_container = sight::ui::qt::container::widget::make();
        service_container->set_qt_container(service_root);
        service_container->set_name("annotation_srv");

        // Names a service gives by hand: a caption, its own uid, a caption of a label.
        const auto add = [&](QWidget* _widget, const QString& _name)
                         {
                             _widget->setObjectName(_name);
                             service_layout->addWidget(_widget);
                         };
        add(new QPushButton("Confirm", service_root), "annotation_srv/Confirm");
        add(new QPushButton("Inner", service_root), "annotation_srv");
        add(new QLabel("Caption", service_root), "annotation_srv/Caption");

        // A caption that repeats: weak and explicit at once.
        add(new QSlider(Qt::Horizontal, service_root), "annotation_srv/Volume");
        add(new QSlider(Qt::Horizontal, service_root), "annotation_srv/Volume");

        // A sanctioned name changed afterwards, and a malformed part the API refused.
        auto* const renamed = new QPushButton("Renamed", service_root);
        sight::ui::qt::detail::set_name(*renamed, "annotation_srv/stable_part");
        add(renamed, "annotation_srv/changed");
        auto* const malformed = new QPushButton("Malformed", service_root);
        CHECK_THROWS_AS(sight::ui::qt::name_part(*malformed, "Bad Part"), std::invalid_argument);
        add(malformed, "annotation_srv/Bad Part");

        // Outside any service: an unnamed button breaks the contract, an unnamed label does not.
        auto* const outside        = new QWidget(&window);
        auto* const outside_layout = new QVBoxLayout(outside);
        outside_layout->addWidget(new QPushButton("Unnamed", outside));
        outside_layout->addWidget(new QLabel("Unnamed label", outside));
        layout->addWidget(outside);

        // Actions and menus named by hand or not at all.
        auto* const tool_bar = new QToolBar(&window);
        sight::ui::qt::detail::set_name(*tool_bar, "toolbar_srv");
        tool_bar->addAction("By hand")->setObjectName("toolbar_srv/By hand");
        tool_bar->addAction("Unnamed action");
        layout->addWidget(tool_bar);
        auto* const menu_bar = new QMenuBar(&window);
        sight::ui::qt::detail::set_name(*menu_bar, "menu_bar_srv");
        menu_bar->addMenu("Unnamed menu");
        layout->addWidget(menu_bar);

        // A dialog nobody opened: it has no owner to be named after.
        QDialog orphan;
        orphan.setWindowTitle("Orphan");
        window.show();
        orphan.show();

        scenario ui(application);
        std::vector<std::string> violations;
        std::string tree;
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("annotation_srv/Confirm").visible();
                violations = ui.naming_violations();
                tree       = ui.dump();
                failure    = failure_of([&]{ui.expect_valid_names();});
            });

        orphan.close();
        service_container->destroy_container();
        INFO(tree);
        const std::vector<std::string> expected {
            R"(explicit name: QPushButton "annotation_srv/Confirm" text="Confirm" in QWidget > annotation_srv)",
            R"(explicit name: QPushButton "annotation_srv" text="Inner")",
            R"(explicit name: QLabel "annotation_srv/Caption" text="Caption")",
            R"(explicit name: QSlider "annotation_srv/Volume" value=0)",
            R"(explicit name: QSlider "annotation_srv/Volume" value=0)",
            R"(explicit name: QPushButton "annotation_srv/changed" text="Renamed")",
            R"(explicit name: QPushButton "annotation_srv/Bad Part" text="Malformed")",
            R"(no name: QPushButton text="Unnamed" in QWidget > QWidget)",
            R"(explicit name: QAction "toolbar_srv/By hand" text="By hand" in QWidget > toolbar_srv)",
            R"(no name: QAction text="Unnamed action" in QWidget > toolbar_srv)",
            R"(no name: QMenu in QWidget > menu_bar_srv)",
            R"(no name: QDialog in no parent)"
        };
        CHECK(violations.size() == expected.size());
        for(const std::string& line : expected)
        {
            CAPTURE(line);
            CHECK(any_line_contains(violations, line));
        }

        // The dump marks the same objects, and the step fails with all of them.
        CHECK(tree.find(R"("annotation_srv/Confirm" text="Confirm"   <- explicit name)") != std::string::npos);
        CHECK(tree.find(R"(dialog "Orphan"   <- no name: breaks the naming contract)") != std::string::npos);
        CHECK(tree.find("weak name: add a part if a test needs it; explicit name") != std::string::npos);
        CHECK(failure.starts_with(std::to_string(expected.size()) + " object(s) break the naming contract:"));
        CHECK(failure.find(expected.front()) != std::string::npos);
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "dumps_a_repeated_name_with_the_scope_that_tells_it_apart")
    {
        QWidget window;
        auto* const axial    = new QWidget(&window);
        auto* const frontal  = new QWidget(&window);
        auto* const layout   = new QVBoxLayout(&window);
        auto* const in_axial = new QSlider(axial);
        sight::ui::qt::detail::set_name(*axial, "axial_scene_view");
        sight::ui::qt::detail::set_name(*frontal, "frontal_scene_view");
        sight::ui::qt::detail::set_name(*in_axial, "negato_slicer_srv/slider");
        sight::ui::qt::detail::set_name(*new QSlider(frontal), "negato_slicer_srv/slider");
        axial->setMinimumSize(100, 50);
        frontal->setMinimumSize(100, 50);
        layout->addWidget(axial);
        layout->addWidget(frontal);
        window.show();

        scenario ui(application);
        std::string tree;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("axial_scene_view").visible();
                tree = ui.dump();
                ui.expect("axial_scene_view >> negato_slicer_srv/slider").visible();
            });

        CHECK(tree.find("QSlider \"axial_scene_view >> negato_slicer_srv/slider\"") != std::string::npos);
        CHECK(tree.find("QSlider \"frontal_scene_view >> negato_slicer_srv/slider\"") != std::string::npos);
        CHECK(tree.find("weak name") == std::string::npos);
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "holds_qt_dialogs_out_of_the_naming_contract")
    {
        QFileDialog dialog;
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setObjectName("file_dialog");
        QColorDialog color_dialog;
        color_dialog.setOption(QColorDialog::DontUseNativeDialog);
        color_dialog.setObjectName("color_dialog");
        QFontDialog font_dialog;
        font_dialog.setOption(QFontDialog::DontUseNativeDialog);
        font_dialog.setObjectName("font_dialog");
        dialog.show();
        color_dialog.show();
        font_dialog.show();

        scenario ui(application);
        std::string tree;
        std::vector<std::string> violations;
        run_scenario(
            *this,
            [&]
            {
                ui.expect("file_dialog").visible();
                tree       = ui.dump();
                violations = ui.naming_violations();
            });

        CHECK(tree.find("dialog") != std::string::npos);
        CHECK(tree.find("breaks the naming contract") == std::string::npos);
        CHECK(violations.empty());
    }

    //------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_dump_from_the_gui_thread")
    {
        scenario ui(application);
        CHECK_THROWS_AS(ui.dump(), std::logic_error);
    }
}

} // namespace sight::ui::test::detail
