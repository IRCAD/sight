/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "progress_bar.hpp"

#include <core/runtime/path.hpp>

#include <QSvgRenderer>

namespace sight::ui::qt::widget
{

progress_bar::progress_bar(
    QWidget* _parent,
    const std::optional<bool>& _show_title,
    const std::optional<bool>& _show_cancel,
    const std::optional<bool>& _pulse,
    const std::optional<bool>& _expanding,
    const std::optional<std::filesystem::path>& _svg_path,
    const std::optional<int>& _svg_size,
    finished_callback_t _finished_callback
) :
    m_show_title(_show_title),
    m_show_cancel(_show_cancel),
    m_pulse(_pulse),
    m_expanding(_expanding),
    m_svg_path(_svg_path),
    m_svg_size(_svg_size),
    m_container(new QWidget(_parent)),
    m_finished_callback(std::move(_finished_callback))
{
    const QString object_name = _parent != nullptr ? _parent->objectName() + "/progress_bar" : "progress_bar";
    m_container->setObjectName(object_name);

    // Create the layout.
    auto* const layout = new QHBoxLayout();
    layout->setObjectName(QString::fromLatin1("/QHBoxLayout"));
    layout->setContentsMargins(0, 0, 0, 0);

    // Create the progress bar.
    if(m_svg_path && !m_svg_path->empty() && m_pulse.value_or(false))
    {
        // ...or an svg widget.
        const auto& resource_path = core::runtime::get_module_resource_file_path(*m_svg_path);
        m_svg_widget = new QSvgWidget(QString::fromStdString(resource_path.string()), m_container);
        m_svg_widget->setObjectName(object_name + QString::fromLatin1("/QSvgWidget"));
        m_svg_widget->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);

        if(m_svg_size)
        {
            m_svg_widget->setFixedSize(*m_svg_size, *m_svg_size);
        }

        layout->addWidget(m_svg_widget);
    }
    else
    {
        m_progress_bar = new QProgressBar(m_container);
        m_progress_bar->setObjectName(object_name + QString::fromLatin1("/QProgressBar"));
        m_progress_bar->setTextVisible(false);

        if(m_pulse.value_or(false))
        {
            m_progress_bar->setRange(0, 0);
        }
        else
        {
            m_progress_bar->setRange(0, 100);
        }

        layout->addWidget(m_progress_bar);

        if(m_expanding.value())
        {
            m_progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }
    }

    // Create button to cancel monitor.
    if(m_show_cancel.value_or(false))
    {
        m_cancel_button = new QToolButton(m_container);
        m_cancel_button->setObjectName(object_name + QString::fromLatin1("/QToolButton"));
        m_cancel_button->setText("Cancel");

        const auto& resource_path = core::runtime::get_module_resource_path("sight::module::ui::icons");
        const auto& icon_path     = resource_path / "cross.svg";
        m_cancel_button->setIcon(QIcon(QString::fromStdString(icon_path.string())));
        m_cancel_button->setIconSize(QSize(24, 24));
        m_cancel_button->setVisible(false);
        layout->addWidget(m_cancel_button);
    }

    // Add layout to the qt_container.
    m_container->setLayout(layout);
    m_container->setContentsMargins(0, 0, 0, 0);

    // Update the minimum height of the container.
    int minimum_height = 0;

    if(!m_svg_widget.isNull())
    {
        minimum_height = std::max(minimum_height, m_svg_widget->minimumSizeHint().height());
    }

    if(!m_progress_bar.isNull())
    {
        minimum_height = std::max(minimum_height, m_progress_bar->sizeHint().height());
    }

    if(!m_cancel_button.isNull())
    {
        minimum_height = std::max(minimum_height, m_cancel_button->sizeHint().height());
    }

    m_container->setMinimumHeight(std::max(minimum_height, m_container->minimumSizeHint().height()));

    m_container->setVisible(false);
}

progress_bar::~progress_bar()
{
    std::scoped_lock m_lock(m_mutex);
    m_progress_monitors.clear();

    if(!m_progress_bar.isNull())
    {
        m_progress_bar->deleteLater();
        m_progress_bar = nullptr;
    }

    if(!m_svg_widget.isNull())
    {
        m_svg_widget->deleteLater();
        m_svg_widget = nullptr;
    }

    if(!m_cancel_button.isNull())
    {
        m_cancel_button->deleteLater();
        m_cancel_button = nullptr;
    }

    if(!m_container.isNull())
    {
        m_container->deleteLater();
        m_container = nullptr;
    }
}

//------------------------------------------------------------------------------

