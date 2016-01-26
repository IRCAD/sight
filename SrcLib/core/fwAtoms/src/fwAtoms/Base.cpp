/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/Exception.hpp"
#include "fwAtoms/Base.hpp"


namespace fwAtoms
{

const std::string Base::s_VERSION = "1";

std::string Base::getString() const
{
    FW_RAISE_EXCEPTION( fwAtoms::Exception("No string conversion available.") );
}

}

