/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

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

void Folder::shallowCopy( Folder::csptr _source )
{
    this->fieldShallowCopy( _source );
    this->m_folder = _source->m_folder;
}

//------------------------------------------------------------------------------

void Folder::deepCopy( Folder::csptr _source )
{
    this->fieldDeepCopy( _source );
    this->m_folder = _source->m_folder;
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
