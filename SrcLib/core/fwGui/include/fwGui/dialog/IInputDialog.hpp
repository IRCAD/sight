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
#include "fwGui/GuiBaseObject.hpp"

#include <string>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic input dialog box for IHM.
 */
class FWGUI_CLASS_API IInputDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(IInputDialog, ::fwGui::GuiBaseObject);

    /// Constructor. Do nothing.
    FWGUI_API IInputDialog();
    /// Destructor. Do nothing.
    FWGUI_API virtual ~IInputDialog();

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the title of the input dialog
    FWGUI_API virtual void setTitle(const std::string& title) = 0;

    /// Set the input text in the input field
    FWGUI_API virtual void setInput(const std::string& text) = 0;

    /// Get the input text in the input field
    FWGUI_API virtual std::string getInput() = 0;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string& msg) = 0;

};

} //namespace dialog
} // namespace fwGui
