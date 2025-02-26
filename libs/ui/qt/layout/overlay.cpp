/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "ui/qt/layout/overlay.hpp"

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QResizeEvent>
#include <QWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::overlay, sight::ui::layout::overlay::s_registry_key)

namespace sight::ui::qt::layout
{

//------------------------------------------------------------------------------

using coord = ui::layout::overlay::view::coord;

//------------------------------------------------------------------------------

static int calculate_offset(coord _configured_offset, int _parent_size, int _widget_size)
{
    int res = _configured_offset.relative ? static_cast<int>((std::abs(_configured_offset.value) / 100.) * _parent_size)
                                          : std::abs(_configured_offset.value);
    // If the size is negative, then the widget must be aligned to the right or the bottom.
    // In Qt, the position of a widget is defined by its top left corner, therefore we must substract the size of the
    // widget from the offset.
    if(_configured_offset.value <= 0 && _configured_offset.negative)
    {
        res = std::max(0, _parent_size - _widget_size - res);
    }
    else
    {
        res = std::min(res, _parent_size - _widget_size);
    }

    return res;
}

//------------------------------------------------------------------------------

static int calculate_size(coord _configured_size, int _parent_size)
{
    return _configured_size.relative ? static_cast<int>((_configured_size.value / 100.) * _parent_size)
                                     : _configured_size.value;
}

/// This class makes the widget transparent for mouse events and restores visibility when destroyed
class widget_hider
{
public:

    explicit widget_hider(QWidget* _widget) :
        m_was_transparent(_widget->testAttribute(Qt::WA_TransparentForMouseEvents)),
        m_widget(_widget)
    {
        if(!m_was_transparent)
        {
            m_widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        }
    }

    ~widget_hider()
    {
        if(!m_widget.isNull() && !m_was_transparent)
        {
            m_widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        }
    }

private:

    const bool m_was_transparent;
    const QPointer<QWidget> m_widget;
};

class event_forwarder : public QObject
{
public:

    explicit event_forwarder(QWidget* _child) :
        QObject(_child)
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _target, QEvent* _event) override
    {
        switch(_event->type())
        {
            case QEvent::LayoutRequest:
                if(auto* const widget = dynamic_cast<QWidget*>(_target); widget != nullptr)
                {
                    // Makes the container transparent to mouse events if there are no visible child in the layout
                    // This is the only known way to make multi-touch events (pan, pinch) work
                    const auto* const layout = widget->layout();
                    const bool transparent   = layout == nullptr || layout->sizeHint().isNull();

                    if(widget->testAttribute(Qt::WA_TransparentForMouseEvents) != transparent)
                    {
                        widget->setAttribute(Qt::WA_TransparentForMouseEvents, transparent);
                    }
                }

                break;

            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseMove:
                if(auto* const mouse_event = dynamic_cast<QMouseEvent*>(_event); mouse_event != nullptr)
                {
                    if(forward_event(_target, _event, mouse_event->globalPos()))
                    {
                        return true;
                    }
                }

                break;

            case QEvent::Wheel:
                if(auto* const wheel_event = dynamic_cast<QWheelEvent*>(_event); wheel_event != nullptr)
                {
                    if(forward_event(_target, _event, wheel_event->globalPos()))
                    {
                        return true;
                    }
                }

                break;

            case QEvent::HoverEnter:
            case QEvent::HoverLeave:
            case QEvent::HoverMove:
                if(auto* const hover_event = dynamic_cast<QHoverEvent*>(_event); hover_event != nullptr)
                {
                    if(auto* const widget = dynamic_cast<QWidget*>(_target); widget != nullptr)
                    {
                        if(forward_event(_target, _event, widget->mapToGlobal(hover_event->pos())))
                        {
                            return true;
                        }
                    }
                }

                break;

            default:
                break;
        }

        return QObject::eventFilter(_target, _event);
    }

private:

