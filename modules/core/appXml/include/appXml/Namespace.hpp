/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
 * @ingroup apprequirement
 * @brief  The module appXml allows to parse xml configuration of an application.
 * See @ref AppConfig for more details.
 *
 * This module is automatically started at the launch of the application if it is present in the REQUIREMENTS of the
 * application's Properties.cmake.
 *
 * Example
 * @code{cmake}
    set( NAME Tuto01Basic )
    set( VERSION 0.1 )
    set( TYPE APP )
    set( DEPENDENCIES  )
    set( REQUIREMENTS
        dataReg
        servicesReg
        guiQt # it will be automatically started when the application launches
        fwlauncher
        appXml # it will be automatically started when the application launches
    )
    moduleParam(appXml PARAM_LIST config PARAM_VALUES tutoBasicConfig)
   @endcode
 */
#pragma once

namespace sight::modules::appXml
{

}
