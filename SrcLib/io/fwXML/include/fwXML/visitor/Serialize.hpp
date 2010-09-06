/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef SERIALIZEBREATH_FIRST_H
#define SERIALIZEBREATH_FIRST_H

#include "fwXML/config.hpp"
#include <fwData/visitor/BreathFirst.hpp>


namespace visitor
{



/**
 * @brief visit all objects then call XMLpartionner on each visit
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API Serialize : public  ::fwData::visitor::BreathFirst // BreathFirst to prove genericity ,DepthFirst is Better
{
public:
    FWXML_API Serialize();

    FWXML_API virtual ~Serialize();

    // override call serialize service for object
    FWXML_API virtual void visit( ::boost::shared_ptr< ::fwTools::Object > obj);


};

}

#endif /*SERIALIZEBREATH_FIRST_H*/
