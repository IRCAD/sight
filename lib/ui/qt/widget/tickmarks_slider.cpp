/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <QPropertyAnimation>
#include <QSlider>
#include <QString>
namespace sight::ui::qt::widget
{

tickmarks_slider::tickmarks_slider(QWidget* _parent) :
    QWidget(_parent),
    m_slider(std::make_unique<QSlider>(Qt::Horizontal, this))
{
    m_slider->setSingleStep(1);
    m_slider->setPageStep(1);
    m_slider->setRange(0, static_cast<int>(m_values.size()) - 1);
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
    m_drag_anim = new QPropertyAnimation(this, "animated_tick", this);
}

//---------------------------------------------------------------------------

void tickmarks_slider::set_range(int _min, int _max)
{
    m_min = _min;
    m_max = _max;
    update();
}

//---------------------------------------------------------------------------
void tickmarks_slider::set_tick_interval(int _interval)
{
    m_interval = _interval;
    update();
}

//---------------------------------------------------------------------------
void tickmarks_slider::set_tick_labels(const std::vector<std::string>& _labels)
{
    m_tick_labels = _labels;
    m_min         = 0;
    m_max         = static_cast<int>(_labels.size()) - 1;

    QSignalBlocker block(m_slider.get());
    m_slider->setRange(m_min, m_max);
    update();
}

//-----------------------------------------------------------------------------
void tickmarks_slider::set_current_tick(int _tick)
{
    _tick = std::clamp(_tick, m_min, m_max);
    if(_tick == m_current_tick)
    {
        return;
    }

    m_current_tick = _tick;

    if(m_dragging)
    {
        m_drag_anim->setStartValue(m_animated_tick);
        m_drag_anim->setEndValue(static_cast<double>(_tick));
        m_drag_anim->setDuration(600);
        m_drag_anim->setEasingCurve(QEasingCurve::InOutQuad);
        m_drag_anim->start();
    }
    else
    {
        m_animated_tick = _tick;
        update();
    }
}

//-----------------------------------------------------------------------------

void tickmarks_slider::paintEvent(QPaintEvent* /*event*/)
{
    if(m_tick_labels.empty())
    {
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::white);

    const QFontMetrics  fm = painter.fontMetrics();
    const int padding      = fm.horizontalAdvance(
        QString::fromStdString(
            *std::max_element(
                m_tick_labels.begin(),
                m_tick_labels.end(),
                [](const auto& _a, const auto& _b){return _a.size() < _b.size();})
        )
                             ) / 2 + 4;

    const int width_available = width() - 2 * padding;
    const int tick_count      = static_cast<int>(m_tick_labels.size());
    const int span            = tick_count > 1 ? tick_count - 1 : 1;
    const int y_bottom        = height();
    const int y_top           = height();
    const int center_x        = width() / 2;
    const  double offset_x    = padding
                                + (width_available * m_animated_tick) / span
                                - center_x;
    const int current_index = std::clamp<int>(static_cast<int>(m_animated_tick), m_min, m_max);
    for(int i = 0 ; i < tick_count ; ++i)
    {
        int x = padding + (width_available * i) / span
                - static_cast<int>(offset_x);

        if(x < 0 || x > width())
        {
            continue;
        }

        const double distance           = std::abs(i - m_animated_tick);
        const double long_tick_distance = 3;
        // Compute tick length: 25 px at distance 0, shrinking linearly to 10 px
        int tick_length = 25 - static_cast<int>(15 * (std::min(distance, long_tick_distance) / long_tick_distance));

        painter.drawLine(x, y_bottom, x, y_bottom - tick_length);
    }

    QString label = QString::fromStdString(m_tick_labels[static_cast<size_t>(current_index)]);
    painter.drawText(QRect(0, y_top - 50, width(), 20), Qt::AlignHCenter, label);
}

//------------------------------------------------------------------------------
void tickmarks_slider::mousePressEvent(QMouseEvent* _event)
{
    if(_event->button() == Qt::LeftButton)
    {
        m_press_pos = _event->pos();
        m_last_x    = _event->pos().x();
        m_dragging  = false;
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

    const int dx = _event->pos().x() - m_last_x;
    if((_event->pos() - m_press_pos).manhattanLength() >= 20)
    {
        m_dragging = true;
    }

    if(m_dragging)
    {
        if(dx <= -step_px && m_current_tick < m_max)
        {
            m_last_x = _event->pos().x();
            set_current_tick(m_current_tick + m_interval);
            m_slider->setValue(m_current_tick);
        }
        else if(dx >= step_px && m_current_tick > m_min)
        {
            m_last_x = _event->pos().x();
            set_current_tick(m_current_tick - m_interval);
            m_slider->setValue(m_current_tick);
        }

        _event->accept();
    }
}

//-------------------------------------------------------------------------------
void tickmarks_slider::mouseReleaseEvent(QMouseEvent* _event)
{
    if(_event->button() == Qt::LeftButton)
    {
        m_released = true;

        m_dragging = false;
        _event->accept();
    }

    QWidget::mouseReleaseEvent(_event);
}

//------------------------------------------------------------------------------

} //namespace sight::ui::qt::widget
