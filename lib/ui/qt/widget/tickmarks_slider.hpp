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
#include <QPoint>
#include <QPropertyAnimation>
#include <QSlider>
namespace sight::ui::qt::widget
{

/// This class allows to display a tickmarks widget. The tickmarks widget can be displayed over settings widgets.

class SIGHT_UI_QT_CLASS_API_QT tickmarks_slider : public QWidget
{
Q_OBJECT
Q_PROPERTY(double animated_tick READ animated_tick WRITE set_animated_tick)

public:

    /**
     * @brief Returns the current animated tick.
     * @return The value of m_animated_tick.
     */
    double animated_tick() const
    {
        return m_animated_tick;
    }

    /**
     * @brief Sets the animated tick and triggers an update.
     * @param t The new value for m_animated_tick.
     */
    void set_animated_tick(double t)
    {
        m_animated_tick = t;
        update();
    }

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

    /**
     * @brief Paints the animated tick-mark scale.
     *
     * Workflow:
     * 1. Compute horizontal padding from the widest label.
     * 2. Spread every tick across the remaining width.
     * 3. Shift the whole scale so the current tick (`m_animated_tick`)
     *    stays roughly centered.
     * 4. For each visible tick, pick length (25/15/10 px) based on distance
     *    from the current tick and draw the vertical line.
     * 5. Draw the label of the current tick, centered under the scale.
     *
     */
    SIGHT_UI_QT_API_QT  void paintEvent(QPaintEvent*  /*event*/) override;

    SIGHT_UI_QT_API_QT  void mouseMoveEvent(QMouseEvent* _event) override;

    SIGHT_UI_QT_API_QT void mouseReleaseEvent(QMouseEvent* _event) override;

    SIGHT_UI_QT_API_QT  void mousePressEvent(QMouseEvent* _event) override;

Q_SIGNALS:

    SIGHT_UI_QT_API_QT void value_changed(int _value);

private:

    int m_min {0};
    int m_max {10};
    int m_interval {1};
    int m_current_tick {0};
    int m_last_x {0};
    int step_px = 8;
    std::vector<int> m_values;
    double m_animated_tick = 0.0;
    QPropertyAnimation* m_drag_anim {nullptr};
    bool m_dragging {false};
    QPoint m_press_pos;
    bool m_released {false};
    std::vector<std::string> m_tick_labels;
    std::unique_ptr<QSlider> m_slider = std::make_unique<QSlider>(this);
};

//------------------------------------------------------------------------------

inline const std::vector<std::string>& tickmarks_slider::tick_labels() const
{
    return m_tick_labels;
}

} //namespace sight::ui::qt::widget
