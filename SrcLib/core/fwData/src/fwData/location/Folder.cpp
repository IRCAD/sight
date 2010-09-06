/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/location/Folder.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::location::Folder, ::fwData::location::Folder);

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

Folder::Folder()
{}

//------------------------------------------------------------------------------

Folder::~Folder()
{}


Folder::sptr Folder::FolderFactory(::boost::filesystem::path _path, bool recursive )
{
    Folder::sptr folder = Folder::New();
    folder->setFolder(_path);
    return folder;
}


//------------------------------------------------------------------------------

void Folder::setFolder( ::boost::filesystem::path folder)
{
    m_folder = folder;
}

//------------------------------------------------------------------------------

::boost::filesystem::path Folder::getFolder() const
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
