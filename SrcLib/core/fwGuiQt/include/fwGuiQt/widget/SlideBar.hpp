/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_WIDGET_SLIDEBAR_HPP__
#define __FWGUIQT_WIDGET_SLIDEBAR_HPP__


#include "fwGuiQt/config.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QGroupBox>
#include <QWidget>


namespace fwGuiQt
{

namespace widget
{

/**
 * @brief This class allows to display a slide widget. The slide widget can be displayed over all the application
 *        widgets.
 */
class FWGUIQT_CLASS_API SlideBar : public QGroupBox
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
    FWGUIQT_API SlideBar(QWidget* parent, Aligment align = LEFT, int buttonWidth = 200, double opacity = 0.8);

    /// Destructor
    FWGUIQT_API virtual ~SlideBar();

    /// Update the shown/hide positions
    FWGUIQT_API void updatePosition();

    /// Set the widget align
    FWGUIQT_API void setSide(Aligment align);

    /// Return true if the widget is visible
    bool isShown() const
    {
        return m_isShown;
    }

    /// Show/hide the slide widget. The widget is animated
    FWGUIQT_API virtual void setVisible(bool visible);


protected:
    /// Filter 'Resize' and 'Move' events to update the widget position.
    bool eventFilter(QObject* obj, QEvent* event);

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
} // namespace fwGuiQt

#endif // __FWGUIQT_WIDGET_SLIDEBAR_HPP__
