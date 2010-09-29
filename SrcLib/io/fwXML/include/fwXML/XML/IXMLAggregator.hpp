/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IXMLAGGREGATOR_HPP_
#define IXMLAGGREGATOR_HPP_


#include "fwXML/config.hpp"
#include <libxml/tree.h>


namespace fwXML
{

/**
 * @brief interface for XML integration
 * During visitor::Serialize scanning each object traversed an xml node is created
 * This node only contain intrinsic information of object ( aggregation ignored )
 * @auhtor Vincent Agnus
 */
class FWXML_CLASS_API IXMLAggregator
{
public:

    FWXML_API IXMLAggregator();

    FWXML_API virtual ~IXMLAggregator();

    /**
     * @brief retreive a pointer on xml document
     */
    FWXML_API virtual xmlDocPtr getXMLDoc();

    /** @brief set XMLAggregator in intial state : trivial XML Doc
     *  @FIXME : what to do with old xmlPtr ? ; actually memory leak : no delete() !!!
     */
    FWXML_API void reset();

    /**
     * @brief append xml node in the xml tree as chiild of root. Subclass should specialize this method
     * to define this location in xml, if xml is splitted, path of splitted position
     */
    FWXML_API virtual void append( xmlNodePtr node ) = 0;

    /**
     * @brief append xml node in the xml tree. Subclass should specialize this method
     * to define this location in xml, if xml is splitted, path of splitted position
     */
    FWXML_API virtual void append( xmlNodePtr parent, xmlNodePtr node ) = 0;

protected :

    xmlDocPtr m_xmlDoc;
};

}

#endif /*IXMLAGGREGATOR_HPP_*/
