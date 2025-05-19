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
    m_tickLabels = _labels;
    m_min        = 0;
    m_max        = static_cast<int>(_labels.size()) - 1;

    QSignalBlocker block(m_slider.get());
    m_slider->setRange(m_min, m_max);
    update();
}

//-----------------------------------------------------------------------------
void tickmarks_slider::set_current_tick(int _tick)
{
    _tick = std::clamp(_tick, m_min, m_max);
    if(_tick != m_currentTick)
    {
        m_currentTick = _tick;
        update();
    }
}

//-----------------------------------------------------------------------------
void tickmarks_slider::paintEvent(QPaintEvent* /*event*/)
{
    if(m_tickLabels.empty())
    {
        return;
    }

    QPainter painter(this);
    painter.setPen(Qt::white);

    const QFontMetrics& metrics     = painter.fontMetrics();
    const std::string longest_label = *std::max_element(
        m_tickLabels.begin(),
        m_tickLabels.end(),
        [](const std::string& _a, const std::string& _b)
        {
            return _a.size() < _b.size();
        });
    const int padding = metrics.horizontalAdvance(
        QString::fromStdString(longest_label)
                        ) / 2 + 4;

    const int available_width = width() - 2 * padding;
    const int tick_count      = static_cast<int>(m_tickLabels.size());
    const int center_x        = width() / 2;

    const int y_bottom = height();
    const int y_top    = height() - 20;
    int span           = (tick_count > 1 ? tick_count - 1 : 1);
    int offset_x       = padding
                         + (available_width * m_currentTick) / span - center_x;
    for(int idx = 0 ; idx < tick_count ; ++idx)
    {
        int x_pos = padding + (available_width * idx) / span - offset_x;
        if(x_pos < 0 || x_pos > width())
        {
            continue;
        }

        bool is_current = (idx == m_currentTick);
        int line_top    = is_current ? y_top : (y_bottom - 15);
        painter.drawLine(x_pos, y_bottom, x_pos, line_top);
    }

    QString current_label = QString::fromStdString(
        m_tickLabels[static_cast<size_t>(m_currentTick)]
    );
    int text_width = metrics.horizontalAdvance(current_label);
    painter.drawText(
        center_x - text_width / 2,
        y_top - 10,
        current_label
    );
}

//------------------------------------------------------------------------------
void tickmarks_slider::mousePressEvent(QMouseEvent* _event)
{
    if(_event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(_event);
        return;
    }

    m_dragging = true;
    update_tick_from_pos(_event->pos().x());
}

//-------------------------------------------------------------------------------
void tickmarks_slider::mouseMoveEvent(QMouseEvent* _event)
{
    if(m_dragging && ((_event->buttons() & Qt::LeftButton) != 0))
    {
        update_tick_from_pos(_event->pos().x());
        _event->accept();
    }
    else
    {
        QWidget::mouseMoveEvent(_event);
    }
}

//---------------------------------------------------------------------------------
void tickmarks_slider::mouseReleaseEvent(QMouseEvent* _event)
{
    if(m_dragging && _event->button() == Qt::LeftButton)
    {
        update_tick_from_pos(_event->pos().x());
        m_dragging = false;
        _event->accept();
    }

    QWidget::mouseReleaseEvent(_event);
}

//------------------------------------------------------------------------------
void tickmarks_slider::update_tick_from_pos(const int _x_pos)
{
    const int range = m_max - m_min;
    if(range <= 0)
    {
        return;
    }

    const int ticks = range / m_interval;
    const int width = this->width();
    if(width == 0)
    {
        return;
    }

    const int idx      = std::clamp((_x_pos * ticks) / width, 0, ticks);
    const int new_tick = m_min + idx * m_interval;

    if(new_tick != m_slider->value())
    {
        m_slider->setValue(new_tick);
        set_current_tick(new_tick);
    }
}

//------------------------------------------------------------------------------

} //namespace sight::ui::qt::widget
