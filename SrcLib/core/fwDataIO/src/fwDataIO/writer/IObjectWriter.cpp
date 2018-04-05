/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/writer/IObjectWriter.hpp"

#include <fwJobs/IJob.hpp>

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

IObjectWriter::IObjectWriter()
{
}

//------------------------------------------------------------------------------

IObjectWriter::~IObjectWriter()
{
}

//------------------------------------------------------------------------------

void IObjectWriter::setObject( ::fwTools::Object::csptr object )
{
    m_object = object;
}

//------------------------------------------------------------------------------

::fwTools::Object::csptr IObjectWriter::getObject() const
{
    assert( !m_object.expired() );
    return m_object.lock();
}

//------------------------------------------------------------------------------

void IObjectWriter::setLocation( const ::fwData::location::ILocation::sptr _location )
{
    m_location = _location;
}

//------------------------------------------------------------------------------

::fwData::location::ILocation::sptr IObjectWriter::getLocation()
{
    return m_location;
}

//------------------------------------------------------------------------------

std::string IObjectWriter::extension()
{
    return std::string();
}

//------------------------------------------------------------------------------

void IObjectWriter::cancel()
{
    auto job = this->getJob();
    if ( job )
    {
        job->cancel();
    }
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
