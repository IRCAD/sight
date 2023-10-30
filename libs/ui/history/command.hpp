/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/history/config.hpp"

#include <memory>
#include <string>

namespace sight::ui::history
{

/**
 * @brief The base class for commands.
 */
class UI_HISTORY_CLASS_API command
{
public:

    using sptr = std::shared_ptr<command>;

    /**
     * @brief Virtual destructor.
     */
    UI_HISTORY_API virtual ~command()
    = default;

    /**
     * @brief Retrieves the memory footprint of the command.
     */
    [[nodiscard]] UI_HISTORY_API virtual std::size_t size() const = 0;

    /**
     * @brief Used to implement the redo operation.
     */
    UI_HISTORY_API virtual bool redo() = 0;

    /**
     * @brief Used to implement the undo operation.
     */
    UI_HISTORY_API virtual bool undo() = 0;

    /**
     * @brief Return an optional description of the command.
     */
    [[nodiscard]] UI_HISTORY_API virtual std::string get_description() const
    {
        return "";
    }
};

} // namespace sight::ui::history
