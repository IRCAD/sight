/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_COMPOSITEEDITOR_HPP_
#define _FWCOMED_COMPOSITEEDITOR_HPP_

#include <map>

#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwData
{
    class Composite;
}

namespace fwComEd
{

/**
 * @brief   Defines the composite edition service.
 * @struct  CompositeEditor
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @todo    Really difference with DefaultEditor ? updating(msg) method are equivalent ?
 */
struct FWCOMED_CLASS_API CompositeEditor : public fwServices::IEditionService
{
    fwCoreServiceClassDefinitionsMacro ( (CompositeEditor)(::fwServices::IEditionService::Baseclass) ) ;

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~CompositeEditor() throw() ;

    /// Implements starting method derived from IService. Do nothing.
    FWCOMED_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    FWCOMED_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements configuring method derived from IService. Do nothing.
    FWCOMED_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements reconfiguring method derived from IService. Do nothing.
    FWCOMED_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    FWCOMED_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    FWCOMED_API virtual void info( std::ostream &_sstream );

    /**
     * @brief   This method is not used, it raise a fatal message.
     * @todo    This method is used ?
     */
    //particular case where composite editor is client for observation : it builds a new po message carrying the fired _msg
    FWCOMED_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
};

} // namespace fwComEd

#endif //_FWCOMED_COMPOSITEEDITOR_HPP_
