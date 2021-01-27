/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "gui/config.hpp"
#include "gui/container/fwContainer.hpp"
#include "gui/GuiBaseObject.hpp"

#include <map>
#include <vector>

namespace sight::gui
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   IMultiSelectorDialog allows the choice of some elements among several (_selections)
 */
class GUI_CLASS_API IMultiSelectorDialog : public gui::GuiBaseObject
{

public:

    fwCoreClassMacro(IMultiSelectorDialog, gui::GuiBaseObject)

    typedef std::map< std::string, bool > Selections;
    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    GUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    GUI_API IMultiSelectorDialog();

    /// Destructor. Do nothing.
    GUI_API virtual ~IMultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    GUI_API virtual void setSelections(Selections _selections) = 0;

    /**
     * @brief Sets the selector title.
     */
    GUI_API virtual void setTitle(std::string _title) = 0;

    /**
     * @brief Show the selector and return the selection.
     */
    GUI_API virtual Selections show() = 0;

    /// Set the message
    GUI_API virtual void setMessage(const std::string& msg) = 0;
};

} //namespace dialog
} //namespace sight::gui
