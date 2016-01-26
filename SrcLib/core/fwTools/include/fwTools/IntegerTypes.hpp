/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_INTEGERTYPES_HPP__
#define __FWTOOLS_INTEGERTYPES_HPP__

#include <boost/mpl/vector.hpp>

namespace fwTools
{

typedef ::boost::mpl::vector<
        signed char, unsigned char,
        signed short, unsigned short,
        signed int, unsigned int
#ifndef DEBUG
        ,signed long, unsigned long
#endif
        >::type IntegerTypes;


} // end namespace fwTools

#endif //__FWTOOLS_INTEGERTYPES_HPP__
