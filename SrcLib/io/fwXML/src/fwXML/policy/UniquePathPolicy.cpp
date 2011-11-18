/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include "fwXML/policy/UniquePathPolicy.hpp"

namespace fwXML
{

UniquePathPolicy::UniquePathPolicy(const std::string &filename)
: m_uniquePath ( filename )
{}

UniquePathPolicy::~UniquePathPolicy()
{}

void UniquePathPolicy::setUniquePath(const std::string &filename)
{
    m_uniquePath = filename;
}

::boost::filesystem::path UniquePathPolicy::getPath(::fwTools::Object::sptr obj )
{
     return m_uniquePath;
}

}//namespace fwXML
