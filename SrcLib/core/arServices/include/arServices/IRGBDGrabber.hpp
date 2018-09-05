/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "arServices/config.hpp"
#include <arServices/IGrabber.hpp>

namespace arServices
{
/**
 * @brief   This interface defines the RGBD grabber service API.
 *          Must be implemented for services that grabs RGBD camera frames.
 */
class ARSERVICES_CLASS_API IRGBDGrabber : public ::arServices::IGrabber
{

public:

    fwCoreServiceClassDefinitionsMacro((IRGBDGrabber)(::arServices::IGrabber));

    /**
     * @name Data API
     * @{
     */
    ARSERVICES_API static const ::fwServices::IService::KeyType s_DEPTHTL_INOUT;
    /** @} */

    /// Constructor.
    ARSERVICES_API IRGBDGrabber() noexcept;

    /// Destructor.
    ARSERVICES_API virtual ~IRGBDGrabber() noexcept;
};

} //namespace arServices
