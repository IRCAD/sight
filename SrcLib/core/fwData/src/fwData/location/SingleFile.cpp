/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/location/SingleFile.hpp"

namespace fwData {
namespace location {

SingleFile::SingleFile()
{
	// TODO Auto-generated constructor stub

}

SingleFile::~SingleFile()
{
	// TODO Auto-generated destructor stub
}

void SingleFile::setPath( ::boost::filesystem::path path)
{
	m_path = path;
}


::boost::filesystem::path SingleFile::getPath()
{
	return m_path;
}



}
}
