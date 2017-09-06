/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_EXCEPTION_HPP__
#define __FWDATA_EXCEPTION_HPP__

#include "fwCore/Exception.hpp"
#include "fwData/config.hpp"

#include <exception>
#include <string>

namespace fwData
{

/**
 * @brief   Implements data exception class.
 */
class FWDATA_CLASS_API Exception : public ::fwCore::Exception
{
public:

    /**
     * @brief       Constructor
     *
     * @param[in]   message a string containing the failure message
     */
    FWDATA_API Exception(const std::string &message) noexcept;

    /**
     * @brief   Destructor : do nothing.
     */
    FWDATA_API ~Exception() noexcept;
};

} // namespace fwData

#endif // __FWDATA_EXCEPTION_HPP__

