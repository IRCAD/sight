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

#include <ui/test/detail/file_dialog.hpp>
#include <ui/test/detail/image.hpp>
#include <ui/test/scenario.hpp>

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <QAbstractItemView>
#include <QBuffer>
#include <QByteArray>
#include <QColor>
#include <QCompleter>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QIODevice>
#include <QImage>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

namespace sight::ui::test::detail
{

namespace
{

class vetoed_file_dialog final : public QFileDialog
{
public:

    int accepts {0};

protected:

    //------------------------------------------------------------------------------

    void accept() override
    {
        ++accepts;
    }
};

/// Opens another modal dialog instead of accepting.
class interrupted_file_dialog final : public QFileDialog
{
public:

    explicit interrupted_file_dialog(QDialog& _intruder) :
        m_intruder(_intruder)
    {
    }

protected:

    //------------------------------------------------------------------------------

    void accept() override
    {
        m_intruder.show();
    }

private:

    QDialog& m_intruder;
};

/// Opens a popup once, as the file dialog shows, and keeps it open: it holds the keyboard and the mouse.
class popup_on_dialog final : public QObject
{
public:

    int popups {0};
    QMenu popup;

    popup_on_dialog()
    {
        popup.addAction("Other");
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _object, QEvent* _event) override
    {
        if(_event->type() == QEvent::Show && qobject_cast<QFileDialog*>(_object) != nullptr && popups == 0)
        {
            ++popups;
            QTimer::singleShot(0, &popup, [this]{popup.popup(QPoint(0, 0));});
        }

        return false;
    }
};

class question_monitor final : public QObject
{
public:

    bool shown {false};

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _object, QEvent* _event) override
    {
        shown = shown || (_event->type() == QEvent::Show && qobject_cast<QMessageBox*>(_object) != nullptr);
        return false;
    }
};

struct file_window
{
    core::os::temp_dir temporary;
    std::filesystem::path folder {temporary / std::filesystem::path(u"folder_\u65e5_\U0001f600")};
    std::filesystem::path path {folder / std::filesystem::path(u"image_\u00e9.txt")};
    QWidget window;
    QPushButton open {"Open", &window};
    QStringList selected;
    QString selected_filter;
    int result {-1};
    QFileDialog::AcceptMode accept_mode {QFileDialog::AcceptOpen};
    QFileDialog::FileMode file_mode {QFileDialog::ExistingFile};

    file_window()
    {
        std::filesystem::create_directories(folder);
        window.resize(320, 100);
        open.setObjectName("open");
        auto* const layout = new QVBoxLayout(&window);
        layout->addWidget(&open);
        QObject::connect(
            &open,
            &QPushButton::clicked,
            [&]
                {
                    QFileDialog dialog(&window);
                    dialog.setOption(QFileDialog::DontUseNativeDialog);
                    dialog.setAcceptMode(accept_mode);
                    dialog.setFileMode(file_mode);
                    dialog.setDirectory(QString::fromStdU16String(temporary.path().u16string()));
                    dialog.setNameFilters({"All (*)", "Text (*.txt)"});
                    result          = dialog.exec();
                    selected        = dialog.selectedFiles();
                    selected_filter = dialog.selectedNameFilter();
                });
        window.show();
    }
};

} // namespace

