/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_VISITOR_SERIALIZEXML_HPP
#define _FWXML_VISITOR_SERIALIZEXML_HPP

#include <map>
#include <boost/shared_ptr.hpp>

#include <fwXML/visitor/IVisitor.hpp>
#include <libxml/tree.h>

#include "fwXML/config.hpp"

namespace visitor
{

/**
 * @brief visit all objects then call complete an XML tree
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API SerializeXML : public  ::fwData::visitor::IVisitor
{
public:
    FWXML_API SerializeXML();

    FWXML_API virtual ~SerializeXML();

    // override call serialize service for object
    FWXML_API virtual void visit( ::fwData::Object::sptr obj);

    /// correspondance within node object and xmlNode
    std::map< ::fwData::Object::sptr , xmlNodePtr>  m_correspondance;
};

}

#endif /*_FWXML_VISITOR_SERIALIZEXML_HPP*/
