/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "io/igtl/config.hpp"

#include <core/Exception.hpp>

#include <data/Object.hpp>

#include <igtlMessageHeader.h>
#include <igtlSocket.h>

#include <set>
#include <string>

namespace sight::io::igtl
{

/**
 *
 * @brief a interface for client and server classes
 */
class IO_IGTL_CLASS_API INetwork
{
public:

    /**
     * @brief default constructor
     */
    IO_IGTL_API INetwork();

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IO_IGTL_API virtual ~INetwork();

    /**
     * @brief generic method to receive object the type of the object is determined by response header
     * this method call the correct receiver for the type found in response header
     *
     * @return a smart pointer of fwData object
     */
    IO_IGTL_API data::Object::sptr receiveObject(std::string& deviceName);

    /**
     * @brief generic method to receive object
     * the type of the object is determined by response header
     * this method calls the correct receiver for the type found in response header and sets the timestamp
     * parameter according to the timestamp set in the message
     *
     * @return a smart pointer of fwData object
     */
    IO_IGTL_API data::Object::sptr receiveObject(std::string& deviceName, double& timestamp);

    /**
     * @brief generic method to send a object the type of object is determined by classname
     *        this method call the correct sender method. If the client is not connected you receive
     *        a sigpipe signal
     * @param[in] dest object to send
     */
    IO_IGTL_API bool sendObject(const data::Object::csptr& dest);

    /**
     * @brief generic method to send a igtl Msg, this method is useful for redirect message
     * @param[in] msg message to send
     */
    IO_IGTL_API bool sendMsg(::igtl::MessageBase::Pointer msg);

    /**
     * @brief receive header
     * @throw igtl::Exception on error (network error or timeout).
     * @return header
     */

    IO_IGTL_API ::igtl::MessageHeader::Pointer receiveHeader();

    /** @brief receive body pack
     *
     *  @param[in] header msg header
     *  @throw igtl::Exception on error (network error or timeout).
     *  @return Message
     */
    IO_IGTL_API ::igtl::MessageBase::Pointer receiveBody(::igtl::MessageHeader::Pointer header);

    /**
     * @brief get socket
     *
     * @return socket
     */
    IO_IGTL_API ::igtl::Socket::Pointer getSocket() const;

    /**
     * @brief add a new authorized device name
     * @param[in] std::string device name
     */
    IO_IGTL_API void addAuthorizedDevice(const std::string& deviceName);

    /**
     * @brief get filteringByDeviceName (true if activated, false otherwise)
     *
     * @return boolean
     */
    IO_IGTL_API bool getFilteringByDeviceName() const;

    /**
     * @brief activate/desactivate the filtering by device name
     * note that if addAuthorizedDevice() is never called this option is automatically disabled
     *
     * @param[in] boolean
     */
    IO_IGTL_API void setFilteringByDeviceName(bool filtering);

    /**
     * @brief set the device name when a message is sent
     */
    IO_IGTL_API void setDeviceNameOut(const std::string& deviceName);

    /**
     * @brief get the device name when a message is sent
     *
     * return std::string
     */
    IO_IGTL_API std::string getDeviceNameOut() const;

protected:

    /// client socket
    ::igtl::Socket::Pointer m_socket;

    /// Filter the message by device name
    bool m_filteringByDeviceName;

    /// Set of authorized device names
    std::set<std::string> m_deviceNamesIn;

    /// device name in the sent message
    std::string m_deviceNameOut;
};

} // namespace sight::io::igtl
