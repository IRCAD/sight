/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLNETWORK_INETWORK_HPP__
#define __IGTLNETWORK_INETWORK_HPP__

#include "igtlNetwork/config.hpp"

#include <igtlProtocol/DataConverter.hpp>

#include <boost/type.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Object.hpp>

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
    IGTLNETWORK_API bool receiveObject(::fwData::Object::sptr dest);

    /**
     * @brief generic method to receive message the type of the message is determined by response header
     *
     * @return a smart pointer of igtl::MessageHeader and a pointer on igtlMessageBase
     */
    IGTLNETWORK_API bool receiveMsg(::igtl::MessageHeader::Pointer headerMsg,
                                    ::igtl::MessageBase::Pointer msg);

    /**
     * @brief generic method to send a object the type of object is determined by classname
     *        this method call the correct sender method. If the client is not connected you receive
     *        a sigpipe signal
     * @param[in] dest object to send
     */
    IGTLNETWORK_API bool sendObject (::fwData::Object::sptr dest);

    /**
     * @brief generic method to send a igtl Msg, this method is usefull for redirect message
     * @param[in] msg message to send
     */
    IGTLNETWORK_API bool sendMsg (::igtl::MessageBase::Pointer msg);

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
    IGTLNETWORK_API ::igtl::MessageBase::Pointer receiveBody (::igtl::MessageHeader::Pointer header);;

    /**
     * @brief get socket
     *
     * @return socket
     */
    IGTLNETWORK_API ::igtl::Socket::Pointer getSocket();

    /**
     * @brief add a new authorized device name
     * @param[in] std::string device name
     */
    IGTLNETWORK_API void addAuthorizedDevice(std::string deviceName);

    /**
     * @brief get filteringByDeviceName (true if activated, false otherwise)
     *
     * @return boolean
     */
    IGTLNETWORK_API bool getFilteringByDeviceName();

    /**
     * @brief activate/desactivate the filtering by device name
     * note that if addAuthorizedDevice() is never called this option is automaticaly desactivate
     *
     * @param[in] boolean
     */
    IGTLNETWORK_API void setFilteringByDeviceName(bool filtering);

    /**
     * @brief set the device name when a message is sended
     */
    IGTLNETWORK_API void setDeviceNameOut(std::string deviceName);

    /**
     * @brief get the device name when a message is sended
     *
     * return std::string
     */
    IGTLNETWORK_API std::string getDeviceNameOut();

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

    /// device name in the sended message
    std::string m_deviceNameOut;
};

} // namespace igtlNetwork

#endif // __IGTLNETWORK_INETWORK_HPP__

