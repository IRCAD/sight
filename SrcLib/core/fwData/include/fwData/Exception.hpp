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

