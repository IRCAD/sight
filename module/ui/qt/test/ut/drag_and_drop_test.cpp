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

#include <core/thread/worker.hpp>

#include <data/series_set.hpp>
#include <data/string.hpp>

#include <io/__/service/reader.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>
#include <ui/test/gui_fixture.hpp>

#include <doctest/doctest.h>

#include <utest/wait.hpp>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMetaObject>
#include <QMimeData>
#include <QThread>
#include <QUrl>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace sight::module::ui::qt::ut
{

namespace
{

struct reader_state
{
    std::atomic_size_t update_count {0};
    std::atomic_bool should_throw {false};

    std::filesystem::path file;
    std::mutex mutex;
};

} // namespace

//------------------------------------------------------------------------------

static reader_state& get_reader_state()
{
    static reader_state state;
    return state;
}

//------------------------------------------------------------------------------

namespace
{

class test_drag_reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(test_drag_reader, sight::io::service::reader);

    test_drag_reader() :
        reader("Drag and drop test reader")
    {
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::file;
    }

    //------------------------------------------------------------------------------

    std::vector<std::pair<std::string, std::string> > get_supported_extensions() override
    {
        return {
            {"Sight drag test", ".sight-drag-test"}
        };
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        auto& state = get_reader_state();

        {
            const std::scoped_lock lock(state.mutex);
            state.file = get_file();
        }

        ++state.update_count;

        if(state.should_throw)
        {
            throw std::runtime_error("Drag and drop test exception");
        }

        m_read_failed = false;
    }
};

} // namespace

} // namespace sight::module::ui::qt::ut

SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::module::ui::qt::ut::test_drag_reader,
    sight::data::series_set
);

namespace sight::module::ui::qt::ut
{

namespace
{

class drag_and_drop_test
{
public:

    explicit drag_and_drop_test(service::base::sptr _service) :
        m_service(std::move(_service)),
        m_data(std::make_shared<data::series_set>())
    {
        auto& state = get_reader_state();
        state.update_count = 0;
        state.should_throw = false;

        {
            const std::scoped_lock lock(state.mutex);
            state.file.clear();
        }

        m_selector_worker = core::thread::worker::make();

        const auto setup = core::thread::get_default_worker()->post_task<setup_t>(
            [service = m_service, data = m_data, path = m_path, selector_worker = m_selector_worker]
                {
                    setup_t result;
                    result.wid = service->get_id();

                    const auto container    = sight::ui::registry::get_sid_container(result.wid);
                    const auto qt_container =
                        std::dynamic_pointer_cast<sight::ui::qt::container::widget>(container);

                    if(!qt_container)
                    {
                        throw std::runtime_error("Unable to find the drag-and-drop test container");
                    }

                    result.widget = qt_container->get_qt_container();
                    if(result.widget == nullptr)
                    {
                        throw std::runtime_error("The drag-and-drop test container has no QWidget");
                    }

                    sight::ui::registry::register_wid_container(result.wid, qt_container);

                    service::config_t config;
                    config.put("config.<xmlattr>.wid", result.wid);
                    service->set_inout(path, "path");
                    service->configure(config);
                    service->start().get();

                    result.selector = service::add("sight::module::ui::io::selector");
                    result.selector->set_worker(selector_worker);
                    result.selector->set_inout(data, sight::io::service::READER_DATA_KEY);
                    result.selector->set_input(path, "path.file");

                    service::config_t selector_config;
                    selector_config.put("path.<xmlattr>.file", "");
                    selector_config.put("selection.<xmlattr>.mode", "include");
                    selector_config.put(
                        "addSelection.<xmlattr>.service",
                        "sight::module::ui::qt::ut::test_drag_reader"
                    );
                    result.selector->configure(selector_config);
                    result.selector->start().get();
                    path->signal(data::signals::MODIFIED)->connect(result.selector->slot("update"));
                    result.selector->signal("succeeded")->connect(service->slot("next_path"));
                    result.selector->signal("failed")->connect(service->slot("next_path"));

                    return result;
                }).get();

        m_wid      = setup.wid;
        m_widget   = setup.widget;
        m_selector = setup.selector;
    }

    ~drag_and_drop_test()
    {
        core::thread::get_default_worker()->post_task<void>(
            [service = m_service, selector = m_selector, wid = m_wid]
                {
                    selector->stop().get();
                    service::remove(selector);

                    if(service->started())
                    {
                        service->stop().get();
                    }

                    sight::ui::registry::unregister_wid_container(wid);
                }).get();

        m_selector_worker->stop();

        for(const auto& path : m_test_files)
        {
            std::error_code error;
            std::filesystem::remove(path, error);
        }
    }

    //------------------------------------------------------------------------------

    void create_file(const std::string& _name)
    {
        m_test_files.push_back(std::filesystem::temp_directory_path() / (m_wid + "_" + _name));

        std::ofstream stream(m_test_files.back());
        REQUIRE(stream.good());
        stream << "test";
    }

    //------------------------------------------------------------------------------

    bool send_drop(const std::filesystem::path& _path)
    {
        return send_drop(std::vector<std::filesystem::path> {_path});
    }

    //------------------------------------------------------------------------------

