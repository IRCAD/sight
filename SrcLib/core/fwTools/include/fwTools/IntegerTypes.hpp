/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IntegerTypes_hpp
#define IntegerTypes_hpp

#include <boost/mpl/vector.hpp>

namespace fwTools {

typedef boost::mpl::vector< 	signed char , unsigned char,
				signed short, unsigned short,
				signed int  , unsigned int,
				signed long , unsigned long
			  >::type IntegerTypes;


} // end namespace fwTools {

#endif
