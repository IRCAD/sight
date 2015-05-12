/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_HELPER_MSGHELPER_HPP__
#define __FWCOMED_HELPER_MSGHELPER_HPP__


#include <fwServices/IService.hpp>

#include "fwComEd/config.hpp"

namespace fwComEd
{

namespace helper
{

/**
 * @class   MsgHelper
 * @brief   This class contains helpers for Messages.
 *
 * @date    2010.
 */
class FWCOMED_CLASS_API MsgHelper
{
public:

    /**
     * @brief       Creates associated ObjectMsg for the ::fwData::Object.
     *
     * @param[in]   _obj a specific ::fwData::Object.
     * @return      ObjectMsg associated to the ::fwData::Object.
     */
    FWCOMED_API static ::fwServices::ObjectMsg::sptr createAssociatedMsg( ::fwData::Object::csptr _obj);

    /**
     * @brief       Returns associated ObjectMsg type for the ::fwData::Object.
     *
     * @param[in]   _obj a specific ::fwData::Object.
     * @return      a std::string with the ObjectMsg type associated to the ::fwData::Object.
     */
    FWCOMED_API static const std::string getAssociatedMsgType( ::fwData::Object::csptr _obj);
};

} // helper
} // fwComEd

#endif /* __FWCOMED_HELPER_MSGHELPER_HPP__ */
