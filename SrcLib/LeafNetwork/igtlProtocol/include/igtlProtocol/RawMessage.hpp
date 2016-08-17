/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_RAWMESSAGE_HPP__
#define __IGTLPROTOCOL_RAWMESSAGE_HPP__

#include "igtlProtocol/config.hpp"

#include <boost/cstdint.hpp>
#include <igtl/igtlMessageBase.h>

#include <string>
#include <vector>

namespace igtlProtocol
{
/**
 *
 * @brief a OpenIGTLink message which you can store raw data
 */
class IGTLPROTOCOL_CLASS_API RawMessage :
    public igtl::MessageBase
{
public:

    typedef RawMessage Self;
    typedef MessageBase Superclass;
    typedef igtl::SmartPointer<Self> Pointer;
    typedef igtl::SmartPointer<const Self> ConstPointer;
    typedef std::vector< char > RawDataType;

    igtlTypeMacro(RawMessage, igtl::MessageBase);

    /// Constructor
    IGTLPROTOCOL_API RawMessage(std::string const& bodyType);

    /// Destructor
    IGTLPROTOCOL_API ~RawMessage();

    /**
     * @brief append a string to raw data
     */
    IGTLPROTOCOL_API void append(const RawDataType& content);

    /**
     * @brief append bytes to raw data
     */
    IGTLPROTOCOL_API void append(const char* data, std::size_t size);

    /**
     * @brief get immutable message
     *
     * @return a immutable byte vector
     */
    IGTLPROTOCOL_API RawDataType const& getMessage() const;

    /**
     * @brief get mutable message
     *
     * @return a mutable byte vector
     */
    IGTLPROTOCOL_API RawDataType& getMessage();

    /**
     * @brief create a new igtl smart pointer to a raw message
     *
     * @return igtl smart pointer to a raw message
     */
    IGTLPROTOCOL_API static RawMessage::Pointer New(std::string const& bodyType);

private:
    /// Override
    int GetBodyPackSize();

    /// Override
    int PackBody();

    /// Override
    int UnpackBody();

private:

    /// raw data
    RawDataType m_msg;
};
}//namespace igtlProtocol

#endif // __IGTLPROTOCOL_RAWMESSAGE_HPP__
