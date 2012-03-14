/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP
#define _FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP

#include <fwXML/visitor/IVisitor.hpp>
#include <fwXML/IFileFormatService.hpp>

#include "fwXML/config.hpp"

namespace visitor
{

/**
 * @brief visit all objects then store FileFormatService if any
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API CollectFileFormatService : public  ::fwData::visitor::IVisitor
{
public:
    FWXML_API CollectFileFormatService();

    FWXML_API virtual ~CollectFileFormatService();

    // override call CollectFileFormatService service for object
    FWXML_API virtual void visit( ::fwData::Object::sptr obj);

    typedef  std::map< ::fwData::Object::sptr , ::fwXML::IFileFormatService::sptr > MapObjectFileFormatService;
    MapObjectFileFormatService m_objWithFileFormatService;


};

}

#endif /*_FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP*/
