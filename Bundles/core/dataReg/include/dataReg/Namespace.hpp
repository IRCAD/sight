/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    <plugin id="Tuto01Basic" version="@DASH_VERSION@">

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
                            <icon>@BUNDLE_PREFIX@/Tuto01Basic_0-1/tuto.ico</icon>
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
