/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VISITOR_ACCEPT_HPP_
#define _FWDATA_VISITOR_ACCEPT_HPP_

#include "fwData/visitor/IVisitor.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
namespace visitor
{

/**
 *  @brief externalisation of Design Pattern Vistor
 *
 * 	Each object can be visited. An object accept a visitor. This visitor visits(performs an action) on object via
 *  an concret implement of Visitor::visit( obj ). Then visitor continues to obj children using Visitor::next( obj, obj'schildren )
 *  strategy implemented via  an concret implementation of this method
 *
 * @param[in] object the object which accept the visitor
 * @param[in] visitor the visitor which visit the object
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */

FWDATA_API void accept( ::fwTools::Object::sptr object,  ::fwData::visitor::IVisitor *visitor );

} // namespace visitor

} // namespace fwData

#endif // _FWDATA_VISITOR_ACCEPT_HPP_
