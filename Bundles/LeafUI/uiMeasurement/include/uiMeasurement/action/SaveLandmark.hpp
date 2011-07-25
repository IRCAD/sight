/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEASUREMENT_UI_ACTION_SAVELANDMARK_HPP_
#define _UIMEASUREMENT_UI_ACTION_SAVELANDMARK_HPP_

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

class UIMEASUREMENT_CLASS_API SaveLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SaveLandmark)( ::fwGui::IActionSrv) ) ;

protected:

    UIMEASUREMENT_API SaveLandmark() throw() ;

    UIMEASUREMENT_API virtual ~SaveLandmark() throw() ;

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    void stopping() throw (::fwTools::Failed);

    UIMEASUREMENT_API void info(std::ostream &_sstream ) ;


};

} // namespace action

} // namespace uiMeasurement

#endif // _UIMEASUREMENT_UI_ACTION_SAVELANDMARK_HPP_
