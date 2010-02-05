/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _PROCESSOBJECT_EDITOR_HPP_
#define _PROCESSOBJECT_EDITOR_HPP_

#include <map>

#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwData
{
        class ProcessObject;
}

namespace fwComEd
{

/**
 * @brief       Defines the interface of operator object edition service.
 * @struct      ProcessObjectEditor
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @todo        really difference with DefaultEditor ? updating(msg) method are equivalent ?
 */
struct FWCOMED_CLASS_API ProcessObjectEditor : public fwServices::IEditionService
{
    fwCoreServiceClassDefinitionsMacro ( ( ProcessObjectEditor ) ( ::fwServices::IEditionService::Baseclass) ) ;

        /**
         * @brief       destructor
         */
        FWCOMED_API virtual ~ProcessObjectEditor() throw() ;

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
         * @brief       This method is not used, it raise a fatal message.
         * @todo        This method is used ?
         */
        FWCOMED_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);


};

} // namespace fwComEd

#endif //_PROCESSOBJECT_EDITOR_HPP_
