/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_BREATH_FIRST_H
#define _FWDATA_BREATH_FIRST_H

#include "fwData/config.hpp"
#include "fwData/visitor/IVisitor.hpp"

namespace fwData
{
namespace visitor
{

/**
 * @brief Implementation for Visitor Pattern : visitor scan breath first
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API BreathFirst : public  IVisitor
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (BreathFirst)(IVisitor) ) ;

    FWDATA_API virtual void visit( ::fwTools::Object::sptr obj)=0;

    /// override processing of childs : scan breath first ( aka fields[0] , fields[1] , ...)
    FWDATA_API virtual void next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields);

protected :

    /// store src,child to process in a fifo queue
    typedef ::fwTools::Object::sptr  Source;
    typedef ::fwTools::Object::sptr  Child;
    std::list< std::pair< Source ,Child > > m_fifo;

};

} // namespace visitor

} // namespace fwData

#endif //_FWDATA_BREATH_FIRST_H
