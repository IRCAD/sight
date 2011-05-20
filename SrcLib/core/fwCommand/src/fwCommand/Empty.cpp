/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwCommand/Empty.hpp"

namespace fwCommand
{

//-----------------------------------------------------------------------------

REGISTER_BINDING_BYCLASSNAME( ::fwCommand::ICommand, ::fwCommand::Empty, ::fwCommand::Empty );

//-----------------------------------------------------------------------------

const boost::uint32_t  Empty::getSize() const
{
    return sizeof(*this);
}

//-----------------------------------------------------------------------------

void Empty::apply()
{}

//-----------------------------------------------------------------------------

void Empty::unapply()
{}

//-----------------------------------------------------------------------------

const std::string Empty::getDescription() const
{
    return std::string("Empty command");
}

//-----------------------------------------------------------------------------

} // namespace fwCommand
