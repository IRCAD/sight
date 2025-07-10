/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
struct castable
{
    //------------------------------------------------------------------------------

    template<typename U>
    static T* safe_cast(U* _p)
    {
        T* casted = dynamic_cast<T*>(_p);
        return casted;
    }
};

class handle : public range_slider::paintable,
               public castable<handle>
{
public:

    explicit handle(QWidget* _w) :
        paintable(_w),
        m_pen(Qt::gray),
        m_brush(Qt::lightGray)
    {
        m_tolerance = std::max(0, 10 - m_width);
    }

    //------------------------------------------------------------------------------

    void draw(QPainter& _painter, bool /*enabled*/) override
    {
        int height        = drawing_area().height() - 1;
        int top           = int(height * m_vertical_padding);
        int handle_height = height - 2 * top;
        _painter.setRenderHint(QPainter::Antialiasing);
        _painter.setPen(m_pen);
        _painter.setBrush(m_brush);
        _painter.drawRect(m_pos - half_width(), top, m_width, handle_height);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool pick(const QPoint& _point) const override
    {
        bool picked = false;

        int height        = drawing_area().height() - 1;
        int top           = int(height * m_vertical_padding);
        int handle_height = height - 2 * top;

        if(abs(_point.x() - m_pos) <= (half_width() + m_tolerance)
           && (top + handle_height) >= _point.y() && _point.y() >= top)
        {
            picked = true;
        }

        return picked;
    }

    //------------------------------------------------------------------------------

    int set_handle_size(int _size)
    {
        //handle size should be odd
        m_width = _size + ((_size + 1) % 2);
        return m_width;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int half_width() const
    {
        return m_width / 2;
    }

    //------------------------------------------------------------------------------

    int set_pos(const int& _pos)
    {
        int p = _pos;

        p = std::max(p, x_pos_min());
        p = std::min(p, x_pos_max());

        m_pos = p;

        return p;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int pos() const
    {
        return m_pos;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] int x_pos_min() const
    {
        return half_width();
    }

    //------------------------------------------------------------------------------

    int x_pos_max()
    {
        return drawing_area().width() - half_width() - 1;
    }

    //------------------------------------------------------------------------------

    double to_floating_pos(int _p)
    {
        int pos_min = x_pos_min();
        int pos_max = x_pos_max();
        return (double) (_p - pos_min) / (double) (pos_max - pos_min);
    }

    //------------------------------------------------------------------------------

    int from_floating_pos(double _p)
    {
        assert(0. <= _p && _p <= 1.);
        int pos_min = x_pos_min();
        int extend  = (x_pos_max()) - pos_min;

        return pos_min + (int) (_p * extend);
    }

    //------------------------------------------------------------------------------

    double floating_pos()
    {
        return to_floating_pos(pos());
    }

    //------------------------------------------------------------------------------

    void set_floating_pos(double _pos)
    {
        set_pos(from_floating_pos(_pos));
    }

private:

    int m_pos {0};
    int m_tolerance;
    int m_width {13};
    double m_vertical_padding {0.2};

    QPen m_pen;
    QBrush m_brush;
};

class window : public range_slider::paintable,
               public castable<window>
{
public:

    explicit window(QWidget* _w) :
        paintable(_w),
        m_pen(s_dark_blue),
        m_brush(s_cyan),
        m_reverse_pen(Qt::darkYellow),
        m_reverse_brush(Qt::yellow)
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
                pen   = m_reverse_pen;
                brush = m_reverse_brush;
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
        _painter.drawRect(m_left, 0, w, drawing_area().height() - 1);
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

    void set_pos(const int& _left, const int& _right)
    {
        m_left  = _left;
        m_right = _right;
    }

private:

    int m_left {0};
    int m_right {0};

    QPen m_pen;
    QBrush m_brush;
    QPen m_reverse_pen;
    QBrush m_reverse_brush;
};

//-----------------------------------------------------------------------------

range_slider::range_slider(QWidget* _parent) :
    QWidget(_parent),
    m_min_handle(new handle(this)),
    m_max_handle(new handle(this)),
    m_window(new class window (this))
{
    auto* min_h = new handle(this);
    auto* max_h = new handle(this);
    min_h->set_handle_size(m_handle_size);
    max_h->set_handle_size(m_handle_size);

    m_min_handle = min_h;
    m_max_handle = max_h;

    this->set_pos(m_min_value, m_max_value);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

range_slider::~range_slider()
{
    delete m_min_handle;
    delete m_max_handle;
    delete m_window;
}

//------------------------------------------------------------------------------

void range_slider::set_pos(double _min, double _max)
{
    handle* min_handle   = handle::safe_cast(m_min_handle);
    handle* max_handle   = handle::safe_cast(m_max_handle);
    class window* window = window::safe_cast(m_window);

    assert(min_handle && max_handle && window);

    min_handle->set_floating_pos(_min);
    max_handle->set_floating_pos(_max);

    int min = 0;
    int max = 0;
    min = min_handle->pos();
    max = max_handle->pos();
    window->set_pos(min, max);

    this->moved_to(_min, _max);
    this->update();
}

//------------------------------------------------------------------------------

void range_slider::move(int _delta)
{
    handle* min_handle   = handle::safe_cast(m_min_handle);
    handle* max_handle   = handle::safe_cast(m_max_handle);
    class window* window = window::safe_cast(m_window);

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
        low  = min_handle->set_pos(low - _delta);
        high = low + width;
        max_handle->set_pos(high);
    }
    else
    {
        high = max_handle->set_pos(high - _delta);
        low  = high - width;
        min_handle->set_pos(low);
    }

    window->set_pos(low, high);
}

//------------------------------------------------------------------------------

bool range_slider::moved_to(double _min, double _max)
{
    bool changed = m_min_value != _min || m_max_value != _max;
    if(changed)
    {
        m_min_value = _min;
        m_max_value = _max;
        Q_EMIT slider_range_changed(m_min_value, m_max_value);
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
    rect.setLeft(rect.left() + m_handle_size / 2);
    rect.setRight(rect.right() - m_handle_size / 2);
    painter.fillRect(rect, QColor(86, 90, 94));

    painter.setBrush(s_cyan);
    m_window->draw(painter, enabled);

    painter.setBrush(Qt::gray);
    painter.setPen(Qt::darkGreen);
    m_min_handle->draw(painter, enabled);

    painter.setPen(Qt::darkRed);
    m_max_handle->draw(painter, enabled);
}

//------------------------------------------------------------------------------

void range_slider::mouseMoveEvent(QMouseEvent* _event)
{
    if(m_current != nullptr)
    {
        handle* min_handle     = handle::safe_cast(m_min_handle);
        handle* max_handle     = handle::safe_cast(m_max_handle);
        class window* window   = window::safe_cast(m_window);
        handle* current_handle = nullptr;

        if((current_handle = handle::safe_cast(m_current)) != nullptr)
        {
            int old_pos = current_handle->pos();
            int new_pos = _event->pos().x();
            current_handle->set_pos(new_pos);

            if(!m_allow_min_greater_than_max
               && min_handle->floating_pos() + m_minimum_min_max_delta >= max_handle->floating_pos())
            {
                current_handle->set_pos(old_pos);
            }

            window->set_pos(min_handle->pos(), max_handle->pos());
        }
        else if(window::safe_cast(m_current) != nullptr)
        {
            QPoint delta = m_press_pos - _event->pos();

            min_handle->set_pos(m_press_min);
            max_handle->set_pos(m_press_max);
            window->set_pos(min_handle->pos(), max_handle->pos());
            this->move(delta.x());
        }

        double min = min_handle->floating_pos();
        double max = max_handle->floating_pos();
        if(this->moved_to(min, max))
        {
            Q_EMIT slider_range_edited(min, max);
        }

        this->update();
    }
}

//------------------------------------------------------------------------------

void range_slider::mousePressEvent(QMouseEvent* _event)
{
    handle* min_handle = handle::safe_cast(m_min_handle);
    handle* max_handle = handle::safe_cast(m_max_handle);
//    Window *window     = Window::safeCast(m_window);

    m_press_pos = _event->pos();
    m_press_min = min_handle->pos();
    m_press_max = max_handle->pos();

    if(m_max_handle->pick(m_press_pos))
    {
        m_current = m_max_handle;
    }
    else if(m_min_handle->pick(m_press_pos))
    {
        m_current = m_min_handle;
    }
    else if(m_window->pick(m_press_pos))
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
    handle* min_handle   = handle::safe_cast(m_min_handle);
    handle* max_handle   = handle::safe_cast(m_max_handle);
    class window* window = window::safe_cast(m_window);

    const auto angle_delta = _event->angleDelta();

    int low  = min_handle->pos();
    int high = max_handle->pos();

    if(angle_delta.y() != 0)
    {
        int delta = int(this->size().width() / (((double) angle_delta.y()) / 4.));
        if(!m_allow_min_greater_than_max)
        {
            int diff     = (high - low);
            int min_diff = min_handle->from_floating_pos(m_minimum_min_max_delta);
            delta = std::max(delta, -(diff - min_diff) / 2);
        }

        low  = min_handle->set_pos(low - delta);
        high = max_handle->set_pos(high + delta);
        window->set_pos(low, high);
    }
    else if(angle_delta.x() != 0)
    {
        int delta = int(this->size().width() / (((double) angle_delta.x()) / 4.));
        this->move(delta);
    }

    double min = min_handle->floating_pos();
    double max = max_handle->floating_pos();
    if(this->moved_to(min, max))
    {
        Q_EMIT slider_range_edited(min, max);
    }

    this->update();
}

//------------------------------------------------------------------------------

void range_slider::resizeEvent(QResizeEvent* /*event*/)
{
    this->set_pos(m_min_value, m_max_value);
}

} // namespace sight::ui::qt::widget
