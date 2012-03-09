/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/Video.hpp"

fwDataRegisterMacro( ::fwData::Video );
namespace fwData
{
//------------------------------------------------------------------------------

Video::Video() :
        m_ui32XSize(0),
        m_ui32YSize(0),
        m_ui8BPP(1),
        m_pImageBuffer(0),
        m_dataAvailable(false),
        m_lastModified(0)
{
    m_camera = ::fwData::Camera::New();
}

//------------------------------------------------------------------------------

Video::~Video() throw()
{}

//------------------------------------------------------------------------------

::boost::interprocess::interprocess_mutex &Video::getMutex()
{
    return m_mutex;
}

//------------------------------------------------------------------------------

bool &Video::dataAvailable()
{
    return m_dataAvailable;
}

//------------------------------------------------------------------------------

::boost::uint64_t Video::getLastModified()
{
    return m_lastModified;
}

//------------------------------------------------------------------------------

void Video::Modified()
{
    m_lastModified++;
}

//------------------------------------------------------------------------------


} // namespace fwData
