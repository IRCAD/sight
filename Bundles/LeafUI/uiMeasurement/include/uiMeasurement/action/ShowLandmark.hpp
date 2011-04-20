/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP_
#define _UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP_

#include <fwGui/IActionSrv.hpp>
#include <fwData/Image.hpp>
#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action allows to show/hide image landmarks.
 * @class   ShowLandmark.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIMEASUREMENT_CLASS_API ShowLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ShowLandmark)( ::fwGui::IActionSrv) ) ;
    UIMEASUREMENT_API ShowLandmark() throw() ;

    UIMEASUREMENT_API virtual ~ShowLandmark() throw() ;

protected:

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    void stopping() throw (::fwTools::Failed);

    UIMEASUREMENT_API void swapping() throw(::fwTools::Failed) ;
    UIMEASUREMENT_API void info(std::ostream &_sstream ) ;

protected :

    ::fwData::Image::wptr m_observedImage;

};

} // namespace action

} // namespace uiMeasurement

#endif // _UIMEASUREMENT_ACTION_SHOWLANDMARK_HPP_
