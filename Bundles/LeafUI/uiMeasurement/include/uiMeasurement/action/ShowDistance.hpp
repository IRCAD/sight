/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP_
#define _UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP_

#include <fwGui/IActionSrv.hpp>
#include <fwData/Image.hpp>
#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action allows to show/hide image distances.
 * @class   ShowDistance.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIMEASUREMENT_CLASS_API ShowDistance : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ShowDistance)( ::fwGui::IActionSrv) ) ;
    UIMEASUREMENT_API ShowDistance() throw() ;

    UIMEASUREMENT_API virtual ~ShowDistance() throw() ;

protected:

    // update its show/hide flag accordinf image ShowDistance field notification
    UIMEASUREMENT_API void updating(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed);

    UIMEASUREMENT_API void swapping() throw(::fwTools::Failed) ;

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void stopping() throw (::fwTools::Failed);

    UIMEASUREMENT_API void info(std::ostream &_sstream ) ;


protected :

    ::fwData::Image::wptr m_observedImage;

};

} // namespace action

} // namespace uiMeasurement

#endif // _UIMEASUREMENT_ACTION_SHOWDISTANCE_HPP_
