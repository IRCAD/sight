/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/jobs/IJob.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/bitmap/Writer.hpp>

// cspell:ignore nvjpeg

namespace sight::data
{

class Image;

} // namespace sight::data

namespace sight::core::jobs
{

class IJob;

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
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is written
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::io::bitmap::SWriter">
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
 * - \b data [sight::data::Image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'openLocationDialog()' should be called to
 *           define the path.
 * - \b dialog(optional):
 *      \b description: allows to display a label in the file dialog / confirmation dialog.
 *      \b policy:
 *          - \b "never": never show the file save / extension change dialog
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog / extension change dialog
 *          - \b "default": default behavior, which is "never"
 * - \b backends (optional): defines the backend available. If nothing is defined the "default" backend is used.
 *               @see sight::io::bitmap::Writer
 *      \b enable (optional): enable group of backends.
 *          - \b "all": enable everything. GPU backends, if available have precedence over CPU ones.
 *          - \b "cpu": enable all CPU backends.
 *          - \b "gpu": enable all GPU backends.
 *      \b mode (optional): set the mode, which defines the compression/speed ratio
 *          - \b "best": emphasis compression over speed
 *          - \b "fast": emphasis speed over compression
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
class MODULE_IO_BITMAP_CLASS_API SWriter final : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(SWriter, sight::io::base::service::IWriter);

    using JobCreatedSignal = core::com::Signal<void (core::jobs::IJob::sptr)>;

    /// Trivial constructor / destructor
    /// @{
    MODULE_IO_BITMAP_API SWriter() noexcept           = default;
    MODULE_IO_BITMAP_API ~SWriter() noexcept override = default;
    /// @}

    /// Show a file selection dialog
    MODULE_IO_BITMAP_API void openLocationDialog() override;

protected:

    MODULE_IO_BITMAP_API sight::io::base::service::IOPathType getIOPathType() const override;

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
    sight::io::bitmap::Writer::Backend findBackend(const std::string& extension) const;

    /// How and When display a dialog
    DialogPolicy m_dialog_policy {DialogPolicy::NEVER};

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal {newSignal<JobCreatedSignal>("jobCreated")};

    /// Selected backend
    sight::io::bitmap::Writer::Backend m_selected_backend {sight::io::bitmap::Writer::Backend::DEFAULT};

    /// Enabled backends
    std::map<sight::io::bitmap::Writer::Backend, sight::io::bitmap::Writer::Mode> m_mode_by_backend {
        {sight::io::bitmap::Writer::Backend::DEFAULT, sight::io::bitmap::Writer::Mode::DEFAULT}
    };

    /// Used internally to avoid double dialog display
    bool m_dialog_shown {false};
};

} // namespace sight::module::io::bitmap
