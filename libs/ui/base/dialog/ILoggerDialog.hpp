/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"

#include <core/log/Logger.hpp>

#include <ui/base/GuiBaseObject.hpp>

#include <string>
#include <vector>

namespace sight::ui::base
{
namespace dialog
{

/**
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 */
class UI_BASE_CLASS_API ILoggerDialog : public ui::base::GuiBaseObject
{

public:

    fwCoreClassMacro(ILoggerDialog, ui::base::GuiBaseObject)

    typedef std::string FactoryRegistryKeyType;

    /// This *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor
     */
    UI_BASE_API ILoggerDialog();

    /// Destructor
    UI_BASE_API virtual ~ILoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    UI_BASE_API virtual void setTitle(const std::string& title) = 0;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    UI_BASE_API virtual void setMessage(const std::string& message) = 0;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    UI_BASE_API virtual void setLogger(const core::log::Logger::sptr& logger) = 0;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    UI_BASE_API virtual bool show() = 0;
};

} //namespace dialog
} //namespace sight::ui::base
