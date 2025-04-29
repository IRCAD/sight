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

#pragma once

#include <sight/ui/qt/config.hpp>

#include <QSlider>

namespace sight::ui::qt::widget
{

class SIGHT_UI_QT_CLASS_API_QT non_linear_slider : public QWidget
{
Q_OBJECT

public:

    SIGHT_UI_QT_API_QT non_linear_slider(QWidget* _parent = nullptr);

    SIGHT_UI_QT_API_QT void set_orientation(Qt::Orientation _orientation);

    SIGHT_UI_QT_API_QT void set_values(const std::vector<int>& _values);
    SIGHT_UI_QT_API_QT void set_value(int _value);
    SIGHT_UI_QT_API_QT void set_index(std::size_t _index);

    SIGHT_UI_QT_API_QT void set_tracking(bool _tracking);

    SIGHT_UI_QT_API_QT int value();
    SIGHT_UI_QT_API_QT std::size_t index();

    [[nodiscard]] SIGHT_UI_QT_API_QT std::size_t num_values() const;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

Q_SIGNALS:

    SIGHT_UI_QT_API_QT void value_changed(int _value);
    SIGHT_UI_QT_API_QT void range_changed(int _min, int _max);

protected:

    bool event(QEvent* _event) override;

private:

    std::vector<int> m_values;
    std::unique_ptr<QSlider> m_slider = std::make_unique<QSlider>(this);
};

}
