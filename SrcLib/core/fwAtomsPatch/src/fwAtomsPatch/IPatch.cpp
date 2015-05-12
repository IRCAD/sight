/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomsPatch/IPatch.hpp"

namespace fwAtomsPatch
{

IPatch::IPatch()
{
}

// ----------------------------------------------------------------------------

IPatch::~IPatch()
{
}

// ----------------------------------------------------------------------------

IPatch::IPatch( const IPatch &cpy )
{
    m_originClassname = cpy.getOriginClassname();
    m_originVersion   = cpy.getOriginVersion();
}

// ----------------------------------------------------------------------------

const std::string& IPatch::getOriginClassname() const
{
    return m_originClassname;
}

// ----------------------------------------------------------------------------

const std::string& IPatch::getOriginVersion() const
{
    return m_originVersion;
}

} //fwAtomsPatch
