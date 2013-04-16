/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/location/Folder.hpp"

fwDataRegisterMacro( ::fwData::location::Folder );

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

Folder::Folder( ::fwData::Object::Key key )
{}

//------------------------------------------------------------------------------

Folder::~Folder()
{}

//------------------------------------------------------------------------------

Folder::sptr Folder::FolderFactory(PathType path, bool recursive )
{
    FwCoreNotUsedMacro(recursive);
    Folder::sptr folder = Folder::New();
    folder->setFolder(path);
    return folder;
}

//------------------------------------------------------------------------------

void Folder::shallowCopy(const Object::csptr &_source )
{
    Folder::csptr other = Folder::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    this->m_folder = other->m_folder;
}

//------------------------------------------------------------------------------

void Folder::deepCopy(const Object::csptr &_source )
{
    Folder::csptr other = Folder::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );
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
