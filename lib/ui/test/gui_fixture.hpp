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

#pragma once

#include <sight/ui/test/config.hpp>

#include <core/runtime/module.hpp>
#include <service/base.hpp>

#include <QApplication>
#include <QPointer>
#include <QWidget>

#include <chrono>
#include <future>
#include <optional>
#include <string>

// cspell:ignore qobject

namespace sight::ui::test
{

/**
 * @brief Test fixture base class that helps testing GUI services.
 */
class SIGHT_UI_TEST_CLASS_API gui_fixture
{
public:

    gui_fixture()                              = default;
    virtual ~gui_fixture()                     = default;
    gui_fixture(const gui_fixture&)            = delete;
    gui_fixture& operator=(const gui_fixture&) = delete;

    /// Execute a test script on one or several services.
    SIGHT_UI_TEST_API void test_services(
        const std::vector<std::string>& _names,
        const std::function<void(const std::vector<sight::service::base::sptr>&)>& _test_script
    );

    //------------------------------------------------------------------------------

    SIGHT_UI_TEST_API void test_service(
        const std::string& _name,
        const std::function<void(const sight::service::base::sptr&)>& _test_script
)
    {
        test_services(
            {_name},
            [&_test_script](const std::vector<sight::service::base::sptr>& _services)
            {
                _test_script(_services[0]);
            });
    }

    /// Find a widget by name and type.
    template<class W = QWidget>
    static QPointer<W> find_widget(const std::string& _name)
    {
        const QString name = QString::fromStdString(_name);

        for(const auto start = std::chrono::steady_clock::now() ;
            std::chrono::steady_clock::now() - start < SECONDS_TO_WAIT ; )
        {
            auto posted_task = sight::core::thread::get_default_worker()->post_task<QPointer<W> >(
                [name]()
                {
                    for(auto* const widget : qApp->allWidgets())
                    {
                        if(auto* const typed_widget = qobject_cast<W*>(widget); typed_widget != nullptr)
                        {
                            if(name.isEmpty() || typed_widget->objectName() == name)
                            {
                                return QPointer<W>(typed_widget);
                            }
                        }

                        for(auto* const child : widget->findChildren<W*>())
                        {
                            if(name.isEmpty() || child->objectName() == name)
                            {
                                return QPointer<W>(child);
                            }
                        }
                    }

                    return QPointer<W>(nullptr);
                });

            if(posted_task.wait_for(SECONDS_TO_WAIT) != std::future_status::ready)
            {
                return QPointer<W>(nullptr);
            }

            if(auto widget_ptr = posted_task.get(); !widget_ptr.isNull())
            {
                return widget_ptr;
            }

            std::this_thread::sleep_for(MILLISECONDS_TO_SLEEP);
        }

        return QPointer<W>(nullptr);
    }

    //------------------------------------------------------------------------------

    template<class W = QWidget>
    static std::optional<std::string> get_widget_text(const std::string& _name)
    {
        for(const auto start = std::chrono::steady_clock::now() ;
            std::chrono::steady_clock::now() - start < SECONDS_TO_WAIT ; )
        {
            const auto& widget = find_widget<W>(_name);

            auto posted_task = sight::core::thread::get_default_worker()->post_task<std::optional<std::string> >(
                [widget]
                {
                    if(!widget.isNull())
                    {
                        return std::make_optional(widget->text().toStdString());
                    }

                    return std::optional<std::string>();
                });

            if(posted_task.wait_for(SECONDS_TO_WAIT) != std::future_status::ready)
            {
                return std::nullopt;
            }

            if(auto text = posted_task.get(); text)
            {
                return text;
            }

            std::this_thread::sleep_for(MILLISECONDS_TO_SLEEP);
        }

        return std::nullopt;
    }

    /// Click on a button with the given text.
    SIGHT_UI_TEST_API static bool click_on_button(
        const std::string& _name,
        const std::string& _text = std::string()
    );

    /// Interact with a widget by sending an event.
    SIGHT_UI_TEST_API static bool interact_with_widget(
        const std::string& _name,
        QEvent::Type _type,
        QPoint _interaction_ratio = QPoint(0, 0)
    );

    /// Close the top-level window containing the named widget.
    SIGHT_UI_TEST_API static bool close_widget_window(const std::string& _name);

    static constexpr std::chrono::seconds SECONDS_TO_WAIT {10};
    static constexpr std::chrono::milliseconds MILLISECONDS_TO_SLEEP {100};

protected:

    SIGHT_UI_TEST_API virtual std::vector<std::filesystem::path> additional_module_paths();
    SIGHT_UI_TEST_API virtual std::vector<std::string> additional_modules();

    //------------------------------------------------------------------------------

    virtual void custom_set_up()
    {
    }

    //------------------------------------------------------------------------------

    virtual void custom_tear_down()
    {
    }

private:

    std::shared_ptr<sight::core::runtime::module> m_qt_module;

    void set_up();
    void tear_down();
};

} // namespace sight::ui::test
