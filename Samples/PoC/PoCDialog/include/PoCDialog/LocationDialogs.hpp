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

#include "PoCDialog/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/IInputDialog.hpp>

#include <QObject>

#include <string>

/**
 * @brief   Defines an Input dialog.
 *
 * Example of use:
 * @code
   ::fwGui::dialog::InputDialog inputDlg;
   inputDlg.setTitle("Identification dialog");
   inputDlg.setMessage("Enter Pin Code: ");
   inputDlg.setInput ("<Enter your code here>");
   std::string inputText = inputDlg.getInput();
   @endcode
 */
class POCDIALOG_CLASS_API LocationDialogs : public QObject
{
Q_OBJECT
Q_PROPERTY(bool existing MEMBER m_existing)
Q_PROPERTY(QStringList filter MEMBER m_filter)
Q_PROPERTY(QStringList filterExt MEMBER m_filterExt)
Q_PROPERTY(QString filterSelected MEMBER m_filterSelected)
Q_PROPERTY(QString folder MEMBER m_folder)
Q_PROPERTY(bool isFolder MEMBER m_isFolder)
Q_PROPERTY(bool multiple MEMBER m_multiple)
Q_PROPERTY(QString title MEMBER m_title)
Q_PROPERTY(QString result MEMBER m_result NOTIFY onResultChanged)

public:

    POCDIALOG_API LocationDialogs();

    POCDIALOG_API virtual ~LocationDialogs() override;

    /// Get the input text in the input field
    POCDIALOG_API std::string getInput();

Q_SIGNALS:
    void onResultChanged();

protected:
    /// Dialog box message
    QString m_message;
    /// Text inputed
    QString m_input;

//    ::fwGui::dialog::ILocationDialog::Options m_style;
//    ::fwGui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    std::string m_wildcard;
//    ::fwData::location::ILocation::sptr m_location;

    /// option to save a file enable
    bool m_existing;
    /// the filter list and the current filter selected
    QStringList m_filter;
    /// the filter list extension
    QStringList m_filterExt;
    QString m_filterSelected;
    /// the path of the location at the beginning
    QString m_folder;
    /// option to select folder enable
    bool m_isFolder;
    /// option to select multiple file enable
    bool m_multiple;

    /// title
    QString m_title;

    QString m_result;

protected Q_SLOTS:
    void open();
};
