/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwNetworkIO/config.hpp"
#include "fwNetworkIO/exceptions/Base.hpp"

namespace fwNetworkIO
{

namespace exceptions
{

/**
 * @brief Implements exception for an HTTP connection refused.
 */
class FWNETWORKIO_CLASS_API ConnectionRefused : public fwNetworkIO::exceptions::Base
{
public:
    /**
     * @name Constructor/Destructor.
     * @{ */
    FWNETWORKIO_API ConnectionRefused(const std::string& message);
    virtual ~ConnectionRefused() noexcept;
    /**  @} */
};

} /* namespace exceptions */
} /* namespace fwNetworkIO */
