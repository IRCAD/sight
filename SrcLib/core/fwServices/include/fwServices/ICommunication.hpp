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
class IEditionService ;
}
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

    //@{
    /**
     * @brief Defines the object to observe (internally stored with weak reference)
     */
    FWSERVICES_API virtual void setSrc( SPTR( ::fwServices::IEditionService ) src) = 0;

    /// Defines the target service
    FWSERVICES_API virtual void setDest( ::fwServices::IService::sptr _client) = 0;

    /// Returns the target service
    FWSERVICES_API virtual IService::sptr  getDest() = 0;

    /// Returns the target service
    FWSERVICES_API virtual SPTR( ::fwServices::IEditionService )  getSrc() = 0;

    /// Return true is both source and destination are valid pointers
    FWSERVICES_API virtual bool isValid() = 0;
    //@}

protected:

    ///@brief ICommunication constructor. Do nothing.
    FWSERVICES_API ICommunication();

    ///@brief ICommunication destructor. Do nothing.
    FWSERVICES_API virtual ~ICommunication();

};

}

#endif // _FWSERVICES_ICOMMUNICATION_HPP_
