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

#include <ui/test/gui_test.hpp>

#include <utest_data/data.hpp>

#include <boost/dll/runtime_symbol_info.hpp>

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMetaObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QTemporaryDir>

#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <thread>

SIGHT_GUI_TEST_CASE("validates_names_from_the_loaded_profile")
{
    std::string message;
    try
    {
        ui.click("not_in_loaded_profile", {}, std::chrono::milliseconds(1));
    }
    catch(const std::invalid_argument& exception)
    {
        message = exception.what();
    }

    CHECK(message.find("unknown XML name") != std::string::npos);
    REQUIRE(message.find("not_in_loaded_profile") != std::string::npos);

    // A name in several views is ambiguous, and the failure names where each one is.
    ui.expect("left_view >> panel_button").visible();
    ui.expect("right_view >> panel_button").visible();
    std::string ambiguity;
    try
    {
        ui.click("panel_button", {}, std::chrono::seconds(5));
    }
    catch(const std::runtime_error& exception)
    {
        ambiguity = exception.what();
    }

    INFO(ambiguity);
    CHECK(ambiguity.find("several GUI objects named 'panel_button'") != std::string::npos);
    CHECK(ambiguity.find("left_view") != std::string::npos);
    CHECK(ambiguity.find("right_view") != std::string::npos);
}

//------------------------------------------------------------------------------

[[noreturn]] static void block_forever()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

//------------------------------------------------------------------------------

