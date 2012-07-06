/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include "fwXML/policy/IndexPathPolicy.hpp"

namespace fwXML
{

::boost::filesystem::path IndexPathPolicy::getPath(::fwData::Object::sptr obj)
{
    if ( m_index[obj] != 0 )
    {
        // index exist m_index[obj] correctly set
    }
    else
    {
        //if not, added to m_index, and increment m_lastIndexAvailable[obj->className()]
        m_index[obj] = m_lastIndexAvailable[ obj->getLeafClassname() ]++ ;
    }

    std::stringstream ss ;
    ss << obj->getLeafClassname() << "_" << m_index[obj] << ".xml"  ;

    return ::boost::filesystem::path(  ss.str() ) ;
}

}//namespace fwXML
