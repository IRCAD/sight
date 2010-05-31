/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ICOMMUNICATION_HPP_
#define _FWSERVICES_ICOMMUNICATION_HPP_

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @class   ICommunication.
 * @brief   This interface defines communication service API.
 *
 * Does nothing except defining a service type dedicated to communications. This is useful to consider communication as a first class entity, in particular to monitor communication and to facilitate their global (un)activation. See ComChannelService for details
 * regarding to the communication mechanism (default implementation).
 *
 * @note    ComChannelService content could moved to this class
 * @author IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API ICommunication : public ::fwServices::IService
{

public:

    /// Definitions
    fwCoreServiceClassDefinitionsMacro ( (ICommunication)(::fwServices::IService) ) ;

    ///@brief ICommunication constructor. Do nothing.
    FWSERVICES_API ICommunication();

    ///@brief ICommunication destructor. Do nothing.
    FWSERVICES_API virtual ~ICommunication();

};

}

#endif // _FWSERVICES_ICOMMUNICATION_HPP_
