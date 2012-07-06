/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef XMLTRANSLATOR_H_
#define XMLTRANSLATOR_H_

#include <libxml/tree.h>

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{

/*
 * @brief The purpose of this class is provide a translation scheme within XML and fwTools::Object.
 * Each specialized subClass have the responsibility of providing the implementation
 * @note : all informations are created on demand. No information are cached
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API XMLTranslator : public ::fwCore::BaseObject
{
public:

    fwCoreNonInstanciableClassDefinitionsMacro( (XMLTranslator)(::fwCore::BaseObject) )

    FWXML_API XMLTranslator();

    FWXML_API virtual ~XMLTranslator();

    /// get XML from current object
    FWXML_API virtual xmlNodePtr getXMLFrom( ::fwData::Object::sptr obj ) = 0;

    /// get Object from an XML node
    FWXML_API virtual void updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source) = 0;

};

}

#endif /*XMLTRANSLATOR_H_*/
