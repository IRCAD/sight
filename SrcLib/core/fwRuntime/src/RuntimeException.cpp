/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwRuntime/RuntimeException.hpp"


namespace fwRuntime
{


RuntimeException::RuntimeException(const RuntimeException& exception) throw()
: ::fwCore::Exception(std::string(exception.what()))
{
    SLM_WARN( this->what() );
}


RuntimeException::RuntimeException(const std::string& message) throw()
: ::fwCore::Exception(message)
{
    SLM_WARN( this->what() );
}


RuntimeException::~RuntimeException() throw()
{}

} // namespace fwRuntime
