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

#include "fwData/location/SingleFile.hpp"

#include "fwData/Exception.hpp"
#include "fwData/location/Folder.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::location::SingleFile );

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

SingleFile::SingleFile( ::fwData::Object::Key )
{
}

//------------------------------------------------------------------------------

SingleFile::~SingleFile()
{
}

//------------------------------------------------------------------------------

void SingleFile::shallowCopy(const Object::csptr& _source )
{
    SingleFile::csptr other = SingleFile::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    this->m_path = other->m_path;
}

//------------------------------------------------------------------------------

void SingleFile::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    SingleFile::csptr other = SingleFile::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    this->m_path = other->m_path;
}

//------------------------------------------------------------------------------

SingleFile::sptr SingleFile::New(PathType path)
{
    SingleFile::sptr singlefile = SingleFile::New();
    singlefile->setPath(path);
    return singlefile;
}

//------------------------------------------------------------------------------

void SingleFile::setPath( PathType path)
{
    m_path = path;
}

//------------------------------------------------------------------------------

ILocation::PathType SingleFile::getPath() const
{
    return m_path;
}

//------------------------------------------------------------------------------

}
}
