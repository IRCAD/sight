/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/location/MultiFiles.hpp"

fwDataRegisterMacro( ::fwData::location::MultiFiles );




namespace fwData
{
namespace location
{

//------------------------------------------------------------------------------

MultiFiles::MultiFiles( ::fwData::Object::Key key )
{}

//------------------------------------------------------------------------------

MultiFiles::~MultiFiles()
{}

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

}
}
