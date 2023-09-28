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

#include "NonLinearSlider.hpp"

#include <core/spy_log.hpp>

#include <QResizeEvent>

namespace sight::ui::qt::widget
{

NonLinearSlider::NonLinearSlider(QWidget* parent) :
    QWidget(parent)
{
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setSingleStep(1);
    m_slider->setPageStep(1);
    setSizePolicy(m_slider->sizePolicy().horizontalPolicy(), m_slider->sizePolicy().verticalPolicy());
    QObject::connect(
        m_slider.get(),
        &QAbstractSlider::valueChanged,
        [this](int v)
        {
            Q_EMIT valueChanged(m_values[std::size_t(v)]);
        });
}

//------------------------------------------------------------------------------

void NonLinearSlider::setOrientation(Qt::Orientation orientation)
{
    m_slider->setOrientation(orientation);
}

//------------------------------------------------------------------------------

void NonLinearSlider::setValues(const std::vector<int>& values)
{
    SIGHT_ASSERT("The values list should be sorted", std::ranges::is_sorted(values));
    m_values = values;
    m_slider->setRange(0, static_cast<int>(values.size() - 1));
    m_slider->setValue(0);
    Q_EMIT rangeChanged(values.front(), values.back());
    Q_EMIT valueChanged(value());
}

//------------------------------------------------------------------------------

void NonLinearSlider::setValue(int value)
{
    auto it = std::ranges::find(m_values, value);
    SIGHT_ASSERT("The value should exist in the list of possible values", it != m_values.end());
    m_slider->setValue(static_cast<int>(std::distance(m_values.begin(), it)));
}

//------------------------------------------------------------------------------

void NonLinearSlider::setTracking(bool tracking)
{
    m_slider->setTracking(tracking);
}

//------------------------------------------------------------------------------

int NonLinearSlider::value()
{
    SIGHT_ASSERT(
        "Internal slider value should be within range",
        0 <= m_slider->value() && m_slider->value() < int(m_values.size())
    );
    return m_values[static_cast<std::size_t>(m_slider->value())];
}

//------------------------------------------------------------------------------

std::size_t NonLinearSlider::index()
{
    return static_cast<std::size_t>(m_slider->value());
}

//------------------------------------------------------------------------------

QSize NonLinearSlider::minimumSizeHint() const
{
    return m_slider->minimumSizeHint();
}

//------------------------------------------------------------------------------

QSize NonLinearSlider::sizeHint() const
{
    return m_slider->sizeHint();
}

//------------------------------------------------------------------------------

bool NonLinearSlider::event(QEvent* event)
{
    if(auto* resizeEvent = dynamic_cast<QResizeEvent*>(event); resizeEvent != nullptr && m_slider != nullptr)
    {
        m_slider->resize(resizeEvent->size());
    }

    return QWidget::event(event);
}

} // namespace sight::ui::qt::widget
