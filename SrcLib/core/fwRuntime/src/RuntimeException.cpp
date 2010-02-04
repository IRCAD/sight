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
: m_message(exception.m_message)
{
	SLM_WARN( m_message );
}


RuntimeException::RuntimeException(const std::string& message) throw()
: m_message(message)
{
	SLM_WARN( m_message );
}


RuntimeException::~RuntimeException() throw()
{}


const char* RuntimeException::what() const throw()
{
	return m_message.c_str();
}


} // namespace fwRuntime
