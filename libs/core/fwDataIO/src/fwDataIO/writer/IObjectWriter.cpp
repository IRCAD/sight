/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
