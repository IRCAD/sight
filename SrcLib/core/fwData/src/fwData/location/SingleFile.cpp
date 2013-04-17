/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/location/Folder.hpp"
#include "fwData/Exception.hpp"

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

void SingleFile::shallowCopy(const Object::csptr &_source )
{
    SingleFile::csptr other = SingleFile::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    this->m_path = other->m_path;
}

//------------------------------------------------------------------------------

void SingleFile::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    SingleFile::csptr other = SingleFile::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    this->m_path = other->m_path;
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
