/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IONETWORK_OBJECTUPDATENOTIFIER_HPP__
#define __IONETWORK_OBJECTUPDATENOTIFIER_HPP__

#include <map>
#include <vector>
#include <fwCore/util/FactoryRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include "ioNetwork/config.hpp"

namespace ioNetwork
{
/**
 * @class ObjectUpdateNotifier
 *
 * @brief utility class to notify a object is modified with the correct msg event
 */
class IONETWORK_CLASS_API ObjectUpdateNotifier
{
public:

    /**
     * @typedef EventMsgRegistryType
     *
     * @brief map of object msg classname and event name to set to say the object is modified
     */
    typedef std::map<std::string, std::string > EventMsgRegistryType;

    /**
     * @brief notify object is modified
     *
     * @param[in] service service where the object is modified
     */
    IONETWORK_API static void notifyUpdate(const ::fwServices::IService::sptr &service);

private:

    /// event msg factory
    static const EventMsgRegistryType s_EVENT_MSG_REGISTRY;
};
}//namespace ioNetwork

#endif //__IONETWORK_OBJECTUPDATENOTIFIER_HPP__
