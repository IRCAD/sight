/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_BASE_NUMERIC_HXX__
#define __FWATOMS_BASE_NUMERIC_HXX__


#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

namespace fwAtoms
{

template<typename T>
Numeric::sptr Numeric::New(T value)
{
    Numeric::sptr valueSptr = Numeric::New();
    valueSptr->m_value = ::boost::lexical_cast<std::string>(value);
    return valueSptr;
}

}

#endif
