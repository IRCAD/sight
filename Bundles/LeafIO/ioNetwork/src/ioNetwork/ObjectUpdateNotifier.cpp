/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/String.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Point.hpp>
#include <fwData/Color.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/MeshMsg.hpp>
#include <fwComEd/helper/MsgHelper.hpp>

#include <fwCom/Signal.hxx>
#include <boost/foreach.hpp>

#include "ioNetwork/ObjectUpdateNotifier.hpp"

namespace ioNetwork
{

//-----------------------------------------------------------------------------

const ObjectUpdateNotifier::EventMsgRegistryType
      ObjectUpdateNotifier::s_EVENT_MSG_REGISTRY =
        ::boost::assign::map_list_of
        (::fwComEd::TransformationMatrix3DMsg::classname(),::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED)
        (::fwComEd::ImageMsg::classname(),::fwComEd::ImageMsg::NEW_IMAGE)
        (::fwComEd::MeshMsg::classname(),::fwComEd::MeshMsg::NEW_MESH);

//-----------------------------------------------------------------------------

void ObjectUpdateNotifier::notifyUpdate (const ::fwServices::IService::sptr &service)
{
    EventMsgRegistryType::const_iterator eventRegistryIt;
    ::fwData::Object::sptr obj = service->getObject();
    ::fwServices::ObjectMsg::sptr objMsg;

    objMsg = ::fwComEd::helper::MsgHelper::createAssociatedMsg(obj);

    if (objMsg)
    {
        if ( (eventRegistryIt = s_EVENT_MSG_REGISTRY.find (objMsg->getClassname())) != s_EVENT_MSG_REGISTRY.end())
        {
            objMsg->addEvent (eventRegistryIt->second);

        }
        objMsg->addEvent (::fwServices::ObjectMsg::UPDATED_OBJECT);
        objMsg->setSubject (obj);

        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = obj->signal< ::fwData::Object::ObjectModifiedSignalType> (::fwData::Object::s_OBJECT_MODIFIED_SIG);
        fwServicesNotifyMacro(service->getLightID(), sig, (objMsg));
    }
}

//-----------------------------------------------------------------------------
} // namespace ioNetwork
