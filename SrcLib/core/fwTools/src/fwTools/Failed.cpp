/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/Failed.hpp"

namespace fwTools
{

Failed::Failed(const std::string & message) throw()
: m_message(message)
{}


Failed::~Failed() throw()
{}


const char * Failed::what() const throw()
{
        return m_message.c_str();
}

        
} // namespace fwTools
