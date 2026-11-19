/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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
/// The values, in order: the inner QSlider moves over their indices. GUI tests read it to set a value by input.
Q_PROPERTY(QList<int> values READ value_list)

public:

    /// Creates a horizontal slider without values; its inner QSlider is named after it, as its "slider" part.
    SIGHT_UI_QT_API_QT explicit non_linear_slider(QWidget* _parent = nullptr);

    SIGHT_UI_QT_API_QT void set_orientation(Qt::Orientation _orientation);

    SIGHT_UI_QT_API_QT void set_values(const std::vector<int>& _values);
    SIGHT_UI_QT_API_QT void set_value(int _value);
    SIGHT_UI_QT_API_QT void set_index(std::size_t _index);

    SIGHT_UI_QT_API_QT void set_tracking(bool _tracking);

    SIGHT_UI_QT_API_QT int value();
    SIGHT_UI_QT_API_QT std::size_t index();

    [[nodiscard]] SIGHT_UI_QT_API_QT const std::vector<int>& values() const;
    /// The values as a QList, for the "values" property.
    [[nodiscard]] SIGHT_UI_QT_API_QT QList<int> value_list() const;
    [[nodiscard]] SIGHT_UI_QT_API_QT std::size_t num_values() const;

    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

Q_SIGNALS:

    SIGHT_UI_QT_API_QT void value_changed(int _value);
    SIGHT_UI_QT_API_QT void range_changed(int _min, int _max);

protected:

    bool event(QEvent* _event) override;

private:

    std::vector<int> m_values;
    std::unique_ptr<QSlider> m_slider = std::make_unique<QSlider>(this);
};

} // namespace sight::ui::qt::widget
