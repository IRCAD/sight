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

#ifndef __FWATOMSPATCH_EXCEPTIONS_MISSINGINFORMATION_HPP__
#define __FWATOMSPATCH_EXCEPTIONS_MISSINGINFORMATION_HPP__

#include "fwAtomsPatch/exceptions/Base.hpp"

namespace fwAtomsPatch
{

namespace exceptions
{

/**
 * @brief Reports an missing information required for data object patching.
 */
class MissingInformation : public fwAtomsPatch::exceptions::Base
{
public:
    MissingInformation(const std::string& message);
    virtual ~MissingInformation() noexcept
    {
    }
};

} // namespace exceptions
} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_EXCEPTIONS_MISSINGINFORMATION_HPP__ */
