/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMS_OBJECT_HXX__
#define  __FWATOMS_OBJECT_HXX__

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/factory/new.hpp"

#ifndef __FWATOMS_OBJECT_HPP__
#error fwAtoms/Object.hpp not included
#endif

namespace fwAtoms
{

template < typename OBJECT_TYPE >
SPTR(OBJECT_TYPE) Object::getAttribute(const std::string& key) const
{
    ::fwAtoms::Base::sptr base = this->getAttribute(key);
    return ::boost::dynamic_pointer_cast< OBJECT_TYPE >(base);
}

}

#endif /*  __FWATOMS_OBJECT_HXX__ */

