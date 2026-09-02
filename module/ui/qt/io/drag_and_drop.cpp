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

#include "drag_and_drop.hpp"

#include <io/__/reader/reader_helper.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QThread>

namespace sight::module::ui::qt::io
{

//------------------------------------------------------------------------------

drag_and_drop::drag_and_drop() noexcept :
    has_monitors(has_signals::signals()),
    m_slot_forward_notification(new_slot(slots::FORWARD_NOTIFICATION, &drag_and_drop::forward_notification, this))
{
}

//------------------------------------------------------------------------------

void drag_and_drop::configuring()
{
    const auto config = this->get_config();
    m_wid = config.get<std::string>("config.<xmlattr>.wid");
}

//------------------------------------------------------------------------------

void drag_and_drop::starting()
{
    const std::scoped_lock lock(m_mutex);

    const auto container = sight::ui::registry::get_wid_container(m_wid);

    const auto qt_container =
        std::dynamic_pointer_cast<sight::ui::qt::container::widget>(container);

    SIGHT_ASSERT("Unable to find Qt container: " << m_wid, qt_container);

    m_widget = qt_container->get_qt_container();
    auto start_drop = [this]()
                      {
                          m_widget->setAcceptDrops(true);
                          qApp->installEventFilter(this);
                      };

    QMetaObject::invokeMethod(
        m_widget,
        start_drop,
        Qt::QueuedConnection
    );

    m_read_worker = core::thread::worker::make();

    SIGHT_INFO("Drop enabled on WID '" << m_wid << "'");
}

//------------------------------------------------------------------------------

void drag_and_drop::updating()
{
}

//------------------------------------------------------------------------------
void drag_and_drop::stopping()
{
    const std::scoped_lock lock(m_mutex);

    if(!m_widget.isNull())
    {
        const QPointer<QWidget> widget = m_widget;

        auto stop_drop = [this, widget]()
                         {
                             qApp->removeEventFilter(this);

                             if(!widget.isNull())
                             {
                                 widget->setAcceptDrops(false);
                             }
                         };

        QMetaObject::invokeMethod(
            widget,
            stop_drop,
            Qt::QueuedConnection
        );

        m_widget.clear();
    }

    if(m_read_worker)
    {
        m_read_worker->stop();
        m_read_worker.reset();
    }
}

//-------------------------------------------------------------------------------

bool drag_and_drop::eventFilter(QObject* _obj, QEvent* _event)
{
    // execute on the main thread
    const std::scoped_lock lock(m_mutex);

    auto* const target = qobject_cast<QWidget*>(_obj);

    if(m_widget.isNull()
       || target == nullptr
       || (target != m_widget && !m_widget->isAncestorOf(target)))
    {
        return QObject::eventFilter(_obj, _event);
    }

    if(_event->type() == QEvent::DragEnter)
    {
        auto* drag_enter = static_cast<QDragEnterEvent*>(_event);

        if(drag_enter->mimeData()->hasUrls())
        {
            drag_enter->acceptProposedAction();
            return true;
        }
    }

    if(_event->type() == QEvent::Drop)
    {
        auto* drop = static_cast<QDropEvent*>(_event);

        if(drop->mimeData()->hasUrls())
        {
            std::vector<std::filesystem::path> paths;
            for(const QUrl& url : drop->mimeData()->urls())
            {
                paths.emplace_back(url.toLocalFile().toStdString());
            }

            if(!paths.empty())
            {
                drop->acceptProposedAction();

                m_read_worker->post(
                    [this, paths = std::move(paths)]() mutable
                    {
                        const auto data = [&]
                                          {
                                              const auto data_lock = m_data.lock();
                                              return data_lock.get_shared();
                                          }();

                        const bool success = sight::io::reader::read_paths(
                            paths,
                            data,
                            m_slot_forward_notification,
                            {},
                            true
                        );

                        if(!success)
                        {
                            const std::string message = "Loading failed.";

                            SIGHT_ERROR(message);
                            this->fail(message);
                        }
                    });

                return true;
            }
        }
    }

    return QObject::eventFilter(_obj, _event);
}

//------------------------------------------------------------------------------

void drag_and_drop::forward_notification(core::notification::base::sptr _notification)
{
    this->async_emit(core::notification::has_notifications::signals::NOTIFICATION_CREATED, _notification);
}

} // namespace sight::module::ui::qt::io
