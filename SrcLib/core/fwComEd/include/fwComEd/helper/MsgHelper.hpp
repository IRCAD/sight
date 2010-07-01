/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_HELPER_MSGHELPER_HPP_
#define _FWCOMED_HELPER_MSGHELPER_HPP_


#include <fwServices/IService.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

namespace helper
{

/**
 * @class   MsgHelper
 * @brief   This class contains helpers for Messages.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class FWCOMED_CLASS_API MsgHelper
{
public :

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

#endif /* _FWCOMED_HELPER_MSGHELPER_HPP_ */
