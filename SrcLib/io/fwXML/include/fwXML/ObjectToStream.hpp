/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_OBJECTTOSTREAM_HPP_
#define _FWXML_OBJECTTOSTREAM_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{

/*
 * @brief serialize an object in a stream
 */


class FWXML_CLASS_API ObjectToStream
{

public:

    /** Convert an object in a xml string representation.
     *
     * @param object : the
     * @param option : select the desired information level ( 0 = short, 1 = fields without splitted fields, 2 = all fields )
     * @return an object string representation
     */
    FWXML_API static const std::string toString( ::fwTools::Object::sptr object, unsigned int option = 0 );

};

}

#endif /* _FWXML_OBJECTTOSTREAM_HPP_ */
