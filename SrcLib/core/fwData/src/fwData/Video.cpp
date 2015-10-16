/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Video.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Video );

namespace fwData
{
//------------------------------------------------------------------------------

Video::Video(::fwData::Object::Key key) :
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
{
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

void Video::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
{
    Video::csptr other = Video::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    SLM_FATAL("Not implemented." );
}


} // namespace fwData
