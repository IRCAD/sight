/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/thread/worker.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QSvgRenderer>
#include <QToolButton>
#include <QWidget>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

progress_bar::progress_bar() noexcept
{
    new_slot(slots::SHOW_JOB, &progress_bar::show_job, this);
}

//-----------------------------------------------------------------------------

void progress_bar::starting()
{
    this->create();
    const std::string service_id = base_id();
    auto widget_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* qt_container = widget_container->get_qt_container();
    qt_container->setObjectName(QString::fromStdString(service_id));

    // Create the layout.
    m_layout = new QHBoxLayout();
    m_layout->setObjectName(QString::fromLatin1("/QHBoxLayout"));
    m_layout->setContentsMargins(0, 0, 0, 0);

    // Create the label which holds the descriptive text shown with the progress bar.
    if(m_show_title)
    {
        m_title = new QLabel(qt_container);
        m_title->setObjectName(QString::fromLatin1("/QLabel"));
        m_title->setVisible(false);
        m_layout->addWidget(m_title);
    }

    // Create the progress bar.
    if(!m_svg_path.empty() && m_pulse)
    {
        // ...or an svg widget.
        const auto& resource_path = core::runtime::get_module_resource_file_path(m_svg_path);
        m_svg_widget = new QSvgWidget(QString::fromStdString(resource_path.string()), qt_container);
        m_svg_widget->setObjectName(QString::fromLatin1("/QSvgWidget"));
        m_svg_widget->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);

        if(m_svg_size)
        {
            m_svg_widget->setFixedSize(*m_svg_size, *m_svg_size);
        }

        m_svg_widget->setVisible(false);
        m_layout->addWidget(m_svg_widget);
    }
    else
    {
        m_progress_bar = new QProgressBar(qt_container);
        m_progress_bar->setObjectName(QString::fromLatin1("/QProgressBar"));

        if(m_pulse)
        {
            m_progress_bar->setRange(0, 0);
        }
        else
        {
            m_progress_bar->setRange(0, 100);
        }

        m_progress_bar->setVisible(false);
        m_layout->addWidget(m_progress_bar);
    }

    // Create button to cancel job.
    if(m_show_cancel)
    {
        m_cancel_button = new QToolButton(qt_container);
        m_cancel_button->setObjectName(QString::fromLatin1("/QToolButton"));
        m_cancel_button->setText("Cancel");

        const auto& resource_path = core::runtime::get_module_resource_path("sight::module::ui::icons");
        const auto& icon_path     = resource_path / "cross.svg";
        m_cancel_button->setIcon(QIcon(QString::fromStdString(icon_path.string())));
        m_cancel_button->setIconSize(QSize(24, 24));
        m_cancel_button->setVisible(false);
        m_layout->addWidget(m_cancel_button);
    }

    const auto& margins = qt_container->contentsMargins();
    int minimum_height  = 0;

    if(!m_title.isNull())
    {
        minimum_height = std::max(minimum_height, m_title->sizeHint().height());
    }

    if(!m_svg_widget.isNull())
    {
        minimum_height = std::max(minimum_height, m_svg_widget->sizeHint().height());
    }

    if(!m_progress_bar.isNull())
    {
        minimum_height = std::max(minimum_height, m_progress_bar->sizeHint().height());
    }

    if(!m_cancel_button.isNull())
    {
        minimum_height = std::max(minimum_height, m_cancel_button->sizeHint().height());
    }

    qt_container->setMinimumHeight(
        std::max(
            minimum_height + margins.bottom() + margins.top() + 2,
            qt_container->minimumSizeHint().height()
        )
    );

    // Add layout to the qt_container.
    widget_container->set_layout(m_layout);
}

//-----------------------------------------------------------------------------

void progress_bar::stopping()
{
    {
        std::lock_guard m_lock(m_mutex);
        m_jobs.clear();
    }

    if(!m_title.isNull())
    {
        m_title->deleteLater();
        m_title.clear();
    }

    if(!m_progress_bar.isNull())
    {
        m_progress_bar->deleteLater();
        m_progress_bar.clear();
    }

    if(!m_svg_widget.isNull())
    {
        m_svg_widget->deleteLater();
        m_svg_widget.clear();
    }

    if(!m_cancel_button.isNull())
    {
        m_cancel_button->deleteLater();
        m_cancel_button.clear();
    }

    if(!m_layout.isNull())
    {
        m_layout->deleteLater();
        m_layout.clear();
    }

    this->destroy();
} // namespace sight::module::ui::qt

