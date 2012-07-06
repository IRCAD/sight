/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_VISITOR_IVISITOR_HPP_
#define _FWXML_VISITOR_IVISITOR_HPP_

#include "fwXML/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{
namespace visitor
{

/**
 * @brief Interface for Visitor Pattern.
 * @warning : FIXME Visitor store father dataObject information : 2 successives call are not *yet* supported
 *             destroy then restart visitor
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @todo implement a reset father two allow 2 successive call
 */
class  FWXML_CLASS_API IVisitor : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IVisitor) );

    FWXML_API IVisitor();

    FWXML_API virtual ~IVisitor();

    /**
     * @brief to be specialized in sub class : define operation to perform on an object
     */
    FWXML_API virtual void visit( ::fwData::Object::sptr obj)=0;

protected :

    /**
     * @brief store information of previous hierarchical position of visitor
     */
    ::fwData::Object::sptr m_source;


};

} // end namespace visitor

} // end namespace fwData


#endif // _FWXML_VISITOR_IVISITOR_HPP_
