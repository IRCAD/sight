/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#pragma once

#include "modules/io/bitmap/config.hpp"

#include <core/com/signal.hpp>
#include <core/jobs/base.hpp>

#include <io/__/service/writer.hpp>
#include <io/bitmap/Writer.hpp>

// cspell:ignore nvjpeg

namespace sight::data
{

class image;

} // namespace sight::data

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::io::bitmap
{

/**
 * @brief Bitmap Writer
 *
 * Service to write a bitmap to various format using sight::io::bitmap library.
 *
 * @copydoc sight::io::bitmap::Writer
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::io::bitmap::writer">
        <in key="data" uid="..." />
        <file>...</file>
        <dialog>...</dialog>
        <backends mode="fast">
            <libtiff mode="best">
            <libpng mode="fast">
            ...
        <backends/>
    </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'openLocationDialog()' should be called to
 *           define the path.
 * - \b dialog(optional):
 *      \b description: allows to display a label in the file dialog / confirmation dialog.
 *      \b policy:
 *          - \b "never": never show the file save / extension change dialog. (DEFAULT)
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog / extension change dialog
 * - \b backends (optional): defines the backend available. If nothing is defined the default (LIBTIFF) backend is used.
 *               @see sight::io::bitmap::Writer
 *      \b enable (optional): enable group of backends.
 *          - \b "all": enable everything. GPU backends, if available have precedence over CPU ones.
 *          - \b "cpu": enable all CPU backends.
 *          - \b "gpu": enable all GPU backends.
 *      \b mode (optional): set the mode, which defines the compression/speed ratio
 *          - \b "best": emphasis compression over speed
 *          - \b "fast": emphasis speed over compression (DEFAULT)
 *      - \b libjpeg (optional): enable LibJPEG backend.
 *          \b mode (optional)
 *      - \b libtiff (optional): enable libTIFF backend.
 *          \b mode (optional)
 *      - \b libpng (optional): enable libPNG backend.
 *          \b mode (optional)
 *      - \b openjpeg (optional): enable openJPEG backend.
 *          \b mode (optional)
 *
 * If the support has been compiled in and if a CUDA capable GPU is found:
 *      - \b nvjpeg (optional): enable nvjpeg backend
 *          \b mode (optional)
 *      - \b nvjpeg2k (optional): enable nvjpeg2k backend
 *          \b mode (optional)
 */
class MODULE_IO_BITMAP_CLASS_API writer final : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(writer, sight::io::service::writer);

    using JobCreatedSignal = core::com::signal<void (core::jobs::base::sptr)>;

    /// Trivial constructor / destructor
    /// @{
    MODULE_IO_BITMAP_API writer() noexcept           = default;
    MODULE_IO_BITMAP_API ~writer() noexcept override = default;
    /// @}

    /// Show a file selection dialog
    MODULE_IO_BITMAP_API void openLocationDialog() override;

protected:

    MODULE_IO_BITMAP_API sight::io::service::IOPathType getIOPathType() const override;

    /// Does nothing
    MODULE_IO_BITMAP_API void starting() override;

    /// Does nothing
    MODULE_IO_BITMAP_API void stopping() override;

    /// Parses the configuration
    MODULE_IO_BITMAP_API void configuring() override;

    /// Write the image
    MODULE_IO_BITMAP_API void updating() override;

private:

    /// Retrieve the backend in the enabled backend list from the given extension
    /// @param extension the extension of the file to write
    /// @return the found backend
    sight::io::bitmap::Backend findBackend(const std::string& extension) const;

    /// How and When display a dialog
    DialogPolicy m_dialog_policy {DialogPolicy::NEVER};

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal {new_signal<JobCreatedSignal>("jobCreated")};

    /// Selected backend
    sight::io::bitmap::Backend m_selected_backend {sight::io::bitmap::Backend::LIBTIFF};

    /// Enabled backends
    std::map<sight::io::bitmap::Backend, sight::io::bitmap::Writer::Mode> m_mode_by_backend {
        {sight::io::bitmap::Backend::LIBTIFF, sight::io::bitmap::Writer::Mode::FAST}
    };

    /// Used internally to avoid double dialog display
    bool m_dialog_shown {false};
};

} // namespace sight::module::io::bitmap
