/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWCOMMAND_ICOMMAND_HPP__
#define __FWCOMMAND_ICOMMAND_HPP__

#include "fwCommand/config.hpp"

#include <memory>
#include <string>

namespace fwCommand
{

/**
 * @brief The base class for commands.
 */
class FWCOMMAND_CLASS_API ICommand
{

public:

    typedef std::shared_ptr<ICommand> sptr;

    /**
     * @brief Virtual destructor.
     */
    FWCOMMAND_API virtual ~ICommand()
    {
    }

    /**
     * @brief Retrieves the memory footprint of the command.
     */
    FWCOMMAND_API virtual size_t getSize() const = 0;

    /**
     * @brief Used to implement the redo operation.
     */
    FWCOMMAND_API virtual bool redo() = 0;

    /**
     * @brief Used to implement the undo operation.
     */
    FWCOMMAND_API virtual bool undo() = 0;

    /**
     * @brief Return an optionnal description of the command.
     */
    FWCOMMAND_API virtual const std::string getDescription() const
    {
        return "";
    }

};

} // namespace fwCommand

#endif // __FWCOMMAND_ICOMMAND_HPP__
