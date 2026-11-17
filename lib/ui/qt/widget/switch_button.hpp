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

#include <QAbstractButton>
#include <QColor>
#include <QIcon>
#include <QPaintEvent>
namespace sight::ui::qt::widget
{

/// This class allows to display a toggle button widget. That can be displayed over settings widgets.
class SIGHT_UI_QT_CLASS_API_QT switch_button : public QAbstractButton
{
Q_OBJECT
// Property to control the horizontal offset of the thumb.
Q_PROPERTY(int offset READ offset WRITE set_offset)
// Property to manage the color of the track when the switch is on.
Q_PROPERTY(QColor track_color_on READ track_color_on WRITE set_track_color_on)
// Property to manage the color of the track when the switch is off.
Q_PROPERTY(QColor track_color_off READ track_color_off WRITE set_track_color_off)
// Property used for animating the color of the track.
Q_PROPERTY(QColor current_track_color READ current_track_color WRITE set_current_track_color)

// Property to manage the color of the thumb when the switch is on.
Q_PROPERTY(QColor thumb_color_on READ thumb_color_on WRITE set_thumb_color_on)
// Property to manage the color of the thumb when the switch is off.
Q_PROPERTY(QColor thumb_color_off READ thumb_color_off WRITE set_thumb_color_off)
// Property used for animating the color of the thumb.
Q_PROPERTY(QColor current_thumb_color READ current_thumb_color WRITE set_current_thumb_color)

// Property to manage the color of the icon when the switch is on.
Q_PROPERTY(QColor icon_color_on READ icon_color_on WRITE set_icon_color_on)
// Property to manage the color of the icon when the switch is off.
Q_PROPERTY(QColor icon_color_off READ icon_color_off WRITE set_icon_color_off)
// Property used for animating the color of the icon.
Q_PROPERTY(QColor current_icon_color READ current_icon_color WRITE set_current_icon_color)

public:

    SIGHT_UI_QT_API_QT  switch_button(QWidget* _parent = nullptr);
    SIGHT_UI_QT_API_QT  ~switch_button() override      = default;

    /**
     * @brief Returns the current horizontal offset of the thumb.
     * @return The current horizontal offset in pixels.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT int offset() const;

    /**
     * @brief Sets the horizontal offset of the thumb to the given value.
     * @param _offset The new horizontal offset in pixels.
     */
    SIGHT_UI_QT_API_QT void set_offset(int _offset);

    /**
     * @brief Sets the current color of the thumb.
     *
     * This function is used to animate the thumb's color during state transitions.
     *
     * @param _color The new thumb color.
     */
    SIGHT_UI_QT_API_QT  void set_current_thumb_color(const QColor& _color);

    /**
     * @brief Returns the color of the thumb when the switch is off.
     * @return The thumb's 'off' color .
     */
    SIGHT_UI_QT_API_QT void set_thumb_color_off(const QColor& _color);

    /**
     * @brief Returns the color of the track when the switch is on.
     * @return The track's 'on' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor track_color_on() const;

    /**
     * @brief Sets the color of the track for the 'on' state.
     * @param _color The new track color for the 'on' state.
     */
    SIGHT_UI_QT_API_QT void set_track_color_on(const QColor& _color);

    /**
     * @brief Returns the color of the track when the switch is off.
     * @return The track's 'off' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor track_color_off() const;

    /**
     * @brief Sets the color of the track for the 'off' state.
     * @param _color The new track color for the 'off' state.
     */
    SIGHT_UI_QT_API_QT void set_track_color_off(const QColor& _color);

    /**
     * @brief Returns the current color of the track.
     * @return The current track color.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor current_track_color() const;

    /**
     * @brief Returns the current color of the thumb.
     * @return The current thumb color.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor current_thumb_color() const;

    /**
     * @brief Sets the current color of the track.
     *
     * This function is used to animate the track's color during state transitions.
     *
     * @param _color The new track color.
     */
    SIGHT_UI_QT_API_QT void set_current_track_color(const QColor& _color);

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
    [[nodiscard]] SIGHT_UI_QT_API_QT Qt::CheckState check_state() const;

    /**
     * @brief Returns the color of the thumb when the switch is on.
     * @return The thumb's 'on' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor thumb_color_on() const;

    /**
     * @brief Returns the color of the thumb when the switch is off.
     * @return The thumb's 'off' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor thumb_color_off() const;

    /**
     * @brief Sets the color of the thumb for the 'on' state.
     * @param _color The new thumb color for the 'on' state.
     */
    SIGHT_UI_QT_API_QT void set_thumb_color_on(const QColor& _color);

    /**
     * @brief Sets the icons for the 'on' and 'off' states of the switch.
     * @param _on The icon for the 'on' state.
     * @param _off The icon for the 'off' state.
     */
    SIGHT_UI_QT_API_QT void set_icons(const QIcon& _on, const QIcon& _off);

    /**
     * @brief Returns the color of the icon when the switch is on.
     * @return The icon's 'on' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT  QColor icon_color_on() const;

    /**
     * @brief Sets the color of the icon for the 'on' state.
     * @param _color The new icon color for the 'on' state.
     */
    SIGHT_UI_QT_API_QT  void set_icon_color_on(const QColor& _color);

    /**
     * @brief Returns the current color of the icon.
     * @return The current icon color.
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT QColor current_icon_color() const;

    /**
     * @brief Sets the current color of the icon.
     *
     * This function is used to animate the icon's color during state transitions.
     *
     * @param _color The new icon color.
     */
    SIGHT_UI_QT_API_QT void set_current_icon_color(const QColor& _color);

    /**
     * @brief Returns the color of the icon when the switch is off.
     * @return The icon's 'off' color .
     */
    [[nodiscard]] SIGHT_UI_QT_API_QT   QColor icon_color_off() const;

    /**
     * @brief Sets the color of the icon for the 'off' state.
     * @param _color The new icon color for the 'off' state.
     */
    SIGHT_UI_QT_API_QT void set_icon_color_off(const QColor& _color);

Q_SIGNALS:

    /**
     * @brief This signal is emitted when the check state of the switch changes.
     * @param _state The new check state.
     */
    void state_changed(int _state);

protected:

    SIGHT_UI_QT_API_QT void paintEvent(QPaintEvent* /*e*/) override;
    SIGHT_UI_QT_API_QT void resizeEvent(QResizeEvent* _e) override;
    SIGHT_UI_QT_API_QT void mousePressEvent(QMouseEvent* _e) override;

private:

    // The current horizontal offset of the thumb for animation.
    int m_offset {0};

    // Hash table storing the final offset positions for the 'on' (true) and 'off' (false) states.
    QHash<bool, int> m_end_offset;

    // The color of the track when the switch is in the 'off' state.
    QColor m_track_color_off;

    // The color of the sliding thumb in the 'on' state.
    QColor m_thumb_color_on;

    // The color of the sliding thumb in the 'off' state.
    QColor m_thumb_color_off;

    // The color of the track when the switch is in the 'on' state.
    QColor m_track_color_on;

    // Current color of the track during color animation transitions.
    QColor m_current_track_color;

    // Current color of the thumb during color animation transitions.
    QColor m_current_thumb_color;

    // Current color of the icon during color animation transitions.
    QColor m_current_icon_color;

    // The icons for the 'on' and 'off' states.
    QIcon m_icon_on, m_icon_off;

    // The color of the icon in the 'on' state.
    QColor m_icon_color_on;

    // The color of the icon in the 'off' state.
    QColor m_icon_color_off;
};

} // namespace sight::ui::qt::widget
