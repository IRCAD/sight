/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "ui/qt/config.hpp"

#include <QApplication>
#include <QWidget>

namespace sight::ui::qt
{

namespace widget
{

/// This class allows to display a slide widget. The slide widget can be displayed over all others widgets.
class UI_QT_CLASS_API SlideBar : public QWidget
{
Q_OBJECT

public:

    /// Defines the horizontal alignment of the widget.
    enum HAlignment
    {
        RIGHT,
        LEFT
    };

    /// Defines the vertical alignment of the widget.
    enum VAlignment
    {
        TOP,
        BOTTOM
    };

    /// Defines animation direction.
    enum AnimatableAlignment
    {
        RIGHT_ANIMATION,
        LEFT_ANIMATION,
        TOP_ANIMATION,
        BOTTOM_ANIMATION
    };

    /**
     * @brief Creates the slide bar widget.
     * @param _parent parent where attached the slide bar.
     * @param _hAlign horizontal alignment.
     * @param _vAlign vertical alignment.
     * @param _width width of the widget.
     * @param _percentWidth defines if the width must be read a percent.
     * @param _height height of the widget.
     * @param _percentHeight defines if the height must be read a percent.
     * @param _hOffset h offset of the widget.
     * @param _percentHOffset defines if the h offset must be read a percent.
     * @param _vOffset v offset of the widget.
     * @param _percentVOffset defines if the v offset must be read a percent.
     * @param _opacity opacity of the widget.
     * @param _animatable defines if the animation is enable.
     * @param _animatableAlignment defines the animation direction.
     */
    UI_QT_API SlideBar(QWidget* _parent,
                       HAlignment _hAlign,
                       VAlignment _vAlign,
                       int _width,
                       bool _percentWidth,
                       int _height,
                       bool _percentHeight,
                       int _hOffset,
                       bool _percentHOffset,
                       int _vOffset,
                       bool _percentVOffset,
                       double _opacity,
                       bool _animatable,
                       AnimatableAlignment _animatableAlignment);

    /// Destroyes the widget.
    UI_QT_API virtual ~SlideBar();

    /// Updates the widget position relative of the parent.
    UI_QT_API void updatePosition();

    /// Sets the widget visibility.
    UI_QT_API virtual void setVisible(bool _visible) override;

    /// Gets if the widget is shown.
    inline bool isShown() const;

private:

    bool eventFilter(QObject* _obj, QEvent* _event) override;

    /// Initializes the widget by settings flags and attributes.
    void init();

    /// Hides the widget.
    void forceHide();

    /// Shows the widget.
    void forceShow();

    /// Slides the widget with the animation if it's enabled, else, show or hide the widget.
    void slide(bool visible);

    /// Defines the horizontal alignment.
    HAlignment m_hAlignment { LEFT };

    /// Defines the vertical alignment.
    VAlignment m_vAlignment { TOP };

    /// Defines the width of the widget.
    int m_width { 100 };

    /// Defines if the width must be read a percent.
    bool m_percentWidth { true };

    /// Defines the height of the widget.
    int m_height { 100 };

    /// Defines if the height must be read a percent.
    bool m_percentHeight { true };

    /// Defines the horizontal offset of the widget.
    int m_hOffset { 0 };

    /// Defines if the horizontal offset must be read a percent.
    bool m_percentHOffset { false };

    /// Defines the vertical offset of the widget.
    int m_vOffset { 0 };

    /// Defines if the vertical offset must be read a percent.
    bool m_percentVOffset { false };

    /// Defines the widget opacity.
    double m_opacity { 1. };

    /// Defines if the widget animation is enabled.
    bool m_animatable { false };

    /// Defines the animation direction.
    AnimatableAlignment m_animatableAlignment { TOP_ANIMATION };

    /// Defines the position of the widget when it's shown.
    QRect m_shownPosition;

    /// Defines the position of the widget when it's hidden.
    QRect m_hiddenPosition;

    /// Defines the widget visibility.
    bool m_isShown { false };

};

//------------------------------------------------------------------------------

bool SlideBar::isShown() const
{
    return m_isShown;
}

} // namespace widget.
} // namespace sight::ui::qt.
