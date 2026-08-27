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

#include "gui_fixture.hpp"

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/uuid.hpp>

#include <service/op.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QMouseEvent>
#include <QPinchGesture>
#include <QProgressBar>
#include <QTest>
#include <QWindow>

#include <algorithm>
#include <map>
#include <stdexcept>

namespace sight::ui::test
{

namespace
{

constexpr auto FRAME_SERVICE = "sight::module::ui::frame";
constexpr auto VIEW_SERVICE  = "sight::module::ui::view";

struct service_keeper final
{
    explicit service_keeper(const std::vector<std::string>& _names)
    {
        m_view_uuid = sight::core::tools::uuid::generate();
        std::erase(m_view_uuid, '-');

        sight::core::thread::get_default_worker()->post_task<void>(
            [this, _names]
                {
                    m_frame = sight::service::add(FRAME_SERVICE);
                    if(!m_frame || !m_frame->is_a(FRAME_SERVICE))
                    {
                        throw std::runtime_error("Failed to create frame service");
                    }

                    sight::service::config_t frame_config;
                    frame_config.put("gui.frame.name", _names.front());
                    frame_config.put("gui.frame.min_size.<xmlattr>.width", "640");
                    frame_config.put("gui.frame.min_size.<xmlattr>.height", "480");
                    frame_config.put("registry.view.<xmlattr>.sid", m_view_uuid);

                    m_frame->configure(frame_config);
                    m_frame->start().get();

                    m_view = sight::service::add(VIEW_SERVICE, m_view_uuid);
                    if(!m_view || !m_view->is_a(VIEW_SERVICE))
                    {
                        throw std::runtime_error("Failed to create view service");
                    }

                    sight::service::config_t view_config;
                    view_config.put("gui.layout.<xmlattr>.type", "sight::ui::layout::line");
                    view_config.put("gui.layout.orientation.<xmlattr>.value", "vertical");

                    std::map<std::string, std::string> uuids;
                    std::ranges::for_each(
                        _names,
                        [&](const auto& _name)
                    {
                        auto uuid = sight::core::tools::uuid::generate();
                        std::erase(uuid, '-');
                        uuids[_name] = uuid;
                    });

                    for(const auto& [name, uuid] : uuids)
                    {
                        sight::service::config_t view_proportion;
                        view_proportion.add("<xmlattr>.proportion", "0");
                        view_config.add_child("gui.layout.view", view_proportion);

                        sight::service::config_t view_sid;
                        view_sid.add("<xmlattr>.sid", uuid);
                        view_config.add_child("registry.view", view_sid);
                    }

                    m_view->configure(view_config);
                    m_view->start().get();

                    m_services.reserve(uuids.size());

                    for(const auto& [name, uuid] : uuids)
                    {
                        const auto service = sight::service::add(name, uuid);
                        if(!service || !service->is_a(name))
                        {
                            throw std::runtime_error("Failed to create service: " + name);
                        }

                        m_services.emplace_back(service);
                    }
                }).get();
    }

    ~service_keeper()
    {
        sight::core::thread::get_default_worker()->post_task<void>(
            []
                {
                    if(!qApp)
                    {
                        return;
                    }

                    for(auto* window = qApp->modalWindow() ; window != nullptr ; window = qApp->modalWindow())
                    {
                        window->close();
                    }
                }).get();

        sight::core::thread::get_default_worker()->post_task<void>(
            [this]
                {
                    for(const auto& service : m_services)
                    {
                        if(service)
                        {
                            if(service->started())
                            {
                                service->stop().get();
                            }

                            sight::service::remove(service);
                        }
                    }

                    if(m_view)
                    {
                        if(m_view->started())
                        {
                            m_view->stop().get();
                        }

                        sight::service::remove(m_view);
                    }

                    if(m_frame)
                    {
                        if(m_frame->started())
                        {
                            m_frame->stop().get();
                        }

                        sight::service::remove(m_frame);
                    }

                    m_services.clear();
                    m_view.reset();
                    m_frame.reset();

                    if(qApp)
                    {
                        qApp->processEvents();
                    }
                }).get();
    }

