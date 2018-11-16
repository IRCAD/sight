/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "igtlNetwork/config.hpp"

#include <fwCore/Exception.hpp>

#include <fwData/Object.hpp>

#include <igtlProtocol/DataConverter.hpp>

#include <igtl/igtlMessageHeader.h>
#include <igtl/igtlSocket.h>

#include <set>
#include <string>

namespace igtlNetwork
{

/**
 *
 * @brief a interface for client and server classes
 */
class IGTLNETWORK_CLASS_API INetwork
{
public:
    /**
     * @brief default constructor
     */
    IGTLNETWORK_API INetwork();

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IGTLNETWORK_API virtual ~INetwork();

    /**
     * @brief generic method to receive object the type of the object is determined by response header
     * this method call the correct receiver for the type found in response header
     *
     * @return a smart pointer of fwData object
     */
    IGTLNETWORK_API ::fwData::Object::sptr receiveObject(std::string& deviceName);

    /**
     * @brief generic method to receive object the type of the object is determined by response header
     * this method calls the correct receiver for the type found in response header and sets the timestamp
     * parameter according to the timestamp set in the message
     *
     * @return a smart pointer of fwData object
     */
    IGTLNETWORK_API ::fwData::Object::sptr receiveObject(std::string& deviceName, double& timestamp);

    /**
     * @brief generic method to send a object the type of object is determined by classname
     *        this method call the correct sender method. If the client is not connected you receive
     *        a sigpipe signal
     * @param[in] dest object to send
     */
    IGTLNETWORK_API bool sendObject(const ::fwData::Object::csptr& dest);

    /**
     * @brief generic method to send a igtl Msg, this method is useful for redirect message
     * @param[in] msg message to send
     */
    IGTLNETWORK_API bool sendMsg(::igtl::MessageBase::Pointer msg);

    /**
     * @brief receive header
     * @return header
     */

    IGTLNETWORK_API ::igtl::MessageHeader::Pointer receiveHeader();

    /** @brief receive body pack
     *
     *  @param[in] header msg header
     *
     *  @return Message
     */
    IGTLNETWORK_API ::igtl::MessageBase::Pointer receiveBody (::igtl::MessageHeader::Pointer header);

    /**
     * @brief get socket
     *
     * @return socket
     */
    IGTLNETWORK_API ::igtl::Socket::Pointer getSocket() const;

    /**
     * @brief add a new authorized device name
     * @param[in] std::string device name
     */
    IGTLNETWORK_API void addAuthorizedDevice(const std::string& deviceName);

    /**
     * @brief get filteringByDeviceName (true if activated, false otherwise)
     *
     * @return boolean
     */
    IGTLNETWORK_API bool getFilteringByDeviceName() const;

    /**
     * @brief activate/desactivate the filtering by device name
     * note that if addAuthorizedDevice() is never called this option is automatically disabled
     *
     * @param[in] boolean
     */
    IGTLNETWORK_API void setFilteringByDeviceName(bool filtering);

    /**
     * @brief set the device name when a message is sent
     */
    IGTLNETWORK_API void setDeviceNameOut(const std::string& deviceName);

    /**
     * @brief get the device name when a message is sent
     *
     * return std::string
     */
    IGTLNETWORK_API std::string getDeviceNameOut() const;

protected:
    /// client socket
    ::igtl::Socket::Pointer m_socket;

    /// DataConverter instance to convert fwData::Object
    /// to igtl::MessageBase and igtl::MessageBase to fwData::Object
    ::igtlProtocol::DataConverter::sptr m_dataConverter;

    /// Filter the message by device name
    bool m_filteringByDeviceName;

    /// Set of authorized device names
    std::set< std::string > m_deviceNamesIn;

    /// device name in the sent message
    std::string m_deviceNameOut;
};

} // namespace igtlNetwork
