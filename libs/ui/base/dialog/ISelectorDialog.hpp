/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <vector>

namespace sight::ui::base::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   ISelectorDialog allows the choice of an element among several (_selections)
 */
class UI_BASE_CLASS_API ISelectorDialog : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(ISelectorDialog, ui::base::GuiBaseObject);

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    UI_BASE_API ISelectorDialog();

    /// Destructor. Do nothing.
    UI_BASE_API ~ISelectorDialog() override;

    /**
     * @brief The string list that can be chosen by the selector.
     */
    UI_BASE_API virtual void setSelections(std::vector<std::string> _selections) = 0;

    /**
     * @brief Sets the selector title.
     */
    UI_BASE_API virtual void setTitle(std::string _title) = 0;

    /**
     * @brief Show the selector and return the selection.
     */
    UI_BASE_API virtual std::string show() = 0;

    /// Set the message
    UI_BASE_API virtual void setMessage(const std::string& msg) = 0;

    /// Add a custom button to this dialog
    UI_BASE_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) = 0;
};

} // namespace sight::ui::base::dialog
