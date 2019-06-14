/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <fwGui/dialog/IProgressDialog.hpp>

#include <QObject>
#include <QPointer>
#include <QString>

#include <string>

namespace fwGuiQml
{
namespace dialog
{
/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 */
class FWGUIQML_CLASS_API ProgressDialog : public QObject,
                                          public ::fwGui::dialog::IProgressDialog
{
Q_OBJECT
Q_PROPERTY(bool visible MEMBER m_visible)
Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
Q_PROPERTY(bool hasCallback MEMBER m_hasCallback NOTIFY hasCallbackChanged)

public:
    fwCoreClassDefinitionsWithFactoryMacro((ProgressDialog)(::fwGui::dialog::IProgressDialog),
                                           (()),
                                           ::fwGui::factory::New<ProgressDialog>)

    FWGUIQML_API ProgressDialog(::fwGui::GuiBaseObject::Key key,
                                const std::string& title   = "Progression",
                                const std::string& message = std::string(86, ' '));

    FWGUIQML_API virtual ~ProgressDialog();

    /// the operator to set the progress of the percentage
    FWGUIQML_API void operator()(float percent, std::string msg) override;

    /// override
    FWGUIQML_API void setTitle(const std::string& title) override;

    /// override
    FWGUIQML_API void setMessage(const std::string& message) override;

    FWGUIQML_API void hideCancelButton() override;

Q_SIGNALS:
    void titleChanged();
    void hasCallbackChanged();

protected Q_SLOTS:
    void cancelPressed() override;
protected:
    /// the progress dialog to delete it during the destruction of the class
    QObject* m_dialog;
    /// the window of the progress dialog to delete it during the destruction of the class
    QObject* m_window;
    /// visible state of the progress dialog
    bool m_visible;
    /// title string
    QString m_title;
    /// boolean to enable Cancel button
    bool m_hasCallback;
};
} // namespace dialog
} // namespace fwGuiQml
