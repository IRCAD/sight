/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/location/MultiFiles.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA( sight::data::location::MultiFiles );

namespace sight::data
{
namespace location
{

//------------------------------------------------------------------------------

MultiFiles::MultiFiles( data::Object::Key )
{
}

//------------------------------------------------------------------------------

MultiFiles::~MultiFiles()
{
}

//------------------------------------------------------------------------------

void MultiFiles::setPaths( VectPathType paths)
{
    m_paths = paths;
}

//------------------------------------------------------------------------------

ILocation::VectPathType MultiFiles::getPaths()
{
    return m_paths;
}

//------------------------------------------------------------------------------

void MultiFiles::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    MultiFiles::csptr other = MultiFiles::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF( data::Exception(
                                  "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                                  + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    SIGHT_FATAL("Not implemented." );
}

//------------------------------------------------------------------------------

}
}
