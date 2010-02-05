/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Video.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Video, ::fwData::Video  );
namespace fwData
{
//------------------------------------------------------------------------------

Video::Video() :
m_ui32XSize(0),
m_ui32YSize(0),
m_ui8BPP(1),
m_pImageBuffer(0),
m_dataAvailable(false),
m_lastModified(0),
m_camera( new ::fwData::Camera)
{
        SLM_TRACE("Video::Video()");
}

//------------------------------------------------------------------------------

Video::~Video() throw()
{
        SLM_TRACE("Video::~Video()");
}

//------------------------------------------------------------------------------
::boost::interprocess::interprocess_mutex &Video::getMutex()
{
    return m_mutex;
}

bool &Video::dataAvailable()
{
    return m_dataAvailable;
}

::boost::uint64_t Video::getLastModified()
{
//  ::boost::interprocess::scoped_lock< ::boost::interprocess::interprocess_mutex > lock(m_mutex);
    return m_lastModified;
}

void Video::Modified()
{
//  ::boost::interprocess::scoped_lock< ::boost::interprocess::interprocess_mutex > lock(m_mutex);
    m_lastModified++;
}

} // namespace fwData
