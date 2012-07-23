/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwComEd/helper/MsgHelper.hpp"

namespace fwComEd
{

namespace helper
{

//------------------------------------------------------------------------------

::fwServices::ObjectMsg::sptr MsgHelper::createAssociatedMsg( ::fwData::Object::csptr _obj)
{
    SLM_ASSERT("Object is NULL", _obj);
    const std::string msgType = MsgHelper::getAssociatedMsgType(_obj);

    ::fwServices::ObjectMsg::sptr objectMsg = ::fwServices::factory::message::New(msgType);
    OSLM_ASSERT(msgType << " creation failed", objectMsg);

    return objectMsg;
}

//------------------------------------------------------------------------------

const std::string MsgHelper::getAssociatedMsgType( ::fwData::Object::csptr _obj)
{
    SLM_ASSERT("Object is NULL", _obj);
    // default ObjectMsg type
    std::string msgType = "::fwServices::ObjectMsg";
    std::string objType = _obj->getLeafClassname();

    // TODO: improve association system
    std::string objMsgType = "::fwComEd::" + objType + "Msg";
    // check if instantiation of msgType is possible, standard Factory stop application if type is unknown
    ::fwServices::ObjectMsg::sptr objMsg = ::fwServices::registry::message::InstantiatorType::getInstance()->create( objMsgType ) ;
    OSLM_WARN_IF("No specific ObjectMsg type found for Object "<<objType
            << " type "<<objMsgType<<" is unknown.", !objMsg);
    if(objMsg)
    {
        // Instantiation of msgType is possible
        msgType = objMsgType;
    }

    return msgType;
}

//------------------------------------------------------------------------------

} // helper
} // fwComEd


