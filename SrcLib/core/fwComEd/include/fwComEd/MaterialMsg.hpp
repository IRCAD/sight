/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_MATERIALMSG_HPP__
#define __FWCOMED_MATERIALMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for material : store modification information
 * @class   MaterialMsg
 *
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API MaterialMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((MaterialMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< MaterialMsg >);

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string MATERIAL_IS_MODIFIED;
    /// @}

    /**
     * @brief   Constructor : do nothing.
     */
    FWCOMED_API MaterialMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~MaterialMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_MATERIALMSG_HPP__

