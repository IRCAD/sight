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

#include "fwGui/config.hpp"
#include "fwGui/dialog/IMultiSelectorDialog.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   MultiSelectorDialog allows the choice of some element among several (_selections)
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 *
 */
class FWGUI_CLASS_API MultiSelectorDialog : public IMultiSelectorDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog),
                                            new MultiSelectorDialog );

    /// will instanciate the concrete implementation
    FWGUI_API MultiSelectorDialog();

    /// Sets the selector title.
    FWGUI_API void setTitle(std::string title) override;

    /// Set the string list that can be chosen by the selector.
    FWGUI_API virtual void setSelections(Selections _selections) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUI_API Selections show() override;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string& msg) override;

protected:

    ::fwGui::dialog::IMultiSelectorDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui
