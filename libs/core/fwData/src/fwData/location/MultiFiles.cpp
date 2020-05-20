/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/location/MultiFiles.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::location::MultiFiles );

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

MultiFiles::MultiFiles( ::fwData::Object::Key )
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
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );

    OSLM_FATAL("Not implemented." );
}

//------------------------------------------------------------------------------

}
}
