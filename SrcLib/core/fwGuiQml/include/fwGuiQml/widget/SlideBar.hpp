/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "fwGuiQml/config.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QGroupBox>
#include <QWidget>

namespace fwGuiQml
{

namespace widget
{

/**
 * @brief This class allows to display a slide widget. The slide widget can be displayed over all the application
 *        widgets.
 */
class FWGUIQML_CLASS_API SlideBar : public QGroupBox
{
Q_OBJECT

public:

    enum Aligment
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    /// SlideBar constructor
    FWGUIQML_API SlideBar(QWidget* parent, Aligment align = LEFT, int buttonWidth = 200, double opacity = 0.8);

    /// Destructor
    FWGUIQML_API virtual ~SlideBar();

    /// Update the shown/hide positions
    FWGUIQML_API void updatePosition();

    /// Set the widget align
    FWGUIQML_API void setSide(Aligment align);

    /// Return true if the widget is visible
    bool isShown() const
    {
        return m_isShown;
    }

    /// Show/hide the slide widget. The widget is animated
    FWGUIQML_API virtual void setVisible(bool visible) override;

protected:
    /// Filter 'Resize' and 'Move' events to update the widget position.
    bool eventFilter(QObject* obj, QEvent* event) override;

private:

    /// Force to hide the widget calling the super class setVisible(false) method
    void forceHide();

    /// Force to show of the widget calling the super class setVisible(true) method
    void forceShow();

    /// Show/hide the slide widget
    void slide(bool visible);

    /// Initialize the widget
    void init();

    QRect m_shownPosition; ///< widget position when it is visible
    QRect m_hiddenPosition; ///< widget position when it is not visible
    int m_buttonSize; ///< widget size (width if m_align is LEFT or RIGHT, height if m_align is TOP or BOTTOM)
    double m_opacity; /// widget opacity [0 - 1]
    bool m_isShown; ///< flag if widget is visible
    Aligment m_align; ///< defines the widget alignement (left, right, top or bottom)

};

} // namespace widget
} // namespace fwGuiQml
