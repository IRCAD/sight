/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>
#include <iostream>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <fwTools/TBClassFactory.hpp>
#include <fwTools/TBKClassFactory.hpp>

namespace fwTools
{


template<class BASECLASS, class KEY>
::boost::shared_ptr< BASECLASS > ClassFactoryRegistry::create(const KEY &key)  throw(std::invalid_argument)
 {
    typedef  TBKClassFactory< BASECLASS, KEY>   TFactory;
    std::string strKey = ::fwTools::getString(key);
    std::string base = ::fwTools::getString(typeid(BASECLASS));
    ::fwTools::IClassFactory::sptr factory =  ::fwTools::ClassFactoryRegistry::getFactory(base, strKey );
    if( factory )
    {
        ::boost::shared_ptr< TFactory >  tfactory = ::boost::dynamic_pointer_cast< TFactory >( factory );
        return tfactory->create();
    }
    return ::boost::shared_ptr< BASECLASS >(); // factory not found, return an empty BASECLASS
 }


} //end namespace fwTools


