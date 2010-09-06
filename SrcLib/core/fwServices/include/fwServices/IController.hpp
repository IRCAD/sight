/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICES_ICONTROLLER_HPP_
#define FWSERVICES_ICONTROLLER_HPP_

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @brief   This interface defines control service API. Does nothing particularly, can be considered as a default service type to be implemented by unclassified services.
 *
 * @class   IController.
 * @author IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API IController : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (IController)(::fwServices::IService) ) ;

protected:

    ///@brief IController constructor. Do nothing.
    FWSERVICES_API IController();

    ///@brief IController destructor. Do nothing.
    FWSERVICES_API virtual ~IController();

};

}

#endif // FWSERVICES_ICONTROLLER_HPP_
