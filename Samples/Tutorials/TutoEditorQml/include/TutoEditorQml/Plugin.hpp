/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "TutoEditorQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace TutoEditorQml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTOEDITORQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTOEDITORQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTOEDITORQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTOEDITORQML_API void start();

    /// Overrides stop method. Do nothing
    TUTOEDITORQML_API void stop() noexcept;

    TUTOEDITORQML_API void initialize();

    TUTOEDITORQML_API void uninitialize() noexcept;
};

} // namespace TutoEditorQml
