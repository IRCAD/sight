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

#include "ui/base/config.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <map>
#include <vector>

namespace sight::ui::base
{

namespace dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   IMultiSelectorDialog allows the choice of some elements among several (_selections)
 */
class UI_BASE_CLASS_API IMultiSelectorDialog : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IMultiSelectorDialog, ui::base::GuiBaseObject);

    typedef std::map<std::string, bool> Selections;
    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    UI_BASE_API IMultiSelectorDialog();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~IMultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    UI_BASE_API virtual void setSelections(Selections _selections) = 0;

    /**
     * @brief Sets the selector title.
     */
    UI_BASE_API virtual void setTitle(std::string _title) = 0;

    /**
     * @brief Show the selector and return the selection.
     */
    UI_BASE_API virtual Selections show() = 0;

    /// Set the message
    UI_BASE_API virtual void setMessage(const std::string& msg) = 0;
};

} //namespace dialog

} //namespace sight::ui::base
