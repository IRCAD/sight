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
    TUTOTRIANCONVERTERCTRL_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTOTRIANCONVERTERCTRL_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTOTRIANCONVERTERCTRL_API void start();

    /// Overrides stop method. Do nothing
    TUTOTRIANCONVERTERCTRL_API void stop() noexcept;

    TUTOTRIANCONVERTERCTRL_API void initialize();

    TUTOTRIANCONVERTERCTRL_API void uninitialize() noexcept;

private:
    ::fwData::Mesh::sptr m_mesh;
    ::fwServices::IService::sptr m_writerSrv;
    ::fwServices::IService::sptr m_readerSrv;
};

} // namespace TutoTrianConverterCtrl

#endif // __TUTOTRIANCONVERTERCTRL_PLUGIN_HPP__