TEST_SUITE("sight::ui::test::file_dialog")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_replay_acceptance_when_the_dialog_stays_open")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / "existing.txt";
        std::ofstream(path).put('x');
        vetoed_file_dialog dialog;
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.show();
        std::array<scenario, 4> attempts {
            scenario(application), scenario(application), scenario(application), scenario(application)
        };
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                std::size_t next   = 0;
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    // A timed-out attempt is a failed scenario, not the scenario used for cleanup.
                    attempts.at(next++).choose_file(path, {}, _timeout);
                },
                    [&](const std::string&)
                {
                    return gateway.call_until(operation_deadline(), [&]{return dialog.accepts > 0;});
                });
                CHECK(failure != "no timeout");
                CHECK(gateway.call_until(operation_deadline(), [&]{return dialog.accepts;}) == 1);
                actions.cancel_file_dialog();
                actions.finish();
            });
        CHECK(dialog.result() == QDialog::Rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "tells_a_dialog_did_not_close_when_the_gui_is_busy_at_the_deadline")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / "existing.txt";
        std::ofstream(path).put('x');
        vetoed_file_dialog dialog;
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.show();
        scenario attempt(application);
        scenario actions(application);
        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                // A loaded machine may take seconds to show the dialog: the deadline under test starts after.
                REQUIRE(wait_for_exposure(*this, dialog, operation_deadline()));

                // Once the dialog refused its acceptance, the GUI stays busy past the deadline.
                const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
                std::thread busy(
                    [&]
                {
                    while(std::chrono::steady_clock::now() < deadline
                          && gateway.call_until(operation_deadline(), [&]{return dialog.accepts;}) == 0)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }

                    QMetaObject::invokeMethod(
                        &application,
                        [deadline]{std::this_thread::sleep_until(deadline + std::chrono::seconds(1));},
                        Qt::QueuedConnection
                    );
                });
                failure = failure_of([&]{attempt.choose_file(path, {}, deadline - std::chrono::steady_clock::now());});

                busy.join();
                actions.cancel_file_dialog();
                actions.finish();
            });

        INFO(failure);
        CHECK(failure.find("did not close") != std::string::npos);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "chooses_real_dialog_paths_in_every_mode")
    {
        file_window file;
        bool folder       = false;
        bool cancel       = false;
        bool overwrite    = false;
        bool multiple     = false;
        const auto second = file.folder / "second.txt";
        SUBCASE("load existing Unicode file")
        {
            std::ofstream(file.path).put('x');
        }
        SUBCASE("save new Unicode file")
        {
            file.accept_mode = QFileDialog::AcceptSave;
            file.file_mode   = QFileDialog::AnyFile;
        }
        SUBCASE("load multiple files")
        {
            file.file_mode = QFileDialog::ExistingFiles;
            std::ofstream(file.path).put('x');
            std::ofstream(second).put('x');
            multiple = true;
        }
        SUBCASE("save and confirm overwrite")
        {
            file.accept_mode = QFileDialog::AcceptSave;
            file.file_mode   = QFileDialog::AnyFile;
            std::ofstream(file.path).put('x');
            overwrite = true;
        }
        SUBCASE("choose Unicode folder")
        {
            file.file_mode = QFileDialog::Directory;
            folder         = true;
        }
        SUBCASE("cancel")
        {
            cancel = true;
        }
        SUBCASE("choose from large asynchronously loaded directory")
        {
            for(int index = 0 ; index < 3000 ; ++index)
            {
                std::ofstream(file.folder / (std::to_string(index) + ".txt")).put('x');
            }

            std::ofstream(file.path).put('x');
        }

        question_monitor monitor;
        application.installEventFilter(&monitor);
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("open");
                if(cancel)
                {
                    actions.cancel_file_dialog();
                }
                else if(folder)
                {
                    actions.choose_folder(file.folder);
                }
                else if(multiple)
                {
                    actions.choose_files({file.path, second}, "Text (*.txt)");
                }
                else
                {
                    actions.choose_file(file.path, "Text (*.txt)");
                }

                actions.finish();
            });
        application.removeEventFilter(&monitor);
        CHECK(file.result == (cancel ? QDialog::Rejected : QDialog::Accepted));
        CHECK(monitor.shown == overwrite);
        if(!cancel)
        {
            REQUIRE(file.selected.size() == (multiple ? 2 : 1));
            CHECK(std::filesystem::path(file.selected.front().toStdU16String()) == (folder ? file.folder : file.path));
            if(multiple)
            {
                CHECK(std::filesystem::path(file.selected.back().toStdU16String()) == second);
            }

            if(!folder)
            {
                CHECK(file.selected_filter == "Text (*.txt)");
            }
        }
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "chooses_a_file_however_short_its_timeout")
    {
        file_window file;
        std::ofstream(file.path).put('x');
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("open");

                // The GUI answers after the timeout: the dialog is still found and accepted.
                QMetaObject::invokeMethod(
                    &application,
                    []{std::this_thread::sleep_for(std::chrono::milliseconds(200));},
                    Qt::QueuedConnection
                );
                actions.choose_file(file.path, "Text (*.txt)", std::chrono::milliseconds(1));
                actions.finish();
            });
        CHECK(file.result == QDialog::Accepted);
        REQUIRE(file.selected.size() == 1);
        CHECK(std::filesystem::path(file.selected.front().toStdU16String()) == file.path);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "chooses_a_file_while_a_popup_holds_the_input")
    {
        // Qt's dialog is driven through its Qt API: a popup of the application cannot hold the path back.
        file_window file;
        std::ofstream(file.path).put('x');
        popup_on_dialog thief;
        application.installEventFilter(&thief);
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("open");
                actions.choose_file(file.path, "Text (*.txt)");
                actions.finish();
            });
        application.removeEventFilter(&thief);
        CHECK(thief.popups == 1);
        CHECK(file.result == QDialog::Accepted);
        REQUIRE(file.selected.size() == 1);
        CHECK(std::filesystem::path(file.selected.front().toStdU16String()) == file.path);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_read_the_folders_above_the_chosen_file")
    {
        file_window file;
        std::ofstream(file.path).put('x');
        QStringList loaded;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.click("open");
                actions.eventually(
                    "the dialog is open",
                    [&]
                {
                    return gateway.call_until(
                        operation_deadline(),
                        [&]
                    {
                        auto* const dialog = QApplication::activeModalWidget();
                        auto* const model  = dialog != nullptr ? dialog->findChild<QFileSystemModel*>() : nullptr;
                        if(model != nullptr)
                        {
                            QObject::connect(
                                model,
                                &QFileSystemModel::directoryLoaded,
                                [&](const QString& _path){loaded << _path;});
                        }

                        return model != nullptr;
                    });
                });
                actions.choose_file(file.path, "Text (*.txt)");
                actions.finish();
            });
        CHECK(file.result == QDialog::Accepted);

        // The machine's temporary folder may hold thousands of entries: nothing reads it.
        const auto above = file.temporary.path().parent_path();
        CHECK_FALSE(loaded.contains(QDir::fromNativeSeparators(qt_path(above))));
        CHECK_FALSE(loaded.contains(QDir::fromNativeSeparators(qt_path(above.parent_path()))));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_modal_dialog_that_is_not_a_file_dialog")
    {
        QDialog dialog;
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.show();
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    actions.choose_file(std::filesystem::temp_directory_path() / "a.txt"),
                    doctest::Contains("not a file dialog"),
                    std::invalid_argument
                );
                CHECK_THROWS_AS(actions.cancel_file_dialog(), std::invalid_argument);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_missing_file_while_the_dialog_stays_open")
    {
        file_window file;
        scenario actions(application);
        scenario cleanup(application);
        run_scenario(
            *this,
            [&]
            {
                cleanup.click("open");
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.choose_file(file.folder / "missing.txt", {}, _timeout);
                },
                    [](const std::string& _message){return _message.find("disabled") != std::string::npos;});
                CHECK(failure.find("disabled") != std::string::npos);
                cleanup.cancel_file_dialog();
                cleanup.finish();
            });
        CHECK(file.result == QDialog::Rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "cancels_through_an_open_completion_popup")
    {
        QFileDialog dialog;
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.show();
        auto* const edit = dialog.findChild<QLineEdit*>("fileNameEdit");
        REQUIRE(edit != nullptr);

        // A list of its own: what the folder model offers depends on the machine.
        edit->setCompleter(new QCompleter(QStringList {"alpha", "alpha_2"}, edit));
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                actions.type("fileNameEdit", "a");
                REQUIRE(gateway.call_until(operation_deadline(), [&]{return edit->completer()->popup()->isVisible();}));
                actions.cancel_file_dialog();
                actions.finish();
            });
        CHECK(dialog.result() == QDialog::Rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_when_an_unrelated_modal_dialog_interrupts_the_selection")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / "existing.txt";
        std::ofstream(path).put('x');
        QDialog intruder;
        intruder.setWindowModality(Qt::ApplicationModal);
        interrupted_file_dialog dialog(intruder);
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setWindowModality(Qt::ApplicationModal);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.show();
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    actions.choose_file(path),
                    doctest::Contains("unrelated modal dialog"),
                    std::runtime_error
                );
            });
        intruder.hide();
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "follows_the_dialog_it_started_with")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / "existing.txt";
        QFileDialog first;
        QFileDialog second;
        file_dialog selection({path}, {}, false);

        // Refused for its text, once it knows its dialog.
        CHECK_THROWS_AS(selection.accept(first), input_failure);
        CHECK_THROWS_WITH_AS(selection.cancel(second), doctest::Contains("was replaced"), std::runtime_error);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "answers_only_the_overwrite_question_of_a_save_dialog")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / "existing.txt";
        QFileDialog open;
        file_dialog selection({path}, {}, false);
        CHECK_THROWS_AS(selection.accept(open), input_failure);

        // The right buttons, on a dialog that saves nothing.
        QMessageBox question(QMessageBox::Warning, "Replace?", "Replace?", QMessageBox::Yes | QMessageBox::No, &open);
        CHECK_THROWS_WITH_AS(
            selection.confirm_overwrite(question),
            doctest::Contains("unexpected confirmation"),
            std::runtime_error
        );
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "rejects_invalid_paths_filters_and_dialog_modes")
    {
        file_window file;
        scenario actions(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_AS(actions.choose_file({}), std::invalid_argument);
                CHECK_THROWS_AS(actions.choose_files({}), std::invalid_argument);
                CHECK_THROWS_AS(actions.choose_file("relative.txt"), std::invalid_argument);
                CHECK_THROWS_AS(
                    actions.choose_folder(file.folder, std::chrono::seconds::zero()),
                    std::invalid_argument
                );
                actions.click("open");
                CHECK_THROWS_AS(actions.choose_folder(file.folder), std::invalid_argument);
                CHECK_THROWS_AS(actions.choose_file(file.path, "Not a filter"), std::invalid_argument);
                CHECK_THROWS_AS(actions.choose_files({file.path, file.path}), std::invalid_argument);
                actions.cancel_file_dialog();
                actions.finish();
            });
        CHECK(file.result == QDialog::Rejected);
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "saves_a_png_through_its_real_dialog")
    {
        core::os::temp_dir temporary;
        const auto path = temporary / std::filesystem::path(u"snapshot_é.png");

        QImage written(4, 3, QImage::Format_RGB32);
        written.fill(Qt::red);
        QByteArray bytes;
        {
            QBuffer buffer(&bytes);
            REQUIRE(buffer.open(QIODevice::WriteOnly));
            REQUIRE(written.save(&buffer, "PNG"));
        }

        // Like a snapshot, the file is written in two parts, the second one later.
        QWidget window;
        QPushButton save("Save", &window);
        save.setObjectName("save");
        QObject::connect(
            &save,
            &QPushButton::clicked,
            [&]
            {
                QFileDialog dialog(&window);
                dialog.setOption(QFileDialog::DontUseNativeDialog);
                dialog.setAcceptMode(QFileDialog::AcceptSave);
                dialog.setDirectory(QString::fromStdU16String(temporary.path().u16string()));
                if(dialog.exec() != QDialog::Accepted)
                {
                    return;
                }

                auto file = std::make_shared<QFile>(dialog.selectedFiles().front());
                if(file->open(QIODevice::WriteOnly))
                {
                    file->write(bytes.left(bytes.size() - 12));
                    file->flush();
                    QTimer::singleShot(0, &window, [file, &bytes]{file->write(bytes.right(12)); file->close();});
                }
            });
        window.show();

        scenario actions(application);
        QImage read;
        run_scenario(*this, [&]{read = actions.save_png("save", path);});
        CHECK(read.size() == QSize(4, 3));
        CHECK(read.pixelColor(0, 0) == QColor(Qt::red));
    }
}

} // namespace sight::ui::test::detail
