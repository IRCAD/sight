/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_NAMESPACE_HPP__
#define __GUIQT_NAMESPACE_HPP__

/**
 * @ingroup apprequirement
 * @brief The namespace guiQt contains the basic services to build the application IHM with Qt.
 *
 * This bundle is automatically started at the launch of the application if it is present in the REQUIREMENTS of the
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
    bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoBasicConfig)
   @endcode
 */
namespace guiQt
{

}
#endif /* __GUIQT_NAMESPACE_HPP__ */