    bool send_drop(const std::vector<std::filesystem::path>& _paths)
    {
        bool result = false;

        const auto connection_type =
            QThread::currentThread() == m_widget->thread()
            ? Qt::DirectConnection
            : Qt::BlockingQueuedConnection;

        QMetaObject::invokeMethod(
            m_widget.data(),
            [service = std::dynamic_pointer_cast<QObject>(m_service),
             widget = m_widget,
             paths = _paths,
             &result]
                {
                    QMimeData mime;
                    QList<QUrl> urls;
                    for(const auto& path : paths)
                    {
                        urls.push_back(QUrl::fromLocalFile(QString::fromStdString(path.string())));
                    }

                    mime.setUrls(urls);

                    QDropEvent event(
                        QPointF(0., 0.),
                        Qt::CopyAction,
                        &mime,
                        Qt::LeftButton,
                        Qt::NoModifier
                    );

                    result = service->eventFilter(widget, &event)
                             && event.isAccepted();
                },
            connection_type
        );

        return result;
    }

    //------------------------------------------------------------------------------

    bool send_drag_enter()
    {
        const auto connection_type =
            QThread::currentThread() == m_widget->thread()
            ? Qt::DirectConnection
            : Qt::BlockingQueuedConnection;

        const auto path =
            std::filesystem::temp_directory_path()
            / (m_wid + "_image.sight-drag-test");

        bool result = false;

        QMetaObject::invokeMethod(
            m_widget.data(),
            [service = std::dynamic_pointer_cast<QObject>(m_service),
             widget = m_widget,
             path,
             &result]
                {
                    QMimeData mime;
                    mime.setUrls(
                        {QUrl::fromLocalFile(QString::fromStdString(path.string()))
                        });

                    QDragEnterEvent event(
                        QPoint(0, 0),
                        Qt::CopyAction,
                        &mime,
                        Qt::LeftButton,
                        Qt::NoModifier
                    );

                    result = service->eventFilter(widget, &event)
                             && event.isAccepted();
                },
            connection_type
        );

        return result;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]]  const std::filesystem::path& test_file() const
    {
        return m_test_files.back();
    }

private:

    struct setup_t
    {
        std::string wid;
        QWidget* widget {nullptr};
        service::base::sptr selector;
    };

    service::base::sptr m_service;
    service::base::sptr m_selector;

    data::series_set::sptr m_data;
    data::string::sptr m_path {std::make_shared<data::string>()};
    core::thread::worker::sptr m_selector_worker;

    std::string m_wid;
    QPointer<QWidget> m_widget;
    std::vector<std::filesystem::path> m_test_files;
};

} // namespace

} // namespace sight::module::ui::qt::ut

TEST_SUITE("sight::module::ui::qt::io::drag_and_drop")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "drag_enter")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            CHECK(context.send_drag_enter());
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "supported_file")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            context.create_file("image.sight-drag-test");

            CHECK(context.send_drop(context.test_file()));

            SIGHT_TEST_FAIL_WAIT(
                sight::module::ui::qt::ut::get_reader_state().update_count == 1
            );

            std::filesystem::path read_file;

            {
                auto& state = sight::module::ui::qt::ut::get_reader_state();
                const std::scoped_lock lock(state.mutex);
                read_file = state.file;
            }

            CHECK_EQ(context.test_file(), read_file);
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "filename_with_semicolon")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            context.create_file("image;part.sight-drag-test");

            CHECK(context.send_drop(context.test_file()));

            SIGHT_TEST_FAIL_WAIT(
                sight::module::ui::qt::ut::get_reader_state().update_count == 1
            );

            std::filesystem::path read_file;
            {
                auto& state = sight::module::ui::qt::ut::get_reader_state();
                const std::scoped_lock lock(state.mutex);
                read_file = state.file;
            }

            CHECK_EQ(context.test_file(), read_file);
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "multiple_files")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            context.create_file("first.sight-drag-test");
            const auto first = context.test_file();
            context.create_file("second.sight-drag-test");
            const auto second = context.test_file();

            CHECK(context.send_drop(std::vector<std::filesystem::path> {first, second}));
            SIGHT_TEST_FAIL_WAIT(
                sight::module::ui::qt::ut::get_reader_state().update_count == 2
            );

            std::filesystem::path last_read_file;
            {
                auto& state = sight::module::ui::qt::ut::get_reader_state();
                const std::scoped_lock lock(state.mutex);
                last_read_file = state.file;
            }

            CHECK_EQ(last_read_file, second);
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "unsupported_file")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            context.create_file("image.unsupported");

            CHECK(context.send_drop(context.test_file()));

            SIGHT_TEST_WAIT(
                sight::module::ui::qt::ut::get_reader_state().update_count == 0,
                200
            );
            CHECK_EQ(sight::module::ui::qt::ut::get_reader_state().update_count, 0);
        });
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "reader_exception")
    {
        test_service(
            "sight::module::ui::qt::io::drag_and_drop",
            [](const sight::service::base::sptr& _service)
        {
            sight::module::ui::qt::ut::drag_and_drop_test context(_service);
            context.create_file("image.sight-drag-test");

            sight::module::ui::qt::ut::get_reader_state().should_throw = true;

            CHECK(context.send_drop(context.test_file()));

            SIGHT_TEST_FAIL_WAIT(
                sight::module::ui::qt::ut::get_reader_state().update_count == 1
            );
        });
    }
} // TEST_SUITE
