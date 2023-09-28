/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "io/__/reader/IObjectReader.hpp"

namespace sight::io::reader
{

//------------------------------------------------------------------------------

IObjectReader::IObjectReader()
= default;

//------------------------------------------------------------------------------

IObjectReader::~IObjectReader()
= default;

//------------------------------------------------------------------------------

void IObjectReader::setObject(core::tools::object::sptr _pObject)
{
    m_object = _pObject;
}

//------------------------------------------------------------------------------

core::tools::object::sptr IObjectReader::getObject() const
{
    auto object = m_object.lock();
    SIGHT_ASSERT("Object is null", object);
    return object;
}

//------------------------------------------------------------------------------

void IObjectReader::cancel() const
{
    auto job = this->getJob();
    if(job)
    {
        job->cancel();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::reader
