/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__
#define __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__

#include <fwServices/ObjectMsg.hpp>
#include <fwData/macros.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace common
{

namespace data
{

/**
 * @brief   This class represents a Progress Event
 * @class   ProgressMsg
 * @date    2013.
 */
class IODICOMEXT_CLASS_API ProgressMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ProgressMsg)(::fwServices::ObjectMsg), (()),
                                            ::fwServices::factory::message::New< ProgressMsg >);

    /// Event identifier used to inform that the progress bar must be started.
    IODICOMEXT_API static const std::string s_START_PROGRESS_BAR;

    /// Event identifier used to inform that the progress bar must be stopped.
    IODICOMEXT_API static const std::string s_STOP_PROGRESS_BAR;

    /// Event identifier used to inform that the progress bar must be updated.
    IODICOMEXT_API static const std::string s_UPDATE_PROGRESS_BAR;

    /**
     * @brief Constructor
     */
    IODICOMEXT_API ProgressMsg(::fwServices::ObjectMsg::Key key) throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~ProgressMsg() throw();

    fwGettersSettersDocMacro(BarID,barId,std::string,Get/Set Progress Bar ID);
    fwGettersSettersDocMacro(Message,message,std::string,Get/Set Message);
    fwGettersSettersDocMacro(Percentage,percentage,float,Get/Set Percentage);

private:
    /// Progress ID
    std::string m_barId;

    /// Progress Message
    std::string m_message;

    /// Percentage
    float m_percentage;

};

} // namespace data
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__
