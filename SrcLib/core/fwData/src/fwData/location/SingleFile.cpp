/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/location/SingleFile.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::location::SingleFile, ::fwData::location::SingleFile);

namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

SingleFile::SingleFile()
{}

//------------------------------------------------------------------------------

SingleFile::~SingleFile()
{}

//------------------------------------------------------------------------------

void SingleFile::shallowCopy( SingleFile::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_path = _source->m_path;
}

//------------------------------------------------------------------------------

void SingleFile::deepCopy( SingleFile::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_path = _source->m_path;
}
//------------------------------------------------------------------------------

SingleFile::sptr SingleFile::SingleFileFactory(::boost::filesystem::path _path)
{
    SingleFile::sptr singlefile = SingleFile::New();
    singlefile->setPath(_path);
    return singlefile;
}

//------------------------------------------------------------------------------

void SingleFile::setPath( ::boost::filesystem::path path)
{
    m_path = path;
}

//------------------------------------------------------------------------------

::boost::filesystem::path SingleFile::getPath() const
{
    return m_path;
}

//------------------------------------------------------------------------------

}
}
