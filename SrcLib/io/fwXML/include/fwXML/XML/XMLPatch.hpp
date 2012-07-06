/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_XMLPATCH_HPP_
#define _FWXML_XMLPATCH_HPP_

#include <map>

#include <libxml/tree.h>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"


namespace fwXML
{

/**
 * @brief This class contains the methods to patch xml files
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLPatch
{
public:

    FWXML_API static void PatchNoVersionToVersion1( xmlNodePtr node );
    FWXML_API static void PatchVersion1ToVersion2( xmlNodePtr node );

};

}

#endif /*_FWXML_XMLPATCH_HPP_*/
