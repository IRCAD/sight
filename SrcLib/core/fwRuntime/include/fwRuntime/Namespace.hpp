/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_NAMESPACE_HPP__
#define __FWRUNTIME_NAMESPACE_HPP__

/**
 * @defgroup requirement Requirements
 * The bundles in the \p \<requirement\> section of the \b plugin.xml are started when the bundle is started.
 *
 * In fw4spl, we can automatically start bundles in two case:
 * - The bundle contains an AppConfig or an Activity: the bundle will be started when the config or the activity is
 *   launched
 * - The bundle contains services: the bundle will be started when one of the services is used in an AppConfig to launch.
 *
 * But in some case, we must define the bundles to start manually using the \p \<requirement\> tag.
 *
 * <b>Which bundle should I add in requirement ?</b>
 *
 * You must add requirement for:
 *  - bundles containing data (ex. dataReg)
 *  - bundles containing registry (ex. servicesReg, @ref activities)
 *  - bundles for @ref preferences, guiQt, visuVTKQt, @ref memory
 *  - ...
 */

/**
 * @defgroup apprequirement Application requirements
 * @ingroup  requirement
 *  This list contains the bundles that could be added in your application's requirements.
 */

/**
 * @defgroup bundlerequirement Bundles requirements
 * @ingroup  requirement
 * This list contains the bundles that could be added in bundle's requirements
 */

/**
 * @brief  The namespace fwRuntime contains classes to manage bundle, configuration element, extension point in the
 *         runtime environment from a configuration file .
 */
namespace fwRuntime
{
}
#endif /* __FWRUNTIME_NAMESPACE_HPP__ */
