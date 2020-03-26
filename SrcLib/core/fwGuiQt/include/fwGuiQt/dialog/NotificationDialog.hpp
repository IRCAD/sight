/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwGuiQt/config.hpp"

#include <fwCore/macros.hpp>

#include <fwGui/dialog/INotificationDialog.hpp>

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QPointer>
#include <QPropertyAnimation>

#include <string>

namespace fwGuiQt
{

/// Creates a clickable QLabel.
class FWGUIQT_CLASS_API ClickableQLabel : public QLabel
{

Q_OBJECT

public:

    /// Creates a clickable QLabel.
    explicit ClickableQLabel(QWidget* parent = nullptr) :
        QLabel(parent)
    {
    }

    /// Destroyes the clickable QLabel.
    virtual ~ClickableQLabel()
    {

    }

public Q_SLOTS:

    /// Creates a fade out effect then close the widget.
    void fadeout()
    {
        QGraphicsOpacityEffect* const effect = new QGraphicsOpacityEffect();
        this->setGraphicsEffect(effect);
        QPropertyAnimation* a = new QPropertyAnimation(effect, "opacity");
        a->setDuration(500); // it will took 500ms to fade out
        a->setStartValue(0.9);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        QObject::connect(a, &QPropertyAnimation::finished, this, &ClickableQLabel::close);
    }

Q_SIGNALS:

    /// Clicked signal, emited when mousePressEvent occurs.
    void clicked();

private:

    /**
     * @brief Handles event related to the mouse buttons.
     *
     * Send the signal @ref clicked().
     */
    void mousePressEvent(QMouseEvent*) override
    {
        Q_EMIT clicked();
    }
};

namespace dialog
{

/**
 * @brief Defines a notification popup.
 *
 * Example of how to customize the style of the popups (You need to refers to their names):
 *
 * @code{.qss}
 #NotificationDialog_Success
    {
        background-color:#58D68D;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #NotificationDialog_Failure
    {
        background-color:#E74C3C;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }

 #NotificationDialog_Info
    {
        background-color:#5DADE2;
        color:white;
        font-weight: bold;
        font-size: 16px;
    }
   @endcode
 *
 */
class FWGUIQT_CLASS_API NotificationDialog : public ::fwGui::dialog::INotificationDialog
{

public:

    fwCoreClassMacro(NotificationDialog, ::fwGui::dialog::INotificationDialog,
                     ::fwGui::factory::New< NotificationDialog > )

    /// Initializes members.
    FWGUIQT_API NotificationDialog(::fwGui::GuiBaseObject::Key key);

    /// Destroyes the dialog.
    FWGUIQT_API virtual ~NotificationDialog() override final;

    /// Shows the notification relative to the active window.
    FWGUIQT_API void show() override;

    /**
     * @brief Returns whether the popup is displayed or not.
     * @return boolean (true is visible, false otherwise).
     */
    FWGUIQT_API bool isVisible() const override final;

    /// Closes the popup (use a fadeout effect).
    FWGUIQT_API void close() const override final;

private:

    /**
     * @brief Computes the position using the active window boundings.
     * @return the position in pixel as a QPoint.
     */
    QPoint computePosition(const QWidget* _parent) const;

    /// Pointer to the Popup QLabel.
    QPointer< ClickableQLabel > m_msgBox { nullptr };

};

} // namespace dialog
} // namespace fwGuiQt
