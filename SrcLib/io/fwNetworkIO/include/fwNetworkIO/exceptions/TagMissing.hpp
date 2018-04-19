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
 * @brief TagMissing Exceptions
 */
class FWNETWORKIO_CLASS_API TagMissing : public fwNetworkIO::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWNETWORKIO_API TagMissing(const std::string& message);

    /// Destructor
    FWNETWORKIO_API virtual ~TagMissing() noexcept;

};

} //exceptions
} //fwDicomWeb
