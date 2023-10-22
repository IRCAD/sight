/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/qt/widget/range_slider.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include <cassert>

namespace sight::ui::qt::widget
{

static QColor s_cyan      = QColor(104, 151, 187);
static QColor s_dark_blue = QColor(102, 108, 123);

template<typename T>
struct Castable
{
    //------------------------------------------------------------------------------

    template<typename U>
    static T* safeCast(U* _p)
    {
        T* casted = dynamic_cast<T*>(_p);
        return casted;
    }
};

class Handle : public range_slider::Paintable,
               public Castable<Handle>
{
public:

    explicit Handle(QWidget* _w) :
        Paintable(_w),
        m_pen(Qt::gray),
        m_brush(Qt::lightGray)
    {
        m_tolerance = std::max(0, 10 - m_width);
    }

    //------------------------------------------------------------------------------

    void draw(QPainter& _painter, bool /*enabled*/) override
    {
        int height        = drawingArea().height() - 1;
        int top           = int(height * m_verticalPadding);
        int handle_height = height - 2 * top;
        _painter.setRenderHint(QPainter::Antialiasing);
        _painter.setPen(m_pen);
        _painter.setBrush(m_brush);
        _painter.drawRect(m_pos - halfWidth(), top, m_width, handle_height);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool pick(const QPoint& _point) const override
    {
        bool picked = false;

        int height        = drawingArea().height() - 1;
        int top           = int(height * m_verticalPadding);
        int handle_height = height - 2 * top;

        if(abs(_point.x() - m_pos) <= (halfWidth() + m_tolerance)
           && (top + handle_height) >= _point.y() && _point.y() >= top)
        {
            picked = true;
        }

        return picked;
    }

    //------------------------------------------------------------------------------

    int setHandleSize(int _size)
    {
        //handle size should be odd
        m_width = _size + ((_size + 1) % 2);
        return m_width;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int halfWidth() const
    {
        return m_width / 2;
    }

    //------------------------------------------------------------------------------

    int setPos(const int& _pos)
    {
        int p = _pos;

        p = std::max(p, xPosMin());
        p = std::min(p, xPosMax());

        m_pos = p;

        return p;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int pos() const
    {
        return m_pos;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int xPosMin() const
    {
        return halfWidth();
    }

    //------------------------------------------------------------------------------

    int xPosMax()
    {
        return drawingArea().width() - halfWidth() - 1;
    }

    //------------------------------------------------------------------------------

    double toFloatingPos(int _p)
    {
        int pos_min = xPosMin();
        int pos_max = xPosMax();
        return (double) (_p - pos_min) / (double) (pos_max - pos_min);
    }

    //------------------------------------------------------------------------------

    int fromFloatingPos(double _p)
    {
        assert(0. <= _p && _p <= 1.);
        int pos_min = xPosMin();
        int extend  = (xPosMax()) - pos_min;

        return pos_min + (int) (_p * extend);
    }

    //------------------------------------------------------------------------------

    double floatingPos()
    {
        return toFloatingPos(pos());
    }

    //------------------------------------------------------------------------------

    void setFloatingPos(double _pos)
    {
        setPos(fromFloatingPos(_pos));
    }

private:

    int m_pos {0};
    int m_tolerance;
    int m_width {13};
    double m_verticalPadding {0.2};

    QPen m_pen;
    QBrush m_brush;
};

class Window : public range_slider::Paintable,
               public Castable<Window>
{
public:

    explicit Window(QWidget* _w) :
        Paintable(_w),
        m_pen(s_dark_blue),
        m_brush(s_cyan),
        m_reversePen(Qt::darkYellow),
        m_reverseBrush(Qt::yellow)
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int width() const
    {
        return m_right - m_left;
    }

    //------------------------------------------------------------------------------

    void draw(QPainter& _painter, bool _enabled) override
    {
        int w = this->width();
        QPen pen;
        QBrush brush;

        if(_enabled)
        {
            if(w < 0)
            {
                pen   = m_reversePen;
                brush = m_reverseBrush;
            }
            else
            {
                pen   = m_pen;
                brush = m_brush;
            }
        }
        else
        {
            pen   = QPen(QColor(86, 90, 94));
            brush = QBrush(QColor(86, 90, 94));
        }

        _painter.setPen(pen);
        _painter.setBrush(brush);
        _painter.setRenderHint(QPainter::Antialiasing);
        _painter.drawRect(m_left, 0, w, drawingArea().height() - 1);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool pick(const QPoint& _point) const override
    {
        bool picked = false;
        int min     = std::min(m_left, m_right);
        int max     = std::max(m_left, m_right);
        picked = min <= _point.x() && _point.x() <= max;
        return picked;
    }

    //------------------------------------------------------------------------------

    void setPos(const int& _left, const int& _right)
    {
        m_left  = _left;
        m_right = _right;
    }

private:

    int m_left {0};
    int m_right {0};

    QPen m_pen;
    QBrush m_brush;
    QPen m_reversePen;
    QBrush m_reverseBrush;
};

//-----------------------------------------------------------------------------

range_slider::range_slider(QWidget* _parent) :
    QWidget(_parent),
    m_minHandle(new Handle(this)),
    m_maxHandle(new Handle(this)),
    m_window(new Window(this))
{
    auto* min_h = new Handle(this);
    auto* max_h = new Handle(this);
    min_h->setHandleSize(m_handleSize);
    max_h->setHandleSize(m_handleSize);

    m_minHandle = min_h;
    m_maxHandle = max_h;

    this->setPos(m_minValue, m_maxValue);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

range_slider::~range_slider()
{
    delete m_minHandle;
    delete m_maxHandle;
    delete m_window;
}

//------------------------------------------------------------------------------

void range_slider::setPos(double _min, double _max)
{
    Handle* min_handle = Handle::safeCast(m_minHandle);
    Handle* max_handle = Handle::safeCast(m_maxHandle);
    Window* window     = Window::safeCast(m_window);

    assert(min_handle && max_handle && window);

    min_handle->setFloatingPos(_min);
    max_handle->setFloatingPos(_max);

    int min = 0;
    int max = 0;
    min = min_handle->pos();
    max = max_handle->pos();
    window->setPos(min, max);

    this->movedTo(_min, _max);
    this->update();
}

//------------------------------------------------------------------------------

void range_slider::move(int _delta)
{
    Handle* min_handle = Handle::safeCast(m_minHandle);
    Handle* max_handle = Handle::safeCast(m_maxHandle);
    Window* window     = Window::safeCast(m_window);

    assert(min_handle && max_handle && window);

    int low   = 0;
    int high  = 0;
    int width = 0;
    int dir   = 0;
    dir = ((min_handle->pos() < max_handle->pos()) ? 1 : -1);
    bool moving_right = (_delta < 0);

    low   = min_handle->pos();
    high  = max_handle->pos();
    width = high - low;

    if((moving_right && dir < 0)
       || (!moving_right && dir > 0))
    {
        low  = min_handle->setPos(low - _delta);
        high = low + width;
        max_handle->setPos(high);
    }
    else
    {
        high = max_handle->setPos(high - _delta);
        low  = high - width;
        min_handle->setPos(low);
    }

    window->setPos(low, high);
}

//------------------------------------------------------------------------------

bool range_slider::movedTo(double _min, double _max)
{
    bool changed = m_minValue != _min || m_maxValue != _max;
    if(changed)
    {
        m_minValue = _min;
        m_maxValue = _max;
        Q_EMIT sliderRangeChanged(m_minValue, m_maxValue);
    }

    return changed;
}

//------------------------------------------------------------------------------

void range_slider::paintEvent(QPaintEvent* /*event*/)
{
    bool enabled = this->isEnabled();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    rect.setLeft(rect.left() + m_handleSize / 2);
    rect.setRight(rect.right() - m_handleSize / 2);
    painter.fillRect(rect, QColor(86, 90, 94));

    painter.setBrush(s_cyan);
    m_window->draw(painter, enabled);

    painter.setBrush(Qt::gray);
    painter.setPen(Qt::darkGreen);
    m_minHandle->draw(painter, enabled);

    painter.setPen(Qt::darkRed);
    m_maxHandle->draw(painter, enabled);
}

//------------------------------------------------------------------------------

void range_slider::mouseMoveEvent(QMouseEvent* _event)
{
    if(m_current != nullptr)
    {
        Handle* min_handle     = Handle::safeCast(m_minHandle);
        Handle* max_handle     = Handle::safeCast(m_maxHandle);
        Window* window         = Window::safeCast(m_window);
        Handle* current_handle = nullptr;

        if((current_handle = Handle::safeCast(m_current)) != nullptr)
        {
            int old_pos = current_handle->pos();
            int new_pos = _event->pos().x();
            current_handle->setPos(new_pos);

            if(!m_allowMinGreaterThanMax
               && min_handle->floatingPos() + m_minimumMinMaxDelta >= max_handle->floatingPos())
            {
                current_handle->setPos(old_pos);
            }

            window->setPos(min_handle->pos(), max_handle->pos());
        }
        else if(Window::safeCast(m_current) != nullptr)
        {
            QPoint delta = m_pressPos - _event->pos();

            min_handle->setPos(m_pressMin);
            max_handle->setPos(m_pressMax);
            window->setPos(min_handle->pos(), max_handle->pos());
            this->move(delta.x());
        }

        double min = min_handle->floatingPos();
        double max = max_handle->floatingPos();
        if(this->movedTo(min, max))
        {
            Q_EMIT sliderRangeEdited(min, max);
        }

        this->update();
    }
}

//------------------------------------------------------------------------------

void range_slider::mousePressEvent(QMouseEvent* _event)
{
    Handle* min_handle = Handle::safeCast(m_minHandle);
    Handle* max_handle = Handle::safeCast(m_maxHandle);
//    Window *window     = Window::safeCast(m_window);

    m_pressPos = _event->pos();
    m_pressMin = min_handle->pos();
    m_pressMax = max_handle->pos();

    if(m_maxHandle->pick(m_pressPos))
    {
        m_current = m_maxHandle;
    }
    else if(m_minHandle->pick(m_pressPos))
    {
        m_current = m_minHandle;
    }
    else if(m_window->pick(m_pressPos))
    {
        m_current = m_window;
    }
}

//------------------------------------------------------------------------------

void range_slider::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    m_current = nullptr;
}

//------------------------------------------------------------------------------

void range_slider::wheelEvent(QWheelEvent* _event)
{
    Handle* min_handle = Handle::safeCast(m_minHandle);
    Handle* max_handle = Handle::safeCast(m_maxHandle);
    Window* window     = Window::safeCast(m_window);

    const auto angle_delta = _event->angleDelta();

    int low  = min_handle->pos();
    int high = max_handle->pos();

    if(angle_delta.y() != 0)
    {
        int delta = int(this->size().width() / (((double) angle_delta.y()) / 4.));
        if(!m_allowMinGreaterThanMax)
        {
            int diff     = (high - low);
            int min_diff = min_handle->fromFloatingPos(m_minimumMinMaxDelta);
            delta = std::max(delta, -(diff - min_diff) / 2);
        }

        low  = min_handle->setPos(low - delta);
        high = max_handle->setPos(high + delta);
        window->setPos(low, high);
    }
    else if(angle_delta.x() != 0)
    {
        int delta = int(this->size().width() / (((double) angle_delta.x()) / 4.));
        this->move(delta);
    }

    double min = min_handle->floatingPos();
    double max = max_handle->floatingPos();
    if(this->movedTo(min, max))
    {
        Q_EMIT sliderRangeEdited(min, max);
    }

    this->update();
}

//------------------------------------------------------------------------------

void range_slider::resizeEvent(QResizeEvent* /*event*/)
{
    this->setPos(m_minValue, m_maxValue);
}

} // namespace sight::ui::qt::widget
