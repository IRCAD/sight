/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef SINGLEFILEXMLTRANSLATOR_HPP_
#define SINGLEFILEXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/location/SingleFile.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API SingleFileXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API SingleFileXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~SingleFileXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::fwTools::Object::sptr obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*SINGLEFILEXMLTRANSLATOR_HPP_*/
