/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
