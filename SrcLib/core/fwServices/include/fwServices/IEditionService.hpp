/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_IEDITIONSERVICE_HPP_
#define _FWSERVICES_IEDITIONSERVICE_HPP_

#include <list>

#include <fwTools/Object.hpp>


#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ObjectMsg.hpp"

namespace fwServices
{
class ObjectMsg ;
}

namespace fwServices
{

/**
 * @brief   This interface defines edition service API. It mainly has the role of the subject in the observer design pattern.
 * @class   IEditionService
 * 

 * @date    2007-2009.
 *
 * This service notifies modifications performed on its associated object. This can be seen a delegate of traditional notifications performed by objects (subjects) which are themselves subject to modification. This ensures a good separation of concerns.
 * @todo the design assumes that any object can have only one service of type IEditionService. For robustness, this should be controlled.
 * @note notification are propagated to "listening" services by invoking the update(const fwServices::ObjectMsg::csptr  ) methods on referenced communication channels which forward this to their destination service acting as the observer.
 * @note could be improved by directly registering the destination service instead of the communication channel. Communication channel, being a first class entity between the subject and observer, have to be preferred to facilitate the configuration, the (des)activation and the monitoring of communications over a software system.
 * @note special thanks to Guillaume Brocker
 */
class FWSERVICES_CLASS_API IEditionService : public fwServices::IService
{
    friend class GlobalEventManager;

public :

    fwCoreServiceClassDefinitionsMacro ( (IEditionService)(::fwServices::IService) ) ;

    /**
     * @brief Invoke the IEditionService::notify( ObjectMsg::csptr ) method on _pSubject.
     * @param[in] _pSource The service which send the message
     * @param[in] _pSubject The subject of the message ( to retrieve listeners of the subject )
     * @param[in] _pMsg message notify to associated object listeners
     *
     *  This method notifies observers of _pSubject. It changes notification status of _pSource during all notification, set correct source and subject of msg and timestamp the message.
     */
    FWSERVICES_API static void notify(
            ::fwServices::IService::sptr _pSource,
            ::fwData::Object::sptr _pSubject,
            ObjectMsg::sptr _pMsg );

    /**
     * @brief Invoke the IEditionService::notify( ObjectMsg::csptr ) method on _pSubject.
     * @param[in] _pSource The service which send the message
     * @param[in] _pSubject The subject of the message ( to retrieve listeners of the subject )
     * @param[in] _pMsg message notify to associated object listeners
     * @param[in] _allowLoops Allow loops (be really careful)
     *
     *  This method notifies observers of _pSubject. It changes notification status of _pSource during all notification, set correct source and subject of msg and timestamp the message.
     */
    FWSERVICES_API static void notify(
            ::fwServices::IService::sptr _pSource,
            ::fwData::Object::sptr _pSubject,
            ObjectMsg::sptr _pMsg,
            bool _allowLoops );

protected:

    /// Constructor. Do nothing.
    FWSERVICES_API IEditionService() ;

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~IEditionService() throw();

};

}

#endif // _FWSERVICES_IEDITIONSERVICE_HPP_
