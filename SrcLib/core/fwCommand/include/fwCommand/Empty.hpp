/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMMAND_EMPTY_HPP_
#define _FWCOMMAND_EMPTY_HPP_

#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include "fwCommand/ICommand.hpp"

namespace fwCommand
{

/**
 * @brief Empty command.
 *
 * This command is used to initialize the Manager of command and avoid an empty commands list in the Manager.
 */
struct FWCOMMAND_CLASS_API Empty : public ICommand
{
    fwCoreClassDefinitionsWithFactoryMacro( (Empty)(ICommand), (( )), ::fwTools::Factory::New< Empty > );

    // Overrides.
    FWCOMMAND_API const boost::uint32_t  getSize() const;

    // Overrides.
    FWCOMMAND_API void apply();

    // Overrides.
    FWCOMMAND_API void unapply();

    // Overrides.
    FWCOMMAND_API const std::string getDescription( void ) const;

};


} // namespace command


#endif //#ifndef _FWCOMMAND_EMPTY_HPP_
