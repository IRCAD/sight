/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPacsIO/exceptions/NegociateAssociationFailure.hpp"

namespace fwPacsIO
{
namespace exceptions
{

NegociateAssociationFailure::NegociateAssociationFailure(const std::string& message) :
    ::fwPacsIO::exceptions::Base(message)
{
}

// ----------------------------------------------------------------------------

NegociateAssociationFailure::~NegociateAssociationFailure() throw()
{
}

} //exceptions
} //fwPacsIO
