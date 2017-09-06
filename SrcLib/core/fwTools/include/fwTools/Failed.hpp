/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_FAILED_HPP__
#define __FWTOOLS_FAILED_HPP__

#include <exception>
#include <string>

#include "fwCore/Exception.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief   Implements a failed exception class
 * @class   Failed
 *
 * @date    2007-2009.
 */
struct FWTOOLS_CLASS_API Failed : public ::fwCore::Exception
{

    /**
     * @brief       Constructor
     *
     * @param[in]   message a string containing the failure message
     */
    FWTOOLS_API Failed(const std::string &message) noexcept;

    /**
     * @brief   Destructor : do nothing.
     */
    FWTOOLS_API ~Failed() noexcept;


};

} // namespace fwTools

#endif // __FWTOOLS_FAILED_HPP__
