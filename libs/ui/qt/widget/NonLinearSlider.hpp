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

#pragma once

#include <ui/qt/config.hpp>

#include <QSlider>

namespace sight::ui::qt::widget
{

class UI_QT_CLASS_API_QT NonLinearSlider : public QWidget
{
Q_OBJECT

public:

    UI_QT_API_QT NonLinearSlider(QWidget* parent = nullptr);

    UI_QT_API_QT void setOrientation(Qt::Orientation orientation);

    UI_QT_API_QT void setValues(const std::vector<int>& values);
    UI_QT_API_QT void setValue(int value);

    UI_QT_API_QT void setTracking(bool tracking);

    UI_QT_API_QT int value();
    UI_QT_API_QT std::size_t index();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

Q_SIGNALS:

    UI_QT_API_QT void valueChanged(int value);
    UI_QT_API_QT void rangeChanged(int min, int max);

protected:

    bool event(QEvent* event) override;

private:

    std::vector<int> m_values;
    std::unique_ptr<QSlider> m_slider = std::make_unique<QSlider>(this);
};

}
