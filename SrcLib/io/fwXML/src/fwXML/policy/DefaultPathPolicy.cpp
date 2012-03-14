/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <fwTools/UUID.hpp>




#include "fwXML/policy/DefaultPathPolicy.hpp"

namespace fwXML
{

::boost::filesystem::path DefaultPathPolicy::getPath(::fwData::Object::sptr obj )
{
    std::stringstream ss ;
    ss << obj->getLeafClassname() << "_" << ::fwTools::UUID::get(obj) << ".xml";

    return  ::boost::filesystem::path ( ss.str() ) ;
}

}//namespace fwXML
