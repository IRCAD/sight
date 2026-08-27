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

#include "notification_zone.hpp"

#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/warning.hpp>

#include <core/runtime/path.hpp>

#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QAudioOutput>
#include <QEvent>
#include <QHBoxLayout>

#include <utility>

namespace sight::module::ui::qt
{

namespace
{

constexpr const char* QSS_CLASS           = "class";
const auto NOTIFICATION_LABEL             = QString::fromLatin1("notification_label");
const auto NOTIFICATION_STACK             = QString::fromLatin1("notification_stack");
const auto NOTIFICATION_LABEL_INSTRUCTION = NOTIFICATION_LABEL + "_instruction";
const auto NOTIFICATION_LABEL_INFORMATION = NOTIFICATION_LABEL + "_information";
const auto NOTIFICATION_LABEL_WARNING     = NOTIFICATION_LABEL + "_warning";
const auto NOTIFICATION_LABEL_ERROR       = NOTIFICATION_LABEL + "_error";

} // namespace

//------------------------------------------------------------------------------

static int priority_of(const sight::core::notification::base::sptr& _notification, bool _is_monitor)
{
    if(std::dynamic_pointer_cast<sight::core::notification::error>(_notification))
    {
        return 4;
    }

    if(std::dynamic_pointer_cast<sight::core::notification::warning>(_notification))
    {
        return 3;
    }

    if(_is_monitor)
    {
        return 2;
    }

    if(std::dynamic_pointer_cast<sight::core::notification::information>(_notification))
    {
        return 1;
    }

    // instruction, or unknown: lowest priority
    return 0;
}

class acknowledger final : public QObject
{
public:

    explicit acknowledger(notification_zone::wptr _notification_zone, QWidget* _page = nullptr) :
        QObject(_page),
        m_notification_zone(std::move(_notification_zone)),
        m_page_widget(_page)
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*_watched*/, QEvent* _event) final
    {
        if(_event->type() != QEvent::MouseButtonRelease && _event->type() != QEvent::TouchBegin)
        {
            return false;
        }

        const auto notification_zone = m_notification_zone.lock();
        if(!notification_zone)
        {
            return false;
        }

        const auto found_page = notification_zone->m_pages.find(m_page_widget);
        if(found_page == notification_zone->m_pages.end())
        {
            return false;
        }

        const auto& page = found_page->second;

        const bool is_error = std::dynamic_pointer_cast<sight::core::notification::error>(page.m_notification)
                              != nullptr;

        if(page.m_progress != nullptr)
        {
            // Progress can only be acknowledged once it is finished.
            if(!page.m_progress->is_finished())
            {
                return false;
            }

            notification_zone->close_page(found_page);
        }
        else if(is_error)
        {
            // Capture the notification before calling finish(): it synchronously drives
            // on_notification_finished()/close_page() (see notifications_store::watch()), which erases
            // found_page - found_page/page must not be used afterward.
            const auto notification = page.m_notification;
            notification->finish();
        }
        else
        {
            return false;
        }

        return false;
    }

private:

    const notification_zone::wptr m_notification_zone;
    const QPointer<QWidget> m_page_widget;
};

notification_zone::notification_zone()
{
    this->new_slot(slots::CLEAR, &notification_zone::clear, this);
    this->new_slot(slots::ADD_NOTIFICATION, &notification_zone::add_notification, this);
    this->new_slot(slots::ADD_MONITOR, &notification_zone::add_monitor, this);

    this->new_signal<signals::notification_t>(signals::ERROR);
    this->new_signal<signals::notification_t>(signals::ERROR_ACKNOWLEDGED);
    this->new_signal<signals::notification_t>(signals::PROGRESS);
    this->new_signal<signals::notification_t>(signals::PROGRESS_ACKNOWLEDGED);
}

//------------------------------------------------------------------------------

