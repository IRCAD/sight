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

#pragma once

#include <sight/ui/qt/config.hpp>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSlider>

namespace sight::ui::qt::widget
{

/// This class allows to display a tickmarks widget. The tickmarks widget can be displayed over settings widgets.

class SIGHT_UI_QT_CLASS_API_QT tickmarks_slider : public QWidget
{
Q_OBJECT
Q_PROPERTY(double animated_tick READ animated_tick WRITE set_animated_tick)
Q_PROPERTY(int current_tick READ current_tick WRITE set_current_tick)
Q_PROPERTY(QString current_value READ current_value_property WRITE set_current_value_property)

public:

    /**
     * @brief tickmarks_slider widget constructor.
     * @param _parent : the parent widget that will contain tickmarks widget.
     */
    SIGHT_UI_QT_API_QT explicit tickmarks_slider(QWidget* _parent = nullptr);

    SIGHT_UI_QT_API_QT ~tickmarks_slider() override = default;

    /**
     * @brief Sets the range of the slider.
     * @param _min The minimum tick index.
     * @param _max The maximum tick index.
     */
    SIGHT_UI_QT_API_QT void set_range(int _min, int _max);

    /**
     * @brief Sets the labels for each tick mark and updates the slider range.
     * @param _values Vector of strings to use as values for each tick.
     */
    SIGHT_UI_QT_API_QT void set_tick_values(const std::vector<std::string>& _values);

    /**
     * @brief  Returns the values for each tick mark.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT const std::vector<std::string>& tick_values() const;

    /**
     * @brief Updates the currently selected tick mark.
     * @param _tick Desired tick index to select.
     */
    SIGHT_UI_QT_API_QT void set_current_tick(int _tick);

    /**
     * @brief Returns the currently selected tick mark.
     * @return The index of the currently selected tick.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT int current_tick() const;

    /**
     * @brief Updates the currently selected tick mark.
     * @param _value Desired tick value to select.
     */
    SIGHT_UI_QT_API_QT void set_current_value(const std::string& _value);

    /**
     * @brief Returns the currently selected tick mark.
     * @return The value of the currently selected tick.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT std::string current_value() const;

protected:

    /**
     * @brief Paints the animated tick-mark scale.
     *
     * Workflow:
     * 1. Compute horizontal padding from the widest label.
     * 2. Spread every tick across the remaining width.
     * 3. Shift the whole scale so the current tick (m_animated_tick)
     *    stays roughly centered.
     * 4. For each visible tick, pick length that  based on distance
     *    from the current tick and draw the vertical line.
     * 5. Draw the label of the current tick, centered under the scale.
     *
     */
    SIGHT_UI_QT_API_QT void paintEvent(QPaintEvent*  /*event*/) override;

    /**
     * @brief Handles mouse press events to initiate dragging.
     * @param _event The mouse event containing details about the press.
     */
    SIGHT_UI_QT_API_QT void mouseMoveEvent(QMouseEvent* _event) override;

    /**
     * @brief Handles mouse release events to end dragging.
     * @param _event The mouse event containing details about the release.
     */
    SIGHT_UI_QT_API_QT void mouseReleaseEvent(QMouseEvent* _event) override;

    /**
     * @brief Handles mouse press events to initiate dragging.
     * @param _event The mouse event containing details about the press.
     */
    SIGHT_UI_QT_API_QT void mousePressEvent(QMouseEvent* _event) override;

Q_SIGNALS:

    /**
     * @brief Signal emitted when the value of the slider changes.
     * @param _value The new value of the slider.
     */
    SIGHT_UI_QT_API_QT void value_changed(int _value);

private:

    /**
     * @brief Returns the current animated tick.
     * @return The value of m_animated_tick.
     */
    [[nodiscard]] double animated_tick() const;

    /**
     * @brief Sets the animated tick and triggers an update.
     * @param value The new value for m_animated_tick.
     */
    void set_animated_tick(double _value);

    /// Helper functions to be able to use Q_PROPERTY
    /// @{
    [[nodiscard]] QString current_value_property() const;
    void set_current_value_property(QString _value);
    /// @}

    int m_current_tick {0};
    double m_drag_start_tick {0.0};
    double m_animated_tick = 0.0;
    bool m_dragging {false};
    QPoint m_press_pos;
    std::vector<std::string> m_tick_values;
    std::unique_ptr<QSlider> m_slider;
    QPointer<QPropertyAnimation> m_drag_anim;
};

} //namespace sight::ui::qt::widget
