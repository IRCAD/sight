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

#include <fwGui/dialog/IPulseProgressDialog.hpp>

#include <QObject>

#include <string>

namespace fwGuiQml
{
namespace dialog
{
/**
 * @brief   This class allows us to show a pulse progress bar.
 */
class FWGUIQML_CLASS_API PulseProgressDialog : public QObject,
                                               public ::fwGui::dialog::IPulseProgressDialog
{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)

public:

    fwCoreClassDefinitionsWithFactoryMacro( (PulseProgressDialog)(::fwGui::dialog::IPulseProgressDialog),
                                            (()),
                                            ::fwGui::factory::New< PulseProgressDialog > );

    FWGUIQML_API PulseProgressDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~PulseProgressDialog();

    /// override
    FWGUIQML_API void setTitle(const std::string& title) override;

    /// override
    FWGUIQML_API void setMessage(const std::string& message) override;

    FWGUIQML_API void show() override;

Q_SIGNALS:
    /// notify the qml of property change
    void messageChanged();

private:
    /// title of the progress dialog
    QString m_title;
    /// message of the progress dialog
    QString m_message;

};
} // namespace dialog
} // namespace fwGuiQml
