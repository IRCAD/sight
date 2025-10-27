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

#include <QAbstractButton>
#include <QColor>
#include <QPaintEvent>
namespace sight::ui::qt::widget
{

/// This class allows to display a toggle button widget. That can be displayed over settings widgets.
class SIGHT_UI_QT_CLASS_API_QT switch_button : public QAbstractButton
{
Q_OBJECT
// Property to control the horizontal offset of the thumb.
Q_PROPERTY(int offset READ offset WRITE set_offset)
// Property to manage the color of the thumb (the sliding button).
Q_PROPERTY(QColor thumb_color READ thumb_color WRITE set_thumb_color)
// Property to manage the color of the track when the switch is on.
Q_PROPERTY(QColor track_color_on READ track_color_on WRITE set_track_color_on)
// Property used for animating the color of the track.
Q_PROPERTY(QColor current_track_color READ current_track_color WRITE set_current_track_color)

public:

    SIGHT_UI_QT_API_QT  switch_button(QWidget* _parent = nullptr);
    SIGHT_UI_QT_API_QT  ~switch_button() override      = default;

    /**
     * @brief Returns the current horizontal offset of the thumb.
     * @return The current horizontal offset in pixels.
     */
    SIGHT_UI_QT_API_QT int offset() const;

    /**
     * @brief Sets the horizontal offset of the thumb to the given value.
     * @param offset The new horizontal offset in pixels.
     */
    SIGHT_UI_QT_API_QT void set_offset(int offset);

    /**
     * @brief Returns the current color of the thumb.
     * @return The color of the thumb .
     */
    SIGHT_UI_QT_API_QT QColor thumb_color() const;

    /**
     * @brief Sets the color of thumb to the given color.
     * @param color  The new color of the thumb (can be changed in QSS).
     */
    SIGHT_UI_QT_API_QT void set_thumb_color(const QColor& color);

    /**
     * @brief Returns the color of the track when the switch is on.
     * @return The track's 'on' color .
     */
    SIGHT_UI_QT_API_QT QColor track_color_on() const;

    /**
     * @brief Sets the color of the track for the 'on' state.
     * @param color The new track color for the 'on' state.
     */
    SIGHT_UI_QT_API_QT void set_track_color_on(const QColor& color);

    /**
     * @brief Returns the current color of the track.
     * @return The current track color.
     */
    SIGHT_UI_QT_API_QT QColor current_track_color() const;

    /**
     * @brief Sets the current color of the track.
     *
     * This function is used to animate the track's color during state transitions.
     *
     * @param color The new track color.
     */
    SIGHT_UI_QT_API_QT void set_current_track_color(const QColor& color);

    /**
     * @brief Sets the check state of the switch.
     *
     * This function is used to change the state of the button
     * to either checked, unchecked.
     *
     * @param _state The new check state.
     */
    SIGHT_UI_QT_API_QT void set_check_state(const Qt::CheckState& _state);

    /**
     * @brief Returns the current check state of the switch.
     * @return The current check state as a Qt::CheckState enum.
     */
    SIGHT_UI_QT_API_QT Qt::CheckState check_state() const;

Q_SIGNALS:

    /**
     * @brief This signal is emitted when the check state of the switch changes.
     * @param _state The new check state.
     */
    void state_changed(int _state);

protected:

    SIGHT_UI_QT_API_QT void paintEvent(QPaintEvent* /*e*/) override;
    SIGHT_UI_QT_API_QT void resizeEvent(QResizeEvent* e) override;
    SIGHT_UI_QT_API_QT void mousePressEvent(QMouseEvent* e) override;

private:

    // The current horizontal offset of the thumb for animation.
    int m_offset {0};

    // Hash table storing the final offset positions for the 'on' (true) and 'off' (false) states.
    QHash<bool, int> m_end_offset;

    // The color of the track when the switch is in the 'off' state.
    QColor m_track_color_off;

    // The color of the sliding thumb.
    QColor m_thumb_color;

    // The color of the track when the switch is in the 'on' state.
    QColor m_track_color_on;

    // Current color of the track during color animation transitions.
    QColor m_current_track_color;
};

} // namespace sight::ui::qt::widget
