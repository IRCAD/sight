/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include <core/log/logger.hpp>

#include <ui/__/object.hpp>

#include <string>
#include <vector>

namespace sight::ui::dialog
{

/**
 * @brief logger_base is used to display a dialog message and a set of logs
 */
class SIGHT_UI_CLASS_API logger_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(logger_base, ui::object);

    using factory_registry_key_t = std::string;

    /// This *unique* key should  be used *for all* factory for specific selector(qt,wx,...)
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    /**
     * @brief Constructor
     */
    SIGHT_UI_API logger_base();

    /// Destructor
    SIGHT_UI_API ~logger_base() override;

    /**
     * @brief Set the dialog title.
     * @param[in] _title Dialog title
     */
    SIGHT_UI_API virtual void set_title(const std::string& _title) = 0;

    /**
     * @brief Set the dialog message.
     * @param[in] _message Dialog message
     */
    SIGHT_UI_API virtual void set_message(const std::string& _message) = 0;

    /**
     * @brief Set the dialog logger.
     * @param[in] _logger Dialog logger
     */
    SIGHT_UI_API virtual void set_logger(const core::log::logger::sptr& _logger) = 0;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    SIGHT_UI_API virtual bool show() = 0;
};

} // namespace sight::ui::dialog
