/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/location/Folder.hpp"

#include "fwData/location/SingleFile.hpp"

#include "fwData/registry/macros.hpp"


fwDataRegisterMacro( ::fwData::location::SingleFile );



namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

SingleFile::SingleFile( ::fwData::Object::Key key )
{}

//------------------------------------------------------------------------------

SingleFile::~SingleFile()
{}

//------------------------------------------------------------------------------

void SingleFile::shallowCopy( SingleFile::csptr _source )
{
    this->fieldShallowCopy( _source );
    this->m_path = _source->m_path;
}

//------------------------------------------------------------------------------

void SingleFile::deepCopy( SingleFile::csptr _source )
{
    this->fieldDeepCopy( _source );
    this->m_path = _source->m_path;
}

//------------------------------------------------------------------------------

SingleFile::sptr SingleFile::SingleFileFactory(PathType path)
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
