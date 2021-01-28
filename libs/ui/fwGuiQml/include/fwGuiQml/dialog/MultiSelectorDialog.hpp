/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <QObject>
#include <QString>
#include <QVariant>

#include <ui/base/dialog/IMultiSelectorDialog.hpp>

#include <vector>

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   MultiSelectorDialog allowing the choice of an element among severals (_selections)
 */

class FWGUIQML_CLASS_API MultiSelectorDialog :  public QObject,
                                                public ui::base::dialog::IMultiSelectorDialog

{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)

public:

    fwCoreClassMacro(MultiSelectorDialog, ui::base::dialog::IMultiSelectorDialog,
                     ui::base::factory::New< MultiSelectorDialog >)

    FWGUIQML_API MultiSelectorDialog(ui::base::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~MultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIQML_API virtual void setSelections(Selections _selections) override;

    /**
     * @brief Sets the selector title.
     */
    FWGUIQML_API virtual void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUIQML_API virtual Selections show() override;

    /**
     * @brief Sets the selector message.
     */
    FWGUIQML_API virtual void setMessage(const std::string& msg) override;

Q_SIGNALS:
    /// notify the qml of property change
    void messageChanged();

protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(QVariant checkList, bool state);

private:

    Selections m_selections;
    /// Dialog box message
    QString m_message;
    /// Dialog box title
    QString m_title;
};

} // namespace dialog
} // namespace fwGuiQml
