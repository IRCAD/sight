/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_VISITOR_SERIALIZE_HPP
#define _FWXML_VISITOR_SERIALIZE_HPP

#include "fwXML/config.hpp"
#include "fwXML/visitor/IVisitor.hpp"

namespace visitor
{



/**
 * @brief visit all objects then call XMLpartionner on each visit
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API Serialize : public  ::fwData::visitor::IVisitor
{
public:
    FWXML_API Serialize();

    FWXML_API virtual ~Serialize();

    // override call serialize service for object
    FWXML_API virtual void visit( ::fwData::Object::sptr obj);
};

}

#endif /*_FWXML_VISITOR_SERIALIZE_HPP*/
