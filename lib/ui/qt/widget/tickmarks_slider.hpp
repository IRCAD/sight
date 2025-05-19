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

#pragma once

#include <sight/ui/qt/config.hpp>

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QSlider>
namespace sight::ui::qt::widget
{

/// This class allows to display a tickmarks widget. The tickmarks widget can be displayed over settings widgets.

class SIGHT_UI_QT_CLASS_API_QT tickmarks_slider : public QWidget
{
Q_OBJECT

public:

    /**
     * @brief tickmarks_slider widget constructor.
     * @param _parent : the parent widget that will contain tickmarks widget.
     */
    SIGHT_UI_QT_API_QT  tickmarks_slider(QWidget* _parent = nullptr);

    SIGHT_UI_QT_API_QT  ~tickmarks_slider() override = default;

    SIGHT_UI_QT_API_QT  void set_range(int _min, int _max);

    /**
     * @brief Sets the spacing between adjacent tick marks.
     * @param _interval Distance (in slider units) between consecutive tick marks.
     */
    SIGHT_UI_QT_API_QT  void set_tick_interval(int _interval);

    /**
     * @brief Sets the labels for each tick mark and updates the slider range.
     * @param _labels Vector of strings to use as labels for each tick.
     */
    SIGHT_UI_QT_API_QT  void set_tick_labels(const std::vector<std::string>& _labels);

    /**
     * @brief  Returns the labels for each tick mark.
     */
    inline  const std::vector<std::string>& tick_labels() const;

    /**
     * @brief Updates the currently selected tick mark.
     * @param _tick Desired tick index to select.
     */
    SIGHT_UI_QT_API_QT  void set_current_tick(int _tick);

    SIGHT_UI_QT_API_QT  void paintEvent(QPaintEvent*  /*event*/) override;

    SIGHT_UI_QT_API_QT  void mouseMoveEvent(QMouseEvent* _event) override;

    SIGHT_UI_QT_API_QT void mouseReleaseEvent(QMouseEvent* _event) override;

    SIGHT_UI_QT_API_QT  void mousePressEvent(QMouseEvent* _event) override;

Q_SIGNALS:

    SIGHT_UI_QT_API_QT void value_changed(int _value);

private:

    /**
     * @brief Calculates and updates the tick index based on a pixel position.
     * @param _x_pos X position (in pixels) relative to the widget’s left edge.
     */
    SIGHT_UI_QT_API_QT void update_tick_from_pos(const int _x_pos);

    int m_min {0};
    int m_max {10};
    int m_interval {2};
    int m_currentTick {0};
    std::vector<int> m_values;
    bool m_dragging {false};
    std::vector<std::string> m_tickLabels;
    std::unique_ptr<QSlider> m_slider = std::make_unique<QSlider>(this);
};

//------------------------------------------------------------------------------

inline const std::vector<std::string>& tickmarks_slider::tick_labels() const
{
    return m_tickLabels;
}

} //namespace sight::ui::qt::widget