    /// Forward the event to the sibling widget "under" the overlay (the scene)
    static bool forward_event(QObject* _target, QEvent* _event, const QPoint& _global_pos)
    {
        auto* const widget = dynamic_cast<QWidget*>(_target);

        if(widget == nullptr)
        {
            return false;
        }

        const auto* const parent = widget->parentWidget();

        if(parent == nullptr)
        {
            return false;
        }

        // This will make the widget transparent for parent->childAt() method and will restore visibility when destroyed
        widget_hider hider(widget);

        // Find the child widget now that the widget is "transparent"
        auto* const child = parent->childAt(parent->mapFromGlobal(_global_pos));

        // If the child is the widget itself, then the event should not be forwarded to avoid infinite loop
        if(child == nullptr || child == widget)
        {
            return false;
        }

        // Forward the event to the child
        if(!qApp->sendEvent(child, _event))
        {
            return false;
        }

        return true;
    }
};

/// Event filter which resizes the overlay if its parent size changed
class overlay_resize_filter : public QObject
{
private:

    QPointer<QWidget> m_child {nullptr}; // The widget to be resized
    coord m_x;
    coord m_y;
    coord m_width;
    coord m_height;

public:

    overlay_resize_filter(
        QWidget* _child,
        coord _x,
        coord _y,
        coord _width,
        coord _height
    ) :
        QObject(_child),
        m_child(_child),
        m_x(_x),
        m_y(_y),
        m_width(_width),
        m_height(_height)
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _target, QEvent* _event) override
    {
        if(_event->type() == QEvent::Resize && !m_child.isNull())
        {
            auto& resize_event = static_cast<QResizeEvent&>(*_event);
            if(m_width.value > 0 && m_width.relative)
            {
                m_child->setFixedWidth(calculate_size(m_width, resize_event.size().width()));
            }

            if(m_height.value > 0 && m_height.relative)
            {
                m_child->setFixedHeight(calculate_size(m_height, resize_event.size().height()));
            }

            if((m_x.value <= 0 && m_x.negative) || m_x.relative || (m_y.value <= 0 && m_y.negative)
               || m_y.relative)
            {
                int x = calculate_offset(m_x, resize_event.size().width(), m_child->width());
                int y = calculate_offset(m_y, resize_event.size().height(), m_child->height());
                m_child->move(x, y);
            }

            m_child->adjustSize();
        }

        return QObject::eventFilter(_target, _event);
    }
};

//------------------------------------------------------------------------------

void overlay::create_layout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parent_container);
    const QString q_id       = QString::fromStdString(_id);
    auto* const qt_container = m_parent_container->get_qt_container();

    qt_container->setObjectName(q_id);

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_parent_container->set_layout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* const viewport = new QWidget(qt_container);
    layout->addWidget(viewport);
    auto viewport_container = ui::qt::container::widget::make();
    viewport_container->set_qt_container(viewport);
    m_sub_views.push_back(viewport_container);

    const auto& views = this->views();

    for(std::size_t i = 1 ; i < views.size() ; i++)
    {
        const view& view   = views[i];
        auto* const widget = new QWidget(qt_container);

        if(view.width.value > 0)
        {
            widget->setFixedWidth(calculate_size(view.width, qt_container->width()));
        }

        if(view.height.value > 0)
        {
            widget->setFixedHeight(calculate_size(view.height, qt_container->height()));
        }

        if(view.min_width > 0)
        {
            widget->setMinimumWidth(view.min_width);
        }

        if(view.min_height > 0)
        {
            widget->setMinimumHeight(view.min_height);
        }

        int x = calculate_offset(view.x, qt_container->width(), widget->width());
        int y = calculate_offset(view.y, qt_container->height(), widget->height());
        widget->move(x, y);
        if((view.x.value <= 0 && view.x.negative) || view.x.relative
           || (view.y.value <= 0 && view.y.negative) || view.y.relative
           || (view.width.value > 0 && view.width.relative) || (view.height.value > 0 && view.height.relative))
        {
            qt_container->installEventFilter(
                new overlay_resize_filter(
                    widget,
                    view.x,
                    view.y,
                    view.width,
                    view.height
                )
            );
        }

        // This will allow to forward the events to the scene under the overlay to not confuse the user, since the
        // overlay "container" is transparent
        // Additionally, empty layout will make the widget transparent to mouse events
        widget->installEventFilter(new event_forwarder(widget));

        auto widget_container = ui::qt::container::widget::make();
        widget_container->set_qt_container(widget);
        if(!view.visible)
        {
            widget_container->set_visible(false);
        }

        m_sub_views.push_back(widget_container);
    }
}

//-----------------------------------------------------------------------------

void overlay::destroy_layout()
{
    this->destroy_sub_views();
    m_parent_container->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
