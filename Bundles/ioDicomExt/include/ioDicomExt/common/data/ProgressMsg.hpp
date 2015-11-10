/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__
#define __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

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

    const std::string& getBarID() const;
    void setBarID(const std::string& barId);

    const std::string& getMessage() const;
    void setMessage(const std::string& message);

    float getPercentage() const;
    void setPercentage(float percentage);

private:
    /// Progress ID
    std::string m_barId;

    /// Progress Message
    std::string m_message;

    /// Percentage
    float m_percentage;

};

//------------------------------------------------------------------------------

inline const std::string& ProgressMsg::getBarID() const
{
    return m_barId;
}

//------------------------------------------------------------------------------

inline void ProgressMsg::setBarID(const std::string& barId)
{
    m_barId = barId;
}

//------------------------------------------------------------------------------

inline const std::string& ProgressMsg::getMessage() const
{
    return m_message;
}

//------------------------------------------------------------------------------

inline void ProgressMsg::setMessage(const std::string& message)
{
    m_message = message;
}

//------------------------------------------------------------------------------

inline float ProgressMsg::getPercentage() const
{
    return m_percentage;
}

//------------------------------------------------------------------------------

inline void ProgressMsg::setPercentage(float percentage)
{
    m_percentage = percentage;
}

//------------------------------------------------------------------------------

} // namespace data
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_DATA_PROGRESSMSG_HPP__
