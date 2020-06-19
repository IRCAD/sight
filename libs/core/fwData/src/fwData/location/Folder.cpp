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

#include "fwData/location/Folder.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

fwDataRegisterMacro( ::fwData::location::Folder );

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

Folder::Folder( ::fwData::Object::Key key )
{
    FwCoreNotUsedMacro(key);
}

//------------------------------------------------------------------------------

Folder::~Folder()
{
}

//------------------------------------------------------------------------------

Folder::sptr Folder::New(PathType path, bool recursive )
{
    FwCoreNotUsedMacro(recursive);
    Folder::sptr folder = Folder::New();
    folder->setFolder(path);
    return folder;
}

//------------------------------------------------------------------------------

void Folder::shallowCopy(const Object::csptr& _source )
{
    Folder::csptr other = Folder::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    this->m_folder = other->m_folder;
}

//------------------------------------------------------------------------------

void Folder::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Folder::csptr other = Folder::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    this->m_folder = other->m_folder;
}

//------------------------------------------------------------------------------

void Folder::setFolder( PathType folder)
{
    m_folder = folder;
}

//------------------------------------------------------------------------------

ILocation::PathType Folder::getFolder() const
{
    return m_folder;
}

//------------------------------------------------------------------------------

void Folder::setRecursive( bool recursive)
{
    m_isRecursive = recursive;
}

//------------------------------------------------------------------------------

bool Folder::getRecursive()
{
    return m_isRecursive;
}

//------------------------------------------------------------------------------

}
}
