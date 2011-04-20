/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP_
#define _UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action moves the image slice on chosen landmark.
 * @class   FocusLandmark.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIMEASUREMENT_CLASS_API FocusLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro ( (FocusLandmark)( ::fwGui::IActionSrv) ) ;
    UIMEASUREMENT_API FocusLandmark() throw() ;

    UIMEASUREMENT_API virtual ~FocusLandmark() throw() ;

protected:

    UIMEASUREMENT_API void starting() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void stopping() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void configuring() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void updating() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void info ( std::ostream &_sstream ) ;

};

} // namespace action
} // namespace uiMeasurement

#endif // _UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP_
