/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include <QApplication>
#include <QWidget>

namespace sight::ui::qt::widget
{

/// This class allows to display a slide widget. The slide widget can be displayed over all others widgets.
class SIGHT_UI_QT_CLASS_API_QT slide_bar : public QWidget
{
Q_OBJECT

public:

    /// Defines the horizontal alignment of the widget.
    enum h_alignment
    {
        right,
        left
    };

    /// Defines the vertical alignment of the widget.
    enum v_alignment
    {
        top,
        bottom
    };

    /// Defines animation direction.
    enum animatable_alignment
    {
        right_animation,
        left_animation,
        top_animation,
        bottom_animation
    };

    /**
     * @brief Creates the slide bar widget.
     * @param _parent parent where attached the slide bar.
     * @param _h_align horizontal alignment.
     * @param _v_align vertical alignment.
     * @param _width width of the widget.
     * @param _percent_width defines if the width must be read a percent.
     * @param _height height of the widget.
     * @param _percent_height defines if the height must be read a percent.
     * @param _h_offset h offset of the widget.
     * @param _percent_h_offset defines if the h offset must be read a percent.
     * @param _v_offset v offset of the widget.
     * @param _percent_v_offset defines if the v offset must be read a percent.
     * @param _opacity opacity of the widget.
     * @param _animatable defines if the animation is enable.
     * @param _animatable_alignment defines the animation direction.
     */
    SIGHT_UI_QT_API_QT slide_bar(
        QWidget* _parent,
        h_alignment _h_align,
        v_alignment _v_align,
        int _width,
        bool _percent_width,
        int _height,
        bool _percent_height,
        int _h_offset,
        bool _percent_h_offset,
        int _v_offset,
        bool _percent_v_offset,
        double _opacity,
        bool _animatable,
        animatable_alignment _animatable_alignment
    );

    /// Destroyes the widget.
    SIGHT_UI_QT_API_QT ~slide_bar() override;

    /// Updates the widget position relative of the parent.
    SIGHT_UI_QT_API_QT void update_position();

    /// Sets the widget visibility.
    SIGHT_UI_QT_API_QT void setVisible(bool _visible) override;

    /// Gets if the widget is shown.
    [[nodiscard]] inline bool is_shown() const;

private:

    bool eventFilter(QObject* _obj, QEvent* _event) override;

    /// Initializes the widget by settings flags and attributes.
    void init();

    /// Hides the widget.
    void force_hide();

    /// Shows the widget.
    void force_show();

    /// Slides the widget with the animation if it's enabled, else, show or hide the widget.
    void slide(bool _visible);

    /// Defines the horizontal alignment.
    h_alignment m_h_alignment {left};

    /// Defines the vertical alignment.
    v_alignment m_v_alignment {top};

    /// Defines the width of the widget.
    int m_width {100};

    /// Defines if the width must be read a percent.
    bool m_percent_width {true};

    /// Defines the height of the widget.
    int m_height {100};

    /// Defines if the height must be read a percent.
    bool m_percent_height {true};

    /// Defines the horizontal offset of the widget.
    int m_h_offset {0};

    /// Defines if the horizontal offset must be read a percent.
    bool m_percent_h_offset {false};

    /// Defines the vertical offset of the widget.
    int m_v_offset {0};

    /// Defines if the vertical offset must be read a percent.
    bool m_percent_v_offset {false};

    /// Defines the widget opacity.
    double m_opacity {1.};

    /// Defines if the widget animation is enabled.
    bool m_animatable {false};

    /// Defines the animation direction.
    animatable_alignment m_animatable_alignment {top_animation};

    /// Defines the position of the widget when it's shown.
    QRect m_shown_position;

    /// Defines the position of the widget when it's hidden.
    QRect m_hidden_position;

    /// Defines the widget visibility.
    bool m_is_shown {false};
};

//------------------------------------------------------------------------------

bool slide_bar::is_shown() const
{
    return m_is_shown;
}

} // namespace sight::ui::qt::widget
