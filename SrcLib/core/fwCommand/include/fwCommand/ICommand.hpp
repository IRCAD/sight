/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    FWCOMMAND_API virtual const size_t getSize() const = 0;

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
