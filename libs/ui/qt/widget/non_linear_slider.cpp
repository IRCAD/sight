/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "non_linear_slider.hpp"

#include <core/spy_log.hpp>

#include <QResizeEvent>

namespace sight::ui::qt::widget
{

non_linear_slider::non_linear_slider(QWidget* _parent) :
    QWidget(_parent)
{
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setSingleStep(1);
    m_slider->setPageStep(1);
    setSizePolicy(m_slider->sizePolicy().horizontalPolicy(), m_slider->sizePolicy().verticalPolicy());
    QObject::connect(
        m_slider.get(),
        &QAbstractSlider::valueChanged,
        [this](int _v)
        {
            Q_EMIT value_changed(m_values[std::size_t(_v)]);
        });
}

//------------------------------------------------------------------------------

void non_linear_slider::set_orientation(Qt::Orientation _orientation)
{
    m_slider->setOrientation(_orientation);
}

//------------------------------------------------------------------------------

void non_linear_slider::set_values(const std::vector<int>& _values)
{
    SIGHT_ASSERT("The values list should be sorted", std::ranges::is_sorted(_values));
    m_values = _values;
    m_slider->setRange(0, static_cast<int>(_values.size() - 1));
    m_slider->setValue(0);
    Q_EMIT range_changed(_values.front(), _values.back());
    Q_EMIT value_changed(value());
}

//------------------------------------------------------------------------------

void non_linear_slider::set_value(int _value)
{
    auto it = std::ranges::find(m_values, _value);
    SIGHT_ASSERT("The value should exist in the list of possible values", it != m_values.end());
    m_slider->setValue(static_cast<int>(std::distance(m_values.begin(), it)));
}

//------------------------------------------------------------------------------

void non_linear_slider::set_tracking(bool _tracking)
{
    m_slider->setTracking(_tracking);
}

//------------------------------------------------------------------------------

int non_linear_slider::value()
{
    SIGHT_ASSERT(
        "Internal slider value should be within range",
        0 <= m_slider->value() && m_slider->value() < int(m_values.size())
    );
    return m_values[static_cast<std::size_t>(m_slider->value())];
}

//------------------------------------------------------------------------------

std::size_t non_linear_slider::index()
{
    return static_cast<std::size_t>(m_slider->value());
}

//------------------------------------------------------------------------------

QSize non_linear_slider::minimumSizeHint() const
{
    return m_slider->minimumSizeHint();
}

//------------------------------------------------------------------------------

QSize non_linear_slider::sizeHint() const
{
    return m_slider->sizeHint();
}

//------------------------------------------------------------------------------

bool non_linear_slider::event(QEvent* _event)
{
    if(auto* resize_event = dynamic_cast<QResizeEvent*>(_event); resize_event != nullptr && m_slider != nullptr)
    {
        m_slider->resize(resize_event->size());
    }

    return QWidget::event(_event);
}

} // namespace sight::ui::qt::widget
