/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwNetworkIO/config.hpp"

#include <fwCore/Exception.hpp>

namespace fwNetworkIO
{
namespace exceptions
{

/**
 * @brief Base class of fwNetworkIO Exception
 */
class FWNETWORKIO_CLASS_API Base : public fwCore::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWNETWORKIO_API Base(const std::string& message);

    /// Destructor
    virtual ~Base() noexcept;
};

} // namespace exceptions
} // namespace fwNetworkIO
