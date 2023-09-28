/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
 * @defgroup requirement Requirements
 * The modules in the \p \<requirement\> section of the \b plugin.xml are started when the module is started.
 *
 * In Sight, we can automatically start modules in two case:
 * - The module contains an AppConfig or an Activity: the module will be started when the config or the activity is
 *   launched
 * - The module contains services: the module will be started when one of the services is used in an AppConfig to
 * launch.
 *
 * But in some case, we must define the modules to start manually using the \p \<requirement\> tag.
 *
 * <b>Which module should I add in requirement ?</b>
 *
 * You must add requirement for:
 *  - modules containing data (ex. dataReg)
 *  - modules containing registry (ex. servicesReg, @ref activities)
 *  - modules for @ref preferences, guiQt, visuVTKQt, @ref memory
 *  - ...
 */

/**
 * @defgroup apprequirement Application requirements
 * @ingroup  requirement
 *  This list contains the modules that could be added in your application's requirements.
 */

/**
 * @defgroup modulerequirement modules requirements
 * @ingroup  requirement
 * This list contains the modules that could be added in module's requirements
 */

/**
 * @brief  The namespace sight::core::runtime contains classes to manage module, configuration element, extension point
 * in the
 *         runtime environment from a configuration file .
 */
#pragma once

namespace sight::core::runtime
{

}
