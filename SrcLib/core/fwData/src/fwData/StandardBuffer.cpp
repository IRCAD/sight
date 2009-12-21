/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include "fwData/StandardBuffer.hpp"

namespace fwData
{

//------------------------------------------------------------------------------

StandardBuffer::StandardBuffer()
{}

//------------------------------------------------------------------------------

StandardBuffer::~StandardBuffer()
{
    this->setBuffer( NULL );
}

//------------------------------------------------------------------------------

void * StandardBuffer::getBuffer()
{
	return m_buffer;
}

//------------------------------------------------------------------------------

void StandardBuffer::setBuffer(void * newBuffer)
{
	char * buff = static_cast< char * >( this->getBuffer() );

	if( buff && newBuffer != buff )
	{
		SLM_ASSERT("StandardBuffer should always manage his buffer", m_bManagesBuff);
		delete[] buff;
	}
	m_buffer = newBuffer;
}

//------------------------------------------------------------------------------

} // namespace fwData
