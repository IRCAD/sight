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

#ifndef __UITF_PLUGIN_HPP__
#define __UITF_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiTF/config.hpp"

namespace uiTF
{
/**
 *
 * @date    2009-2010.
 */
class UITF_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    UITF_API ~Plugin() noexcept;

    // Overrides
    UITF_API void start();

    // Overrides
    UITF_API void stop() noexcept;

};

} // namespace uiTF

#endif //__UITF_PLUGIN_HPP__