void progress_bar::add_monitor(core::progress::monitor::sptr _monitor)
{
    {
        std::scoped_lock m_lock(m_mutex);

        // Add the monitor to the list.
        if(std::ranges::find_if(
               m_progress_monitors,
               [&_monitor](const auto& _other_monitor)
            {
                return _other_monitor.lock() == _monitor;
            }) == m_progress_monitors.cend())
        {
            m_progress_monitors.push_back(_monitor);
        }
    }

    // Use a "weak" this to avoid ownership to be passed to the lambdas which can be executed in different threads.
    const auto weak_this = this->weak_from_this();

    _monitor->add_done_work_hook(
        [weak_this](core::progress::monitor&, std::uint64_t)
        {
            core::thread::get_default_worker()->post_task<void>(
                [weak_this]
            {
                if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
                {
                    shared_this->update_widgets();
                }
            });
        });

    const core::progress::monitor::wptr weak_monitor = _monitor;
    _monitor->add_state_hook(
        [weak_this, weak_monitor](core::progress::monitor::state _state)
        {
            if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
            {
                if(_state == core::progress::monitor::canceled || _state == core::progress::monitor::finished)
                {
                    core::thread::get_default_worker()->post_task<void>(
                        [weak_this, weak_monitor]
                    {
                        if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
                        {
                            { // Some cleanup to remove expired monitors.
                                std::scoped_lock m_lock(shared_this->m_mutex);
                                std::erase_if(
                                    shared_this->m_progress_monitors,
                                    [weak_monitor](const auto& _weak_monitor)
                                {
                                    return _weak_monitor.expired() || (_weak_monitor.lock() == weak_monitor.lock());
                                });
                            }
                            shared_this->update_widgets();
                        }
                    });
                }
            }
        });

    if(!m_cancel_button.isNull())
    {
        QObject::connect(
            m_cancel_button,
            &QAbstractButton::clicked,
            [weak_monitor]
            {
                if(auto canceled_monitor = weak_monitor.lock(); canceled_monitor)
                {
                    // Call the cancel hook
                    canceled_monitor->cancel();
                }
            });
    }

    core::thread::get_default_worker()->post_task<void>(
        [weak_this]
        {
            if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
            {
                shared_this->update_widgets();
            }
        });
}

//------------------------------------------------------------------------------

void progress_bar::update_widgets()
{
    std::scoped_lock m_lock(m_mutex);

    // Update visibility of the widgets.
    const bool visible = !m_progress_monitors.empty();

    if(!m_container.isNull() && m_container->isVisible() != visible)
    {
        m_container->setVisible(visible);
    }

    // Iterate over a copy of monitors, because they can be removed in add_monitor state hook while iterating over them,
    // in the rare case where we release the last reference in "if(const auto& monitor = weak_monitor.lock();"
    // We have the weak/shared mechanism to protect each pointer, but we also require the iterator to be safe
    const auto safe_monitors_list = m_progress_monitors;

    // Update the widgets value.
    for(const auto& weak_monitor : safe_monitors_list)
    {
        // Get the current monitor as shared pointer.
        if(const auto& monitor = weak_monitor.lock();
           monitor && monitor->get_state() == core::progress::monitor::state::running)
        {
            const auto name       = monitor->name();
            const std::string msg = (monitor->get_logs().empty()) ? "" : monitor->get_logs().back();
            const auto title      = name.empty() ? QString() : QString::fromStdString(
                name + (msg.empty() ? "" : " - ") + msg
                                    ) + " - %p%";

            const int value = static_cast<int>((static_cast<float>(monitor->get_done_work_units())
                                                / static_cast<float>(monitor->get_total_work_units())
                                                * 100));

            if(!m_progress_bar.isNull())
            {
                if(const bool show_title = m_show_title.value_or(false); show_title != m_progress_bar->isTextVisible())
                {
                    if(show_title)
                    {
                        m_progress_bar->setAlignment(Qt::AlignCenter);
                        m_progress_bar->setTextVisible(true);
                    }
                    else
                    {
                        m_progress_bar->setTextVisible(false);
                    }
                }

                if(const auto format = m_progress_bar->format(); title != format)
                {
                    m_progress_bar->setFormat(title);
                }

                if(!m_pulse.value_or(false) && m_progress_bar->value() != value)
                {
                    m_progress_bar->setValue(value);
                }
            }

            if(!m_cancel_button.isNull())
            {
                const bool cancelable = monitor->is_cancelable();

                if(m_cancel_button->isVisible() != cancelable)
                {
                    m_cancel_button->setVisible(cancelable);
                }
            }

            // Look only for the first monitor alive.
            break;
        }
    }

    // Allow others to do something when all monitors are finished
    if(m_finished_callback && is_finished())
    {
        m_finished_callback();
    }
}

} //namespace sight::ui::qt::widget
