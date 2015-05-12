/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_RECONSTRUCTIONMSG_HPP__
#define __FWCOMED_RECONSTRUCTIONMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for reconstruction : store modification information
 * @class   ReconstructionMsg
 *
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ReconstructionMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((ReconstructionMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< ReconstructionMsg >);

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string MESH;
    FWCOMED_API static std::string VISIBILITY;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API ReconstructionMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~ReconstructionMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_RECONSTRUCTIONMSG_HPP__

