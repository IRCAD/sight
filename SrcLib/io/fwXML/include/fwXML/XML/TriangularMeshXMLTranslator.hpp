/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef TRIANGULARMESHXMLTRANSLATOR_HPP_
#define TRIANGULARMESHXMLTRANSLATOR_HPP_

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

#include <fwData/TriangularMesh.hpp>


#include "fwXML/XML/XMLTranslator.hpp"


namespace fwXML
{

class FWXML_CLASS_API TriangularMeshXMLTranslator : public XMLTranslator
{
public:

    /// trivial constructor
    FWXML_API TriangularMeshXMLTranslator() ;

    /// trivial destructor
    FWXML_API virtual ~TriangularMeshXMLTranslator() ;

    /// get XML from current object
    FWXML_API xmlNodePtr getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj );


    /// get Object from an XML node
    FWXML_API void updateDataFromXML( ::boost::shared_ptr<fwTools::Object> toUpdate,  xmlNodePtr source);

protected :

    /// manage extra serialization : read image from path extracted from filename attribut in <Buffer> xml node
    FWXML_API void manageLoadingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> );

    /// manage extra serialization : only inform <Buffer filename attribut />
    FWXML_API void manageSavingBuffer( xmlNodePtr boostXMLBuffer /* FIXMEXPATH*/ , ::boost::shared_ptr< ::fwData::TriangularMesh> );
};

}

#endif /*TRIANGULARMESHXMLTRANSLATOR_HPP_*/
