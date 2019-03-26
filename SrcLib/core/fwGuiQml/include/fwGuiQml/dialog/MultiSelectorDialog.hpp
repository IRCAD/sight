/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwGui/dialog/IMultiSelectorDialog.hpp>

#include <QDialog>
#include <QString>
#include <QWidget>

#include <vector>

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   MultiSelectorDialog allowing the choice of an element among severals (_selections)
 */

class FWGUIQML_CLASS_API MultiSelectorDialog : public ::fwGui::dialog::IMultiSelectorDialog,
                                               public QDialog
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog),
                                            (()),
                                            ::fwGui::factory::New< MultiSelectorDialog > );

    FWGUIQML_API MultiSelectorDialog(::fwGui::GuiBaseObject::Key key);

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

    FWGUIQML_API virtual void setMessage(const std::string& msg) override;

private:

    Selections m_selections;
    /// Dialog box message
    std::string m_message;
    std::string m_title;
};

} // namespace dialog
} // namespace fwGuiQml