// Only blocks for the scenario below, which runs this one as a process of its own.
SIGHT_GUI_TEST_CASE("blocks_at_exit_when_asked")
{
    if(qEnvironmentVariableIsSet("SIGHT_GUI_TEST_BLOCK_AT_EXIT"))
    {
        REQUIRE(std::atexit(&block_forever) == 0);
    }
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("reports_a_process_that_does_not_exit")
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SIGHT_GUI_TEST_BLOCK_AT_EXIT", "1");
    environment.insert("SIGHT_GUI_TEST_EXIT_GRACE", "1");

    // Windows writes the threads of the process there.
    const QTemporaryDir dumps;
    REQUIRE(dumps.isValid());
    environment.insert("SIGHT_TEST_DUMP_DIR", dumps.path());

    // Not this test's own desktop: the run gets its own, and the process that watches it.
    environment.remove("SIGHT_GUI_TEST_DESKTOP");

    QProcess run;
    run.setProcessEnvironment(environment);
    run.start(
        QString::fromStdString(boost::dll::program_location().string()),
        {"--test-case=blocks_at_exit_when_asked", "--test-suite=ui_test_app"
        });
    REQUIRE(run.waitForStarted());
    constexpr int ctest_margin_ms = 280'000;
    REQUIRE(run.waitForFinished(ctest_margin_ms));

    const QByteArray errors = run.readAllStandardError();
    INFO(errors.toStdString());
    CHECK(errors.contains("scenario finished, process did not exit within 1 s"));
    CHECK(run.exitCode() != 0);
#ifdef WIN32
    CHECK(errors.contains("threads in "));
    CHECK(QDir(dumps.path()).entryList({"*_no_exit.dmp"}, QDir::Files).size() == 1);
#endif
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("reports_a_supervised_scenario_that_does_not_exit")
{
    // Two scenarios in one run: each runs in a process of its own, and the first never exits.
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SIGHT_GUI_TEST_BLOCK_AT_EXIT", "1");
    environment.insert("SIGHT_GUI_TEST_EXIT_GRACE", "1");

    const QTemporaryDir dumps;
    REQUIRE(dumps.isValid());
    environment.insert("SIGHT_TEST_DUMP_DIR", dumps.path());

    // A run of its own, not a scenario of this one.
    environment.remove("SIGHT_GUI_TEST_DESKTOP");
    environment.remove("SIGHT_GUI_TEST_CHILD");
    environment.remove("SIGHT_GUI_TEST_FINISHED_EVENT");

    QProcess run;
    run.setProcessEnvironment(environment);
    run.start(
        QString::fromStdString(boost::dll::program_location().string()),
        {"--test-case=blocks_at_exit_when_asked,fails_when_asked", "--test-suite=ui_test_app"
        });
    REQUIRE(run.waitForStarted());
    constexpr int ctest_margin_ms = 280'000;
    REQUIRE(run.waitForFinished(ctest_margin_ms));

    const QByteArray output = run.readAllStandardOutput();
    const QByteArray errors = run.readAllStandardError();
    INFO(output.toStdString());
    INFO(errors.toStdString());
    CHECK(errors.contains("scenario finished, process did not exit within 1 s"));
    CHECK(output.contains("running ui_test_app::fails_when_asked"));
    CHECK(run.exitCode() != 0);
}

//------------------------------------------------------------------------------

// Only saves a candidate for the scenario below, which runs this one as a process of its own.
SIGHT_GUI_TEST_CASE("saves_a_candidate_when_asked")
{
    if(qEnvironmentVariableIsSet("SIGHT_GUI_TEST_SAVE_A_CANDIDATE"))
    {
        ui.expect_image("scene_srv/view", sight::utest_data::dir() / "sight/ui/ui_test_app/unused.png");
    }
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("reports_a_failure_of_finish_with_its_artifacts")
{
    // In update mode, the scenario succeeds and finish() fails.
    const QTemporaryDir output;
    REQUIRE(output.isValid());
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SIGHT_GUI_TEST_SAVE_A_CANDIDATE", "1");
    environment.insert("SIGHT_UI_TEST_UPDATE_REFERENCES", "1");
    environment.insert("IMAGE_OUTPUT_PATH", output.path());
    environment.remove("SIGHT_GUI_TEST_DESKTOP");

    QProcess run;
    run.setProcessEnvironment(environment);
    run.start(
        QString::fromStdString(boost::dll::program_location().string()),
        {"--test-case=saves_a_candidate_when_asked", "--test-suite=ui_test_app"
        });
    REQUIRE(run.waitForStarted());
    constexpr int ctest_margin_ms = 280'000;
    REQUIRE(run.waitForFinished(ctest_margin_ms));

    const QByteArray errors = run.readAllStandardError();
    INFO(errors.toStdString());
    CHECK(run.exitCode() != 0);
    CHECK(errors.contains("reference update requested"));
    QDirIterator trees(output.path(), {"ui_tree.txt"}, QDir::Files, QDirIterator::Subdirectories);
    CHECK(trees.hasNext());
}

//------------------------------------------------------------------------------

// Only fails for the scenario below, which runs this one as a process of its own.
SIGHT_GUI_TEST_CASE("fails_when_asked")
{
    if(qEnvironmentVariableIsSet("SIGHT_GUI_TEST_FAIL"))
    {
        throw std::runtime_error("failed on purpose");
    }
}

//------------------------------------------------------------------------------

// Only for the scenario below: the GUI thread is busy once the application has started, as on a loaded machine.
static void block_after_start_when_asked()
{
    bool valid        = false;
    const int seconds = qEnvironmentVariableIntValue("SIGHT_GUI_TEST_BUSY_AFTER_START", &valid);
    if(valid && seconds > 0)
    {
        QMetaObject::invokeMethod(
            QCoreApplication::instance(),
            [seconds]{std::this_thread::sleep_for(std::chrono::seconds(seconds));},
            Qt::QueuedConnection
        );
    }
}

Q_COREAPP_STARTUP_FUNCTION(block_after_start_when_asked)

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("waits_for_an_application_busy_after_its_start")
{
    // Busy for twice the wait for the main window: only the time the GUI thread answers counts.
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SIGHT_UI_TEST_TIMEOUT_SCALE", "0.2");
    environment.insert("SIGHT_GUI_TEST_BUSY_AFTER_START", "12");
    environment.remove("SIGHT_GUI_TEST_FAIL");
    environment.remove("SIGHT_GUI_TEST_DESKTOP");

    QProcess run;
    run.setProcessEnvironment(environment);
    run.start(
        QString::fromStdString(boost::dll::program_location().string()),
        {"--test-case=fails_when_asked", "--test-suite=ui_test_app"
        });
    REQUIRE(run.waitForStarted());
    constexpr int ctest_margin_ms = 280'000;
    REQUIRE(run.waitForFinished(ctest_margin_ms));

    INFO(run.readAllStandardError().toStdString());
    CHECK(run.exitCode() == 0);
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("keeps_the_artifacts_of_a_failure_after_the_run")
{
    // Without IMAGE_OUTPUT_PATH, as a local run: the report names files to open once the run has ended.
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SIGHT_GUI_TEST_FAIL", "1");
    environment.remove("IMAGE_OUTPUT_PATH");
    environment.remove("SIGHT_GUI_TEST_DESKTOP");

    QProcess run;
    run.setProcessEnvironment(environment);
    run.start(
        QString::fromStdString(boost::dll::program_location().string()),
        {"--test-case=fails_when_asked", "--test-suite=ui_test_app"
        });
    REQUIRE(run.waitForStarted());
    constexpr int ctest_margin_ms = 280'000;
    REQUIRE(run.waitForFinished(ctest_margin_ms));

    const QString errors = QString::fromLocal8Bit(run.readAllStandardError());
    INFO(errors.toStdString());
    CHECK(run.exitCode() != 0);
    const QRegularExpressionMatch steps = QRegularExpression("artifacts: ([^,\\n]*steps\\.txt)").match(errors);
    REQUIRE(steps.hasMatch());
    const QFileInfo steps_file(steps.captured(1));
    CHECK(steps_file.isFile());
    CHECK(QDir(steps_file.absolutePath()).removeRecursively());
}
