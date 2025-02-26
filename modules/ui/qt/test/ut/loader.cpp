/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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
#include "loader.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/uuid.hpp>

#include <service/op.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QAbstractButton>
#include <QApplication>
#include <QPointer>
#include <QTimer>
#include <QWindow>

namespace sight::module::ui::qt::ut
{

inline static const struct loader final
{
    loader()
    {
        CPPUNIT_ASSERT_NO_THROW(
            core::runtime::add_modules(
                core::runtime::get_resource_file_path(
                    "module_ui_qt_ut"
                )
            )
        );

        sight::core::runtime::init();
    }
} LOADER;

//------------------------------------------------------------------------------

std::tuple<service::base::sptr, std::string> make_container(const std::string& _child_uuid)
{
    auto uuid = _child_uuid;

    if(uuid.empty())
    {
        uuid = core::tools::uuid::generate();
        uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
    }

    const service::base::sptr container(service::add("sight::module::ui::frame"));

    service::config_t container_config;
    container_config.put("gui.frame.name", "guiQtUnitTest");
    container_config.put("gui.frame.minSize.<xmlattr>.width", "640");
    container_config.put("gui.frame.minSize.<xmlattr>.height", "480");
    container_config.put("registry.view.<xmlattr>.sid", uuid);

    // Configure and start the container service
    container->configure(container_config);
    container->start().get();

    return std::make_tuple(container, uuid);
}

//------------------------------------------------------------------------------

void destroy_container(service::base::sptr& _container)
{
    if(_container)
    {
        if(_container->started())
        {
            _container->stop().get();
        }

        service::remove(_container);

        // Do not forget to reset the container to close remaining windows, otherwise the next test will fail
        _container.reset();
    }
}

//------------------------------------------------------------------------------

std::shared_future<bool> click_on_button(
    const std::string& _button_name,
    const std::string& _widget_name
)
{
    const auto& widget_name = QString::fromStdString(_widget_name);
    const auto& button_name = QString::fromStdString(_button_name);

    return wait_for_widget(
        [widget_name, button_name](QWidget* _widget)
        {
            if(widget_name.isEmpty() || _widget->objectName().startsWith(widget_name))
            {
                if(auto* button = _widget->findChild<QAbstractButton*>(button_name); button != nullptr)
                {
                    QPointer<QAbstractButton> button_ptr(button);

                    core::thread::get_default_worker()->post_task<void>(
                        [button_ptr]
                    {
                        if(!button_ptr.isNull())
                        {
                            button_ptr->click();
                        }
                    }).get();

                    return true;
                }
            }

            return false;
        });
}

//------------------------------------------------------------------------------

std::shared_future<bool> wait_for_widget(std::function<bool(QWidget*)> _predicate)
{
    return std::async(
        std::launch::async,
        [_predicate]
        {
            for(const auto start = std::chrono::steady_clock::now() ;
                std::chrono::steady_clock::now() - start < std::chrono::seconds(3) ; )
            {
                for(auto* widget : qApp->allWidgets())
                {
                    if(_predicate(widget))
                    {
                        return true;
                    }
                }

                // Wait a bit
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            // Close all modal windows to exit the local event loop
            if(QWindow* window = qApp->modalWindow(); window != nullptr)
            {
                QPointer<QWindow> window_ptr(window);

                core::thread::get_default_worker()->post_task<void>(
                    [window_ptr]
                {
                    if(!window_ptr.isNull())
                    {
                        window_ptr->close();
                    }
                });
            }
            else
            {
                core::thread::get_default_worker()->post_task<void>(
                    []
                {
                    qApp->closeAllWindows();
                });
            }

            return false;
        });
}

} // namespace sight::module::ui::qt::ut
