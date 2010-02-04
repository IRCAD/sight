/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/IBufferDelegate.hpp"

namespace fwData
{

//------------------------------------------------------------------------------

IBufferDelegate::IBufferDelegate()  :
m_buffer(0),
m_bManagesBuff(true)
{}

//------------------------------------------------------------------------------

IBufferDelegate::~IBufferDelegate()
{}

//------------------------------------------------------------------------------

} // namespace fwData
