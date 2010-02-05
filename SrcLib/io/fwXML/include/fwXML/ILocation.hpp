/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _FWXML_LOCATION_HPP_
#define _FWXML_LOCATION_HPP_

namespace fwXML
{

#include "fwXML/config.hpp"



/**
 * @brief Abstract class to define a location ( fileSystem, directory(IES) etc.... )
 */
class FWXML_CLASS_API ILocation
{
protected:

    // cannot be instanciated but derivated class OK

    FWXML_API ILocation();

    FWXML_API virtual ~ILocation();
};

}

#endif /* LOCATION_HPP_ */
