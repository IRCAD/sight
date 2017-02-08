/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMMAND_EMPTY_HPP__
#define __FWCOMMAND_EMPTY_HPP__

#include "fwCommand/ICommand.hpp"

namespace fwCommand
{

/**
 * @brief Empty command.
 *
 * This command is used to initialize the Manager of command and avoid an empty commands list in the Manager.
 */
class FWCOMMAND_CLASS_API Empty : public ICommand
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Empty)(ICommand), (( )), std::make_shared< Empty > );

    // Overrides.
    FWCOMMAND_API const boost::uint32_t  getSize() const;

    // Overrides.
    FWCOMMAND_API void apply();

    // Overrides.
    FWCOMMAND_API void unapply();

    // Overrides.
    FWCOMMAND_API const std::string getDescription( void ) const;

};

} // namespace fwCommand

#endif // __FWCOMMAND_EMPTY_HPP__
