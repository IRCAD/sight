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

protected Q_SLOTS:
    void cancelPressed() override;
protected:
    /// the progress dialog to delete it during the destruction of the class
    QObject* m_dialog;
    /// the message object inside the dialog
    QObject* m_messageObject;
    /// the value object of the ProgressBar inside the dialog
    QObject* m_valueObject;
    /// visible state of the progress dialog
    bool m_visible;
};
} // namespace dialog
} // namespace fwGuiQml
