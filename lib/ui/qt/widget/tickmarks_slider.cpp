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

#include "tickmarks_slider.hpp"

#include <core/spy_log.hpp>

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSlider>

namespace sight::ui::qt::widget
{

tickmarks_slider::tickmarks_slider(QWidget* _parent) :
    QWidget(_parent),
    m_slider(std::make_unique<QSlider>(Qt::Horizontal, this)),
    m_drag_anim(new QPropertyAnimation(this, "animated_tick", this))
{
    m_slider->setSingleStep(1);
    m_slider->setPageStep(1);
    m_slider->setValue(0);

    m_slider->setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(
        m_slider->sizePolicy().horizontalPolicy(),
        m_slider->sizePolicy().verticalPolicy()
    );

    QObject::connect(
        m_slider.get(),
        &QSlider::valueChanged,
        this,
        [this](const int _value)
        {
            set_current_tick(_value);
            Q_EMIT value_changed(_value);
        });

    m_slider->setObjectName("tickmarks_slider");
    m_slider->setProperty("class", "tickmarks_slider");
}

//---------------------------------------------------------------------------

void tickmarks_slider::set_range(int _min, int _max)
{
    QSignalBlocker block(m_slider.get());
    m_slider->setRange(_min, _max);
    update();
}

//------------------------------------------------------------------------------

double tickmarks_slider::animated_tick() const
{
    return m_animated_tick;
}

//------------------------------------------------------------------------------

void tickmarks_slider::set_animated_tick(double _value)
{
    m_animated_tick = _value;

    if(const auto tick = static_cast<int>(std::round(_value)); tick != m_current_tick)
    {
        m_current_tick = tick;
        m_slider->setValue(tick);
        Q_EMIT value_changed(tick);
    }

    update();
}

//------------------------------------------------------------------------------

const std::vector<std::string>& tickmarks_slider::tick_values() const
{
    return m_tick_values;
}

//---------------------------------------------------------------------------

void tickmarks_slider::set_tick_values(const std::vector<std::string>& _values)
{
    m_tick_values = _values;
    const int min = 0;
    const int max = std::clamp(static_cast<int>(_values.size()) - 1, 0, std::numeric_limits<int>::max());

    QSignalBlocker block(m_slider.get());
    m_slider->setRange(min, max);
    update();
}

//------------------------------------------------------------------------------

int tickmarks_slider::current_tick() const
{
    return m_current_tick;
}

//------------------------------------------------------------------------------

void tickmarks_slider::set_current_tick(int _tick)
{
    const auto min = m_slider->minimum();
    const auto max = m_slider->maximum();

    const auto clamped_tick = std::clamp(_tick, min, max);
    if(clamped_tick == m_current_tick)
    {
        return;
    }

    set_animated_tick(clamped_tick);
}

//------------------------------------------------------------------------------

void tickmarks_slider::set_current_value(const std::string& _value)
{
    const auto it = std::ranges::find(m_tick_values, _value);
    if(it != m_tick_values.end())
    {
        set_current_tick(static_cast<int>(std::distance(m_tick_values.begin(), it)));
    }
}

//------------------------------------------------------------------------------

[[nodiscard]] std::string tickmarks_slider::current_value() const
{
    return m_tick_values[std::size_t(m_current_tick)];
}

//-----------------------------------------------------------------------------

void tickmarks_slider::paintEvent(QPaintEvent* /*event*/)
{
    if(m_tick_values.empty())
    {
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::white);
    const QFontMetrics  fm = painter.fontMetrics();
    const int padding      = fm.horizontalAdvance(
        QString::fromStdString(
            *std::ranges::max_element(
                m_tick_values,
                [](const auto& _a, const auto& _b)
        {
            return _a.size() < _b.size();
        })
        )
                             ) / 2 + 4;

    const int width_available = width() + 2 * padding;
    const int tick_count      = static_cast<int>(m_tick_values.size());
    const int span            = 8;
    const int y_bottom        = height();
    const int y_top           = height();
    const int center_x        = width() / 2;
    const  double offset_x    = padding
                                + (width_available * m_animated_tick) / span
                                - center_x;

    for(int i = 0 ; i < tick_count ; ++i)
    {
        const int x = padding + (width_available * i) / span - static_cast<int>(offset_x);

        if(x < 0 || x > width())
        {
            continue;
        }

        // Determine tick length and set pen opacity based on proximity to the current tick.
        const double distance     = std::abs(i - m_animated_tick);
        const double max_distance = 2.0;
        const double ratio        = (std::min(distance, max_distance)) / max_distance;
        const int max_tick_length = 27;
        const int min_tick_length = 10;
        const int tick_length     = static_cast<int>(max_tick_length - (max_tick_length - min_tick_length) * ratio);
        if(distance < 0.5)
        {
            painter.setOpacity(1.0);
        }
        else if(distance < 1.5)
        {
            painter.setOpacity(0.8);
        }
        else
        {
            painter.setOpacity(0.4);
        }

        painter.drawLine(x, y_bottom, x, y_bottom - tick_length);
    }

    QPainter painter_label(this);
    const auto display_tick = std::clamp(
        static_cast<std::size_t>(std::round(m_animated_tick)),
        std::size_t(0),
        m_tick_values.size() - 1
    );
    const auto current_label = QString::fromStdString(m_tick_values[display_tick]);
    painter_label.drawText(QRect(0, y_top - 50, width(), 20), Qt::AlignHCenter, current_label);
}

//------------------------------------------------------------------------------
void tickmarks_slider::mousePressEvent(QMouseEvent* _event)
{
    if(_event->button() == Qt::LeftButton)
    {
        m_press_pos       = _event->pos();
        m_drag_start_tick = m_animated_tick;
        m_dragging        = false;
        _event->accept();
    }
    else
    {
        QWidget::mousePressEvent(_event);
    }
}

//-------------------------------------------------------------------------------
void tickmarks_slider::mouseMoveEvent(QMouseEvent* _event)
{
    if(!(_event->buttons() & Qt::LeftButton))
    {
        QWidget::mouseMoveEvent(_event);
        return;
    }

    if((_event->pos() - m_press_pos).manhattanLength() < 10)
    {
        return;
    }

    m_dragging = true;

    int w = width();
    if(w == 0)
    {
        return;
    }

    auto max_idx = double(m_tick_values.size() - 1);
    double dx    = _event->pos().x() - m_press_pos.x();

    const double sensitivity = 0.6;
    double delta             = -dx / w * max_idx * sensitivity;
    double raw               = m_drag_start_tick + delta;
    raw = std::clamp(raw, 0.0, max_idx);

    set_animated_tick(raw);
    _event->accept();
}

//-------------------------------------------------------------------------------

void tickmarks_slider::mouseReleaseEvent(QMouseEvent* _event)
{
    if(_event->button() != Qt::LeftButton)
    {
        QWidget::mouseReleaseEvent(_event);
        return;
    }

    if(!m_dragging)
    {
        _event->accept();
        return;
    }

    m_dragging = false;
    const int target = int(std::round(m_animated_tick));

    const double speed = 3.0;

    const double distance = std::abs(target - m_animated_tick);

    const int duration_ms = int((distance / speed) * 1000);

    m_drag_anim->setStartValue(m_animated_tick);
    m_drag_anim->setEndValue(double(target));
    m_drag_anim->setDuration(duration_ms);
    m_drag_anim->setEasingCurve(QEasingCurve::InOutQuad);
    connect(
        m_drag_anim,
        &QPropertyAnimation::finished,
        this,
        [this, target]{set_current_tick(target);});
    m_drag_anim->start();

    _event->accept();

    QWidget::mouseReleaseEvent(_event);
}

//------------------------------------------------------------------------------

void tickmarks_slider::set_current_value_property(QString _value)
{
    set_current_value(_value.toStdString());
}

//------------------------------------------------------------------------------

[[nodiscard]] QString tickmarks_slider::current_value_property() const
{
    return QString::fromStdString(current_value());
}

} //namespace sight::ui::qt::widget
