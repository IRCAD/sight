/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP_
#define _UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP_

#include <fwData/Image.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"


namespace uiMeasurement
{

namespace action
{

/**
 * @brief   This action removes distances.
 * @class   RemoveDistance.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIMEASUREMENT_CLASS_API RemoveDistance : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (RemoveDistance)( ::fwGui::IActionSrv) ) ;

    UIMEASUREMENT_API RemoveDistance() throw() ;

    UIMEASUREMENT_API virtual ~RemoveDistance() throw() ;

protected:

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    void stopping() throw (::fwTools::Failed);

    UIMEASUREMENT_API void info(std::ostream &_sstream ) ;

private:
    void notifyNewDistance( ::fwData::Image::sptr image, ::fwData::Object::sptr backup );
};

} // namespace action

} // namespace uiMeasurement

#endif // _UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP_
