/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP
#define _FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP

#include <fwData/visitor/BreathFirst.hpp>
#include <fwXML/IFileFormatService.hpp>

#include "fwXML/config.hpp"

namespace visitor
{

/**
 * @brief visit all objects then store FileFormatService if any
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API CollectFileFormatService : public  ::fwData::visitor::BreathFirst // BreathFirst to prove genericity ,DepthFirst is Better
{
public:
    FWXML_API CollectFileFormatService();

    FWXML_API virtual ~CollectFileFormatService();

    // override call CollectFileFormatService service for object
    FWXML_API virtual void visit( ::fwTools::Object::sptr obj);

    /// override the behavior when visiting composite by visit its fields *and* values in its map
    FWXML_API virtual void next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields);

    typedef  std::map< ::fwTools::Object::sptr , ::fwXML::IFileFormatService::sptr > MapObjectFileFormatService;
    MapObjectFileFormatService m_objWithFileFormatService;


};

}

#endif /*_FWXML_VISITOR_COLLECTFILEFORMATSERVICE_HPP*/
