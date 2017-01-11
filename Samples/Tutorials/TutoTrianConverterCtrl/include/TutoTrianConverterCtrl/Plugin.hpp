/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTOTRIANCONVERTERCTRL_PLUGIN_HPP__
#define __TUTOTRIANCONVERTERCTRL_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include <fwData/Mesh.hpp>
#include <fwServices/IService.hpp>

#include "TutoTrianConverterCtrl/config.hpp"

namespace TutoTrianConverterCtrl
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTOTRIANCONVERTERCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTOTRIANCONVERTERCTRL_API Plugin() throw();

    /// Destructor. Do nothing.
    TUTOTRIANCONVERTERCTRL_API ~Plugin() throw();

    /// Overrides start method. .
    TUTOTRIANCONVERTERCTRL_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides stop method. Do nothing
    TUTOTRIANCONVERTERCTRL_API void stop() throw();

    TUTOTRIANCONVERTERCTRL_API void initialize() throw( ::fwRuntime::RuntimeException );

    TUTOTRIANCONVERTERCTRL_API void uninitialize() throw();

private:
    ::fwData::Mesh::sptr m_mesh;
    ::fwServices::IService::sptr m_writerSrv;
    ::fwServices::IService::sptr m_readerSrv;
};

} // namespace TutoTrianConverterCtrl

#endif // __TUTOTRIANCONVERTERCTRL_PLUGIN_HPP__
