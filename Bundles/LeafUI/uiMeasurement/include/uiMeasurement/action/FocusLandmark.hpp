/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP__

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action moves the image slice on chosen landmark.
 * @class   FocusLandmark
 */
class UIMEASUREMENT_CLASS_API FocusLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro ( (FocusLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API FocusLandmark() throw();

    UIMEASUREMENT_API virtual ~FocusLandmark() throw();

protected:

    UIMEASUREMENT_API void starting() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void stopping() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void configuring() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void updating() throw ( ::fwTools::Failed );

    UIMEASUREMENT_API void info ( std::ostream &_sstream );

};

} // namespace action
} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_FOCUSLANDMARK_HPP__