void notification_zone::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void notification_zone::starting()
{
    this->create();

    const auto widget_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    SIGHT_THROW_IF("Invalid widget container", !widget_container);
    auto* const qt_container = widget_container->get_qt_container();

    auto* main_layout = qt_container->layout();

    if(main_layout == nullptr)
    {
        main_layout = new QHBoxLayout();
        widget_container->set_layout(main_layout);
    }

    m_stack = new QStackedWidget(qt_container);
    m_stack->setObjectName(NOTIFICATION_STACK);

    main_layout->addWidget(m_stack);

    m_sound = std::make_unique<QMediaPlayer>(qApp);
    auto* audio_output = new QAudioOutput(qApp);
    m_sound->setAudioOutput(audio_output);
}

//------------------------------------------------------------------------------

void notification_zone::updating()
{
}

//------------------------------------------------------------------------------

void notification_zone::stopping()
{
    this->clear();

    if(m_stack != nullptr)
    {
        m_stack->deleteLater();
        m_stack = nullptr;
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void notification_zone::swapping(std::string_view /*_key*/)
{
}

//------------------------------------------------------------------------------

void notification_zone::clear()
{
    for(const auto& entry : m_pages)
    {
        const auto& page = entry.second;

        if(!page.m_timer.isNull())
        {
            page.m_timer->deleteLater();
        }

        this->forget(page.m_notification);
    }

    m_pages.clear();

    if(m_stack != nullptr)
    {
        while(m_stack->count() > 0)
        {
            auto* const page_widget = m_stack->widget(0);
            m_stack->removeWidget(page_widget);
            page_widget->deleteLater();
        }
    }
}

//------------------------------------------------------------------------------

void notification_zone::add_notification(sight::core::notification::base::sptr _notification)
{
    if(!_notification)
    {
        return;
    }

    if(const auto job = std::dynamic_pointer_cast<sight::core::notification::monitor>(_notification))
    {
        this->handle_monitor(job);
        return;
    }

    if(const auto instruction = std::dynamic_pointer_cast<sight::core::notification::instruction>(_notification))
    {
        this->handle_instruction(instruction);
        return;
    }

    if(const auto message = std::dynamic_pointer_cast<sight::core::notification::message>(_notification))
    {
        this->handle_message(message);
        return;
    }

    SIGHT_WARN("Ignoring notification of unsupported type passed to add_notification slot.");
}

//------------------------------------------------------------------------------

void notification_zone::add_monitor(sight::core::notification::base::sptr _notification)
{
    const auto job = std::dynamic_pointer_cast<sight::core::notification::monitor>(_notification);
    if(!job)
    {
        SIGHT_WARN("Ignoring non-monitor notification passed to add_monitor slot.");
        return;
    }

    this->handle_monitor(job);
}

//------------------------------------------------------------------------------

void notification_zone::handle_monitor(const sight::core::notification::monitor::sptr& _job)
{
    const auto weak_this = this->weak_from_this();

    sight::core::thread::get_default_worker()->post_task<void>(
        [weak_this, _job]
        {
            const auto shared_this = dynamic_pointer_cast<notification_zone>(weak_this.lock());
            if(!shared_this)
            {
                return;
            }

            const auto found_page = std::ranges::find_if(
                shared_this->m_pages,
                [](const auto& _page)
            {
                return _page.second.m_progress != nullptr;
            });

            if(found_page != shared_this->m_pages.end() && !found_page->second.m_progress->is_finished())
            {
                found_page->second.m_progress->add_monitor(_job);
            }
            else
            {
                if(found_page != shared_this->m_pages.end())
                {
                    // Page's finished, just not closed yet: don't pile onto it, start a fresh one.
                    shared_this->close_page(found_page);
                }

                auto& page = shared_this->create_page(_job);
                page.m_progress->add_monitor(_job);
            }

            shared_this->refresh_top();
        });
}

//------------------------------------------------------------------------------

void notification_zone::handle_instruction(const sight::core::notification::instruction::sptr& _instruction)
{
    const auto weak_this = this->weak_from_this();

    sight::core::thread::get_default_worker()->post_task<void>(
        [weak_this, _instruction]
        {
            const auto shared_this = dynamic_pointer_cast<notification_zone>(weak_this.lock());
            if(!shared_this)
            {
                return;
            }

            const auto found_page = std::ranges::find_if(
                shared_this->m_pages,
                [](const auto& _page)
            {
                return std::dynamic_pointer_cast<sight::core::notification::instruction>(
                    _page.second.m_notification
                ) != nullptr;
            });

            if(found_page != shared_this->m_pages.end())
            {
                if(_instruction->text().empty())
                {
                    // close_page() already forgets the notification and refreshes the top page.
                    shared_this->close_page(found_page);
                    return;
                }

                // The instruction being replaced is discarded: forget it and start watching the new one,
                // otherwise a later finish()/set_text() call on it would have no effect on the display.
                shared_this->forget(found_page->second.m_notification);
                found_page->second.m_notification = _instruction;
                shared_this->watch(_instruction, /*_own=*/ true, shared_this->weak_from_this());

                if(auto* const label = found_page->first->findChild<QLabel*>(NOTIFICATION_LABEL_INSTRUCTION);
                   label != nullptr)
                {
                    label->setText(QString::fromStdString(_instruction->text()));
                }
            }
            else if(!_instruction->text().empty())
            {
                shared_this->create_page(_instruction);
            }

            shared_this->refresh_top();
        });
}

//------------------------------------------------------------------------------

void notification_zone::handle_message(const sight::core::notification::message::sptr& _message)
{
    const auto weak_this = this->weak_from_this();

    sight::core::thread::get_default_worker()->post_task<void>(
        [weak_this, _message]
        {
            const auto shared_this = dynamic_pointer_cast<notification_zone>(weak_this.lock());
            if(!shared_this)
            {
                return;
            }

            auto& page = shared_this->create_page(_message);

            const int duration = shared_this->resolve_duration_ms(_message);

            if(duration > 0)
            {
                auto* const timer = new QTimer(shared_this->m_stack);
                timer->setSingleShot(true);
                timer->setInterval(duration);

                QObject::connect(
                    timer,
                    &QTimer::timeout,
                    shared_this->m_stack,
                    [weak_this, widget = page.m_widget]()
                {
                    const auto shared_this = dynamic_pointer_cast<notification_zone>(weak_this.lock());
                    if(!shared_this)
                    {
                        return;
                    }

                    if(!widget.isNull())
                    {
                        if(const auto found_page = shared_this->m_pages.find(widget);
                           found_page != shared_this->m_pages.end())
                        {
                            shared_this->close_page(found_page);
                        }
                    }
                });

                const auto it_timer = shared_this->m_pages.find(page.m_widget);
                if(it_timer != shared_this->m_pages.end())
                {
                    it_timer->second.m_timer = timer;
                }
            }

            shared_this->refresh_top();

            if(_message->sound().value_or(false))
            {
                shared_this->play_sound(_message);
            }
        });
}

//------------------------------------------------------------------------------

int notification_zone::resolve_duration_ms(const sight::core::notification::message::sptr& _message) const
{
    if(std::dynamic_pointer_cast<sight::core::notification::instruction>(_message)
       || std::dynamic_pointer_cast<sight::core::notification::error>(_message))
    {
        return 0;
    }

    if(_message->duration())
    {
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(*_message->duration()).count()
        );
    }

    if(std::dynamic_pointer_cast<sight::core::notification::warning>(_message))
    {
        return static_cast<int>(*m_warning_duration);
    }

    return static_cast<int>(*m_information_duration);
}

//------------------------------------------------------------------------------

void notification_zone::play_sound(const sight::core::notification::base::sptr& _notification) const
{
    static const auto s_INFO_SOUND = std::filesystem::canonical(
        sight::core::runtime::get_resource_file_path("sight::module::ui::qt/sounds/info_beep.wav")
    );
    static const auto s_ERROR_SOUND = std::filesystem::canonical(
        sight::core::runtime::get_resource_file_path("sight::module::ui::qt/sounds/failure_beep.wav")
    );

    const bool is_error = std::dynamic_pointer_cast<sight::core::notification::error>(_notification) != nullptr;

    m_sound->setSource(QUrl::fromLocalFile(QString::fromStdString((is_error ? s_ERROR_SOUND : s_INFO_SOUND).string())));
    m_sound->play();
}

//------------------------------------------------------------------------------

void notification_zone::close_page(std::map<QWidget*, page>::iterator _it)
{
    if(_it == m_pages.end())
    {
        return;
    }

    auto* const widget           = _it->first;
    const auto notification      = _it->second.m_notification;
    const auto weak_notification = _it->second.m_notification_weak;
    const bool is_error          = std::dynamic_pointer_cast<sight::core::notification::error>(notification)
                                   != nullptr;
    const bool is_monitor = _it->second.m_progress != nullptr;

    if(!_it->second.m_timer.isNull())
    {
        _it->second.m_timer->deleteLater();
    }

    m_stack->removeWidget(widget);
    widget->deleteLater();

    this->forget(notification);
    m_pages.erase(_it);

    if(is_error || is_monitor)
    {
        // Monitor jobs are weakly tracked, so they may already be gone here - pass nullptr if so.
        this->async_emit(
            is_error ? signals::ERROR_ACKNOWLEDGED : signals::PROGRESS_ACKNOWLEDGED,
            is_error ? notification : weak_notification.lock()
        );
    }

    this->refresh_top();
}

//------------------------------------------------------------------------------

void notification_zone::on_notification_finished(const sight::core::notification::base::wptr& _notification)
{
    const auto notification = _notification.lock();
    if(!notification)
    {
        return;
    }

    const auto found_page = std::ranges::find_if(
        m_pages,
        [&notification](const auto& _page)
        {
            return _page.second.m_notification == notification;
        });

    if(found_page != m_pages.end())
    {
        this->close_page(found_page);
    }
}

//------------------------------------------------------------------------------

void notification_zone::on_notification_changed(const sight::core::notification::message::wptr& _notification)
{
    const auto notification = _notification.lock();
    if(!notification)
    {
        return;
    }

    const auto found_page = std::ranges::find_if(
        m_pages,
        [&notification](const auto& _page)
        {
            return _page.second.m_notification == notification;
        });

    if(found_page != m_pages.end() && !found_page->second.m_label.isNull())
    {
        found_page->second.m_label->setText(QString::fromStdString(notification->text()));
    }
}

//------------------------------------------------------------------------------

notification_zone::page& notification_zone::create_page(const sight::core::notification::base::sptr& _notification)
{
    notification_zone::page page;

    if(std::dynamic_pointer_cast<sight::core::notification::monitor>(_notification))
    {
        // Weak only: the emitter owns the job and must be free to let it finish() on its own.
        page.m_notification_weak = _notification;

        page.m_progress = std::make_shared<sight::ui::qt::widget::progress_bar>(
            m_stack,
            *m_show_progress_title,
            *m_show_progress_cancel,
            *m_progress_pulse,
            *m_progress_expand,
            *m_progress_svg_path,
            *m_progress_svg_size
        );

        page.m_widget = new QWidget(m_stack);
        page.m_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        page.m_widget->setObjectName("progress_page");

        auto* const layout = new QHBoxLayout(page.m_widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(page.m_progress->widget());

        const auto weak_this                                                   = this->weak_from_this();
        const std::weak_ptr<sight::ui::qt::widget::progress_bar> weak_progress = page.m_progress;

        page.m_progress->set_finished_callback(
            [weak_this, weak_progress]
            {
                const auto shared_this = std::dynamic_pointer_cast<notification_zone>(weak_this.lock());
                if(!shared_this)
                {
                    return;
                }

                const auto shared_progress = weak_progress.lock();
                if(!shared_progress)
                {
                    return;
                }

                auto* const widget = shared_progress->widget()->parentWidget();
                if(widget == nullptr)
                {
                    return;
                }

                if(const auto found_page = shared_this->m_pages.find(widget);
                   found_page != shared_this->m_pages.end())
                {
                    shared_this->close_page(found_page);
                }
            });

        page.m_widget->installEventFilter(
            new acknowledger(std::dynamic_pointer_cast<notification_zone>(this->get_sptr()), page.m_widget)
        );
    }
    else
    {
        page.m_notification = _notification;

        page.m_widget = new QWidget(m_stack);
        page.m_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        page.m_widget->setObjectName("notification_page");

        auto* const box = new QHBoxLayout(page.m_widget);
        box->setContentsMargins(0, 0, 0, 0);
        page.m_label = new QLabel(page.m_widget);
        page.m_label->setWordWrap(true);

        const auto message = std::dynamic_pointer_cast<sight::core::notification::message>(_notification);
        page.m_label->setText(QString::fromStdString(message ? message->text() : std::string()));
        page.m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        if(std::dynamic_pointer_cast<sight::core::notification::instruction>(_notification))
        {
            page.m_label->setObjectName(NOTIFICATION_LABEL_INSTRUCTION);
            page.m_label->setProperty(QSS_CLASS, NOTIFICATION_LABEL_INSTRUCTION);
        }
        else if(std::dynamic_pointer_cast<sight::core::notification::information>(_notification))
        {
            page.m_label->setObjectName(NOTIFICATION_LABEL_INFORMATION);
            page.m_label->setProperty(QSS_CLASS, NOTIFICATION_LABEL_INFORMATION);
        }
        else if(std::dynamic_pointer_cast<sight::core::notification::warning>(_notification))
        {
            page.m_label->setObjectName(NOTIFICATION_LABEL_WARNING);
            page.m_label->setProperty(QSS_CLASS, NOTIFICATION_LABEL_WARNING);
        }
        else if(std::dynamic_pointer_cast<sight::core::notification::error>(_notification))
        {
            page.m_label->setObjectName(NOTIFICATION_LABEL_ERROR);
            page.m_label->setProperty(QSS_CLASS, NOTIFICATION_LABEL_ERROR);

            page.m_label->installEventFilter(
                new acknowledger(std::dynamic_pointer_cast<notification_zone>(this->get_sptr()), page.m_widget)
            );
        }

        box->addWidget(page.m_label);
        page.m_widget->setLayout(box);

        // Let the emitting service manipulate its own notification's widget later on, by calling
        // finish()/cancel() (closes the page) or set_title()/set_text() (updates the label) on the sptr it
        // kept from notify()/warn()/fail()/inform()/instruct().
        this->watch(_notification, /*_own=*/ true, this->weak_from_this());
    }

    m_stack->addWidget(page.m_widget);

    const auto widget = page.m_widget;
    m_pages.emplace(widget, std::move(page));

    return m_pages[widget];
}

//------------------------------------------------------------------------------

void notification_zone::refresh_top()
{
    std::optional<int> top_priority;

    for(int index = m_stack->count() ; --index >= 0 ; )
    {
        auto* const widget = m_stack->widget(index);
        if(widget == nullptr)
        {
            continue;
        }

        const auto& page   = m_pages[widget];
        const int priority = priority_of(page.m_notification, page.m_progress != nullptr);

        if(!top_priority || priority > *top_priority)
        {
            top_priority = priority;
            m_stack->setCurrentIndex(index);
        }
    }

    if(auto* const top_widget = m_stack->currentWidget(); top_widget != nullptr)
    {
        for(auto& entry : m_pages)
        {
            auto* const widget = entry.first;
            auto& page         = entry.second;

            if(widget == top_widget)
            {
                if(!page.m_timer.isNull() && !page.m_timer->isActive())
                {
                    page.m_timer->start();
                }
            }
            else
            {
                if(!page.m_timer.isNull() && page.m_timer->isActive())
                {
                    page.m_timer->stop();
                }
            }
        }

        if(const auto& top_page = m_pages[top_widget];
           std::dynamic_pointer_cast<sight::core::notification::error>(top_page.m_notification))
        {
            this->async_emit(signals::ERROR, top_page.m_notification);
        }
        else if(top_page.m_progress != nullptr)
        {
            this->async_emit(signals::PROGRESS, top_page.m_notification_weak.lock());
        }
    }
}

} // namespace sight::module::ui::qt
