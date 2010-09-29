/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef LABELEDOBJECTXMLTRANSLATOR_HPP_
#define LABELEDOBJECTXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwTools/Object.hpp>
#include "fwXML/XML/XMLTranslator.hpp"




namespace fwXML
{

class FWXML_CLASS_API FieldXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API FieldXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~FieldXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

};

}

#endif /*LABELEDOBJECTXMLTRANSLATOR_HPP_*/
