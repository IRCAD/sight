/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _TUTO02DATASERVICEBASICCTRL_PLUGIN_HPP_
#define _TUTO02DATASERVICEBASICCTRL_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include <fwData/Image.hpp>
#include <fwServices/IService.hpp>

#include "tuto02DataServiceBasicCtrl/config.hpp"

namespace tuto02DataServiceBasicCtrl
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class TUTO02DATASERVICEBASICCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO02DATASERVICEBASICCTRL_API Plugin() throw();

    /// Destructor. Do nothing.
    TUTO02DATASERVICEBASICCTRL_API ~Plugin() throw();

    /// Overrides start method. .
    TUTO02DATASERVICEBASICCTRL_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides stop method. Do nothing
    TUTO02DATASERVICEBASICCTRL_API void stop() throw();

    TUTO02DATASERVICEBASICCTRL_API void initialize() throw( ::fwRuntime::RuntimeException );

    TUTO02DATASERVICEBASICCTRL_API void uninitialize() throw();

private:
    ::fwData::Image::sptr m_image;
    ::fwServices::IService::sptr m_comChannel;
    ::fwServices::IService::sptr m_frameSrv;
    ::fwServices::IService::sptr m_renderSrv;
    ::fwServices::IService::sptr m_readerSrv;
};

} // namespace tuto02DataServiceBasicCtrl

#endif // _TUTO02DATASERVICEBASICCTRL_PLUGIN_HPP_
