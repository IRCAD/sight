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
 * @brief Implements exception for HTTP content not found errors.
 */
class FWNETWORKIO_CLASS_API ContentNotFound : public fwNetworkIO::exceptions::Base
{
public:
    /**
     * @name Constructor/Destructor.
     * @{ */
    FWNETWORKIO_API ContentNotFound(const std::string& message);
    virtual ~ContentNotFound() noexcept;
    /**  @} */
};

} /* namespace exceptions */
} /* namespace fwNetworkIO */