//-----------------------------------------------------------------------------

void progress_bar::updating()
{
}

//-----------------------------------------------------------------------------

void progress_bar::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    m_show_title  = config.get<bool>("config.<xmlattr>.show_title", m_show_title);
    m_show_cancel = config.get<bool>("config.<xmlattr>.show_cancel", m_show_cancel);
    m_svg_path    = config.get<std::filesystem::path>("config.<xmlattr>.svg", m_svg_path);
    m_pulse       = config.get<bool>("config.<xmlattr>.pulse", !m_svg_path.empty());
    SIGHT_ASSERT("The svg attribute is only valid in pulse mode.", m_svg_path.empty() || m_pulse);

    if(const auto& svg_size = config.get_optional<int>("config.<xmlattr>.svg_size"); svg_size)
    {
        SIGHT_ASSERT("The svg size attribute is only valid if an svg file is used.", !m_svg_path.empty());

        m_svg_size = *svg_size;
    }
}

//------------------------------------------------------------------------------

void progress_bar::update_widgets()
{
    std::lock_guard m_lock(m_mutex);
    // Update visibility of the widgets.
    const bool visible = !m_jobs.empty();

    if(!m_title.isNull() && m_title->isVisible() != visible)
    {
        m_title->setVisible(visible);
    }

    if(!m_progress_bar.isNull() && m_progress_bar->isVisible() != visible)
    {
        m_progress_bar->setVisible(visible);
    }

    if(!m_svg_widget.isNull() && m_svg_widget->isVisible() != visible)
    {
        m_svg_widget->setVisible(visible);
    }

    if(!m_cancel_button.isNull() && !visible && m_cancel_button->isVisible())
    {
        m_cancel_button->setVisible(false);
    }

    // Update the widgets value.
    for(const auto& weak_job : m_jobs)
    {
        // Get the current job as shared pointer.
        if(const auto& job = weak_job.lock(); job && job->get_state() == core::jobs::base::state::running)
        {
            const auto name       = job->name();
            const std::string msg = (job->get_logs().empty()) ? "" : job->get_logs().back();
            const auto title      = name.empty() ? QString() : QString::fromStdString(
                name + (msg.empty() ? "" : " - ") + msg
            );
            const int value = int((float(job->get_done_work_units()) / float(job->get_total_work_units()) * 100));

            if(!m_title.isNull() && m_title->text() != title)
            {
                m_title->setText(title);
            }

            if(!m_pulse && !m_progress_bar.isNull() && m_progress_bar->value() != value)
            {
                m_progress_bar->setValue(value);
            }

            if(!m_cancel_button.isNull())
            {
                const bool cancelable = job->is_cancelable();

                if(m_cancel_button->isVisible() != cancelable)
                {
                    m_cancel_button->setVisible(cancelable);
                }
            }

            // Look only for the first job alive.
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void progress_bar::show_job(core::jobs::base::sptr _job)
{
    {
        std::lock_guard m_lock(m_mutex);
        // Add the job to the list.
        // We use weak pointer to avoid taking ownership of the job.
        if(std::find_if(
               m_jobs.cbegin(),
               m_jobs.cend(),
               [&_job](const auto& _other_job)
            {
                return _other_job.lock() == _job;
            }) == m_jobs.cend())
        {
            m_jobs.push_back(_job);
        }
    }

    // Use a "weak" this to avoid ownership to be passed to the lambdas which can be executed in different threads.
    auto weak_this = this->weak_from_this();

    _job->add_done_work_hook(
        [weak_this](core::jobs::base&, std::uint64_t)
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

    core::jobs::base::wptr weak_job = _job;
    _job->add_state_hook(
        [weak_this, weak_job](core::jobs::base::state _state)
        {
            if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
            {
                if(_state == core::jobs::base::canceled || _state == core::jobs::base::finished)
                {
                    core::thread::get_default_worker()->post_task<void>(
                        [weak_this, weak_job]
                    {
                        if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
                        {
                            { // Some cleanup to remove expired jobs.
                                std::lock_guard m_lock(shared_this->m_mutex);
                                std::erase_if(
                                    shared_this->m_jobs,
                                    [weak_job](const auto& _weak_job)
                                {
                                    return _weak_job.expired() || (_weak_job.lock() == weak_job.lock());
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
            &QPushButton::clicked,
            [weak_job]
            {
                if(auto canceled_job = weak_job.lock(); canceled_job)
                {
                    // Call the cancel hook
                    canceled_job->cancel();
                }
            });
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
