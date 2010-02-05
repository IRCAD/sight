/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VISITOR_DEPTHFIRST_HPP_
#define _FWDATA_VISITOR_DEPTHFIRST_HPP_

#include "fwData/config.hpp"
#include "fwData/visitor/IVisitor.hpp"

namespace fwData
{
namespace visitor
{

/**
 * @brief Implementation for Visitor Pattern : visitor scan in depth first
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWDATA_CLASS_API DepthFirst : public  IVisitor
{
public:
        fwCoreNonInstanciableClassDefinitionsMacro( (DepthFirst)(IVisitor::Baseclass) ) ;

        FWDATA_API virtual void visit( ::fwTools::Object::sptr obj)=0;

        /// override processing of childs : scan in depth ( aka fields[0].fields[0].fields[0] , fields[0].fields[0].fields[1], ....)
        FWDATA_API virtual void next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields);

};

} // namespace visitor

} // namespace fwData

#endif // _FWDATA_VISITOR_DEPTHFIRST_HPP_
