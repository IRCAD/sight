/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/location/MultiFiles.hpp"

namespace fwData{
namespace location {

MultiFiles::MultiFiles()
{
        // TODO Auto-generated constructor stub

}

MultiFiles::~MultiFiles()
{
        // TODO Auto-generated destructor stub
}

void MultiFiles::setPaths( std::vector< ::boost::filesystem::path> paths)
{
        m_paths = paths;
}


std::vector< ::boost::filesystem::path> MultiFiles::getPaths()
{
        return m_paths;
}





}
}
