/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

/**
 * @brief Contains base functionalities used to transform objects from a version to another.
 */
#pragma once

namespace sight::io::atoms::patch
{

/**
 * @brief Contains functions used to control patching process by defining conditions on objects.
 */
namespace conditions
{
}

/**
 * @brief Contains exceptions related to patching process.
 */
namespace exceptions
{
}

/**
 * @brief Contains functions to facilitate object patching.
 */
namespace helper
{
}

/**
 * @brief Contains utilities to report informations about patching.
 */
namespace infos
{
}

/**
 * @brief Contains patchers allowing to transform objects using patches.
 */
namespace patcher
{

/**
 * @brief       Contains io::atoms::patch::factory utilities
 */
namespace factory
{
}         // namespace factory

/**
 * @brief       Contains io::atoms::patch::regjstry utilities
 */
namespace registry
{
}         // namespace registry

}     // namespace patcher

} // namespace sight::io::atoms::patch
