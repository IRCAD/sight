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

#ifndef __FWATOMSPATCH_NAMESPACE_HPP__
#define __FWATOMSPATCH_NAMESPACE_HPP__

/**
 * @brief Contains base functionalities used to transform objects from a version to another.
 * @namespace fwAtomsPatch
 */
namespace fwAtomsPatch
{

/**
 * @brief Contains functions used to control patching process by defining conditions on objects.
 * @namespace fwAtomsPatch::conditions
 */
namespace conditions
{
}

/**
 * @brief Contains exceptions related to patching process.
 * @namespace fwAtomsPatch::exceptions
 */
namespace exceptions
{
}

/**
 * @brief Contains functions to facilitate object patching.
 * @namespace fwAtomsPatch::helper
 */
namespace helper
{
}

/**
 * @brief Contains utilities to report informations about patching.
 * @namespace fwAtomsPatch::infos
 */
namespace infos
{
}

/**
 * @brief Contains patchers allowing to transform objects using patches.
 * @namespace fwAtomsPatch::patcher
 */
namespace patcher
{

/**
 * @brief       Contains fwAtomsPatch::factory utilities
 * @namespace   fwAtomsPatch::patcher::factory
 * @date        2009-2013
 */
namespace factory
{
}         // namespace factory

/**
 * @brief       Contains fwAtomsPatch::regjstry utilities
 * @namespace   fwAtomsPatch::patcher::registry
 * @date        2009-2013
 */
namespace registry
{
}         // namespace registry


}     // namespace patcher

} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_NAMESPACE_HPP__ */