    std::string m_view_uuid;
    sight::service::base::sptr m_frame;
    sight::service::base::sptr m_view;
    std::vector<sight::service::base::sptr> m_services;
};

} // namespace

//------------------------------------------------------------------------------

void gui_fixture::set_up()
{
    std::ranges::for_each(
        this->additional_module_paths(),
        [](const auto& _path)
        {
            sight::core::runtime::add_modules(_path);
        });

    m_qt_module = sight::core::runtime::load_module("sight::module::ui::qt");

    std::ranges::for_each(
        this->additional_modules(),
        [](const auto& _module)
        {
            const auto module = sight::core::runtime::load_module(_module);
            if(module == nullptr)
            {
                throw std::runtime_error("Failed to load module: " + _module);
            }
        });

    static bool initialized = false;

    if(!initialized)
    {
        const auto& profile = sight::core::runtime::get_current_profile();
        profile->set_name(sight::core::tools::uuid::generate());
        profile->start();

        initialized = true;
    }

    custom_set_up();
}

//------------------------------------------------------------------------------

void gui_fixture::tear_down()
{
    custom_tear_down();

    std::ranges::for_each(
        this->additional_modules(),
        [](const auto& _module)
        {
            sight::core::runtime::unload_module(_module);
        });

    if(m_qt_module && m_qt_module->is_started())
    {
        m_qt_module->stop();
    }
}

//------------------------------------------------------------------------------

std::vector<std::filesystem::path> gui_fixture::additional_module_paths()
{
    return {};
}

//------------------------------------------------------------------------------

std::vector<std::string> gui_fixture::additional_modules()
{
    return {};
}

//------------------------------------------------------------------------------

void gui_fixture::test_services(
    const std::vector<std::string>& _names,
    const std::function<void(const std::vector<sight::service::base::sptr>&)>& _test_script
)
{
    this->set_up();

    auto launch_script = std::async(
        std::launch::async,
        [_names, _test_script]
        {
            service_keeper keeper(_names);
            _test_script(keeper.m_services);
        });

    sight::core::thread::get_default_worker()->get_future().get();
    launch_script.get();

    this->tear_down();
}

//------------------------------------------------------------------------------

bool gui_fixture::click_on_button(const std::string& _name, const std::string& _text)
{
    const QString name = QString::fromStdString(_name);
    const QString text = QString::fromStdString(_text);

    for(const auto start = std::chrono::steady_clock::now() ;
        std::chrono::steady_clock::now() - start < SECONDS_TO_WAIT ; )
    {
        auto widget_ptr = gui_fixture::find_widget(_name);

        auto posted_task = sight::core::thread::get_default_worker()->post_task<bool>(
            [widget_ptr, name, text]
            {
                if(!widget_ptr.isNull())
                {
                    if(auto* const button = qobject_cast<QAbstractButton*>(widget_ptr); button != nullptr)
                    {
                        if(text.isEmpty() || button->text() == text)
                        {
                            button->click();
                            return true;
                        }
                    }

                    for(auto* const button : widget_ptr->findChildren<QAbstractButton*>())
                    {
                        if(text.isEmpty() || button->text() == text)
                        {
                            button->click();
                            return true;
                        }
                    }
                }

                for(auto* const widget : qApp->allWidgets())
                {
                    if(auto* const button = qobject_cast<QAbstractButton*>(widget); button != nullptr)
                    {
                        if((name.isEmpty() || button->objectName() == name)
                           && (text.isEmpty() || button->text() == text))
                        {
                            button->click();
                            return true;
                        }
                    }

                    for(auto* const button : widget->findChildren<QAbstractButton*>())
                    {
                        if((name.isEmpty() || button->objectName() == name)
                           && (text.isEmpty() || button->text() == text))
                        {
                            button->click();
                            return true;
                        }
                    }
                }

                return false;
            });

        if(posted_task.wait_for(SECONDS_TO_WAIT) != std::future_status::ready)
        {
            return false;
        }

        if(posted_task.get())
        {
            return true;
        }

        std::this_thread::sleep_for(MILLISECONDS_TO_SLEEP);
    }

    return false;
}

//------------------------------------------------------------------------------

bool gui_fixture::interact_with_widget(const std::string& _name, QEvent::Type _type, QPoint _interaction_ratio)
{
    auto widget_ptr = gui_fixture::find_widget(_name);

    auto posted_task = sight::core::thread::get_default_worker()->post_task<bool>(
        [widget_ptr, _type, _interaction_ratio]
        {
            if(!widget_ptr.isNull())
            {
                if(auto* const widget = qobject_cast<QWidget*>(widget_ptr); widget != nullptr)
                {
                    if(_type == QEvent::MouseButtonPress || _type == QEvent::MouseMove)
                    {
                        const QPoint local_pos(
                            static_cast<int>(widget->width() * _interaction_ratio.x() / 100.0),
                            static_cast<int>(widget->height() * _interaction_ratio.y() / 100.0)
                        );

                        QMouseEvent event(
                            _type,
                            local_pos,
                            widget->mapToGlobal(local_pos),
                            Qt::LeftButton,
                            Qt::LeftButton,
                            Qt::NoModifier
                        );

                        return QApplication::sendEvent(widget, &event);
                    }

                    if(_type == QEvent::Wheel)
                    {
                        QWheelEvent event(
                            QPointF(
                                widget->width() * _interaction_ratio.x() / 100.0,
                                widget->height() * _interaction_ratio.y() / 100.0
                            ),
                            QPointF(0, 0),
                            QPoint(0, 0),
                            QPoint(_interaction_ratio.x(), _interaction_ratio.y()),
                            Qt::NoButton,
                            Qt::NoModifier,
                            Qt::ScrollPhase::ScrollBegin,
                            false
                        );
                        return QApplication::sendEvent(widget, &event);
                    }

                    if(_type == QEvent::Gesture)
                    {
                        auto* pinch = new QPinchGesture();
                        pinch->setScaleFactor(_interaction_ratio.y() / 100.0);
                        QGestureEvent gesture(QList<QGesture*>() << pinch);

                        return QApplication::sendEvent(widget, &gesture);
                    }
                }
            }

            return false;
        });

    if(posted_task.wait_for(SECONDS_TO_WAIT) != std::future_status::ready)
    {
        return false;
    }

    return posted_task.get();
}

//------------------------------------------------------------------------------

bool gui_fixture::close_widget_window(const std::string& _name)
{
    auto widget_ptr = gui_fixture::find_widget(_name);

    auto posted_task = sight::core::thread::get_default_worker()->post_task<bool>(
        [widget_ptr]
        {
            if(!widget_ptr.isNull())
            {
                if(auto* const widget = qobject_cast<QWidget*>(widget_ptr); widget != nullptr)
                {
                    widget->window()->close();
                    return true;
                }
            }

            return false;
        });

    if(posted_task.wait_for(SECONDS_TO_WAIT) != std::future_status::ready)
    {
        return false;
    }

    return posted_task.get();
}

//------------------------------------------------------------------------------

} // namespace sight::ui::test
