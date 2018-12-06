/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __DATAREG_NAMESPACE_HPP__
#define __DATAREG_NAMESPACE_HPP__

/**
 * @ingroup apprequirement
 * @brief  The namespace dataReg (data registration) contains classes which allow to parse the xml configuration
 *         of objects .
 *
 * It also is used to load fwData and fwMedData library. So, if you want to use any objects in your application
 * described in XML, you require this bundle: add the requirement in your application plugin.xml.
 *
 * @code{xml}
    <requirement id="dataReg" />
   @endcode
 *
 *  Example of plugin.xml:
 *
 * @code{xml}
    <plugin id="Tuto01Basic" version="@PROJECT_VERSION@">

        <!-- The bundles in requirements are automatically started when this Application is launched. -->
        <requirement id="dataReg" />
        <requirement id="servicesReg" />

        <!-- Defines the App-config -->
        <extension implements="::fwServices::registry::AppConfig">
            <id>tutoBasicConfig</id><!-- identifier of the configuration -->
            <config>

                <!-- Frame service -->
                <service uid="myFrame" type="::gui::frame::SDefaultFrame">
                    <gui>
                        <frame>
                            <name>tutoBasicApplicationName</name>
                            <icon>Tuto01Basic-0.1/tuto.ico</icon>
                            <minSize width="800" height="600" />
                        </frame>
                    </gui>
                </service>

                <start uid="myFrame" /><!-- start the frame service -->

            </config>
        </extension>
    </plugin>
   @endcode
 */
namespace dataReg
{

}
#endif /* __DATAREG_NAMESPACE_HPP__ */
