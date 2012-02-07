/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLSTREAM_H_
#define XMLSTREAM_H_


#include <ostream>
#include <libxml/tree.h>
#include "fwXML/config.hpp"

namespace fwXML
{

/** @brief the purpose of this class is to extend libxml API to support xmltree output(input) into(from) std::stream
 *  @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLStream
{
public:

    FWXML_API XMLStream();

    FWXML_API virtual ~XMLStream();

    /**
     * @brief export xml document into the os stream
     * @warning if os is a fstream instead an ofstream the file is not created and you get an I/O error !!!
     **/
    FWXML_API static void toStream( xmlDocPtr doc, std::ostream &os );
};

}

#endif /*XMLSTREAM_H_*/
