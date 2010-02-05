/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef NODEXMLTRANSLATOR_HPP_
#define NODEXMLTRANSLATOR_HPP_

#include <fwData/Node.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API NodeXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API NodeXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~NodeXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*NODEXMLTRANSLATOR_HPP_*/
