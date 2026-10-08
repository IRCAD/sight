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

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

namespace sight::module::ui::qt::io
{

//------------------------------------------------------------------------------

drag_and_drop::drag_and_drop() noexcept
{
    new_slot(slots::NEXT_PATH, &drag_and_drop::send_next_path, this);
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
    const auto container    = sight::ui::registry::get_wid_container(m_wid);
    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(container);

    SIGHT_ASSERT("Unable to find Qt container: " << m_wid, qt_container);

    m_widget = qt_container->get_qt_container();

    m_widget->setAcceptDrops(true);
    qApp->installEventFilter(this);

    SIGHT_INFO("Drop enabled on WID '" << m_wid << "'");
}

//------------------------------------------------------------------------------

void drag_and_drop::updating()
{
}

//------------------------------------------------------------------------------
void drag_and_drop::stopping()
{
    m_pending_paths.clear();
    m_path_in_progress = false;

    SIGHT_ASSERT("Widget should not be null", !m_widget.isNull());

    qApp->removeEventFilter(this);
    m_widget->setAcceptDrops(false);

    m_widget.clear();
}

//-------------------------------------------------------------------------------

bool drag_and_drop::eventFilter(QObject* _obj, QEvent* _event)
{
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
                if(!url.isLocalFile())
                {
                    continue;
                }

                paths.emplace_back(url.toLocalFile().toStdString());
            }

            if(!paths.empty())
            {
                drop->acceptProposedAction();
                m_pending_paths.insert(m_pending_paths.end(), paths.begin(), paths.end());

                if(!m_path_in_progress)
                {
                    m_path_in_progress = true;
                    QMetaObject::invokeMethod(this, [this]{send_next_path();}, Qt::QueuedConnection);
                }

                return true;
            }
        }
    }

    return QObject::eventFilter(_obj, _event);
}

//------------------------------------------------------------------------------

void drag_and_drop::send_next_path()
{
    std::filesystem::path path;
    {
        if(!this->started() || m_pending_paths.empty())
        {
            m_pending_paths.clear();
            m_path_in_progress = false;
            return;
        }

        path = std::move(m_pending_paths.front());
        m_pending_paths.pop_front();
    }

    const auto data = m_path.lock();
    SIGHT_ASSERT("The path data is not initialized.", data);
    *data = path.string();
    data->async_emit(data::signals::MODIFIED);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::io
