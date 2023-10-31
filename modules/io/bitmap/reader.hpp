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

#include <io/__/service/reader.hpp>
#include <io/bitmap/reader.hpp>

#include <set>

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
 * @brief Bitmap Reader
 *
 * Service to read a bitmap from various format using sight::io::bitmap library.
 *
 * @copydoc sight::io::bitmap::Reader
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is read
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::io::bitmap::reader">
        <inout key="data" uid="..." />
        <file>...</file>
        <dialog>...</dialog>
        <backends>
            <libtiff>
            <libpng>
            ...
        <backends/>
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::image]: image to read to.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to read, if it is not defined, 'open_location_dialog()' should be called to
 *           define the path.
 * - \b dialog(optional):
 *      \b description: allows to display a label in the file dialog / confirmation dialog.
 *      \b policy:
 *          - \b "never": never show the file load / extension change dialog. (DEFAULT)
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog / extension change dialog
 * - \b backends (optional): defines the backend available. If nothing is defined the default (LIBTIFF) backend is used.
 *               @see sight::io::bitmap::Reader
 *      \b enable (optional): enable group of backends.
 *          - \b "all": enable everything. GPU backends, if available have precedence over CPU ones.
 *          - \b "cpu": enable all CPU backends.
 *          - \b "gpu": enable all GPU backends.
 *      - \b libjpeg (optional): enable LibJPEG backend.
 *      - \b libtiff (optional): enable libTIFF backend.
 *      - \b libpng (optional): enable libPNG backend.
 *      - \b openjpeg (optional): enable openJPEG backend.
 *
 * If the support has been compiled in and if a CUDA capable GPU is found:
 *      - \b nvjpeg (optional): enable nvjpeg backend
 *      - \b nvjpeg2k (optional): enable nvjpeg2k backend
 */
class MODULE_IO_BITMAP_CLASS_API reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(reader, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (core::jobs::base::sptr)>;

    /// Trivial constructor / destructor
    /// @{
    MODULE_IO_BITMAP_API reader() noexcept           = default;
    MODULE_IO_BITMAP_API ~reader() noexcept override = default;
    /// @}

    /// Show a file selection dialog
    MODULE_IO_BITMAP_API void open_location_dialog() override;

protected:

    MODULE_IO_BITMAP_API sight::io::service::path_type_t get_path_type() const override;

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
    sight::io::bitmap::backend find_backend(const std::string& _extension) const;

    /// How and When display a dialog
    dialog_policy m_dialog_policy {dialog_policy::never};

    /// Signal emitted when job created.
    job_created_signal_t::sptr m_job_created_signal {new_signal<job_created_signal_t>("jobCreated")};

    /// Selected backend
    sight::io::bitmap::backend m_selected_backend {sight::io::bitmap::backend::libtiff};

    /// Enabled backends
    std::set<sight::io::bitmap::backend> m_backends {sight::io::bitmap::backend::libtiff};

    /// Used internally to avoid double dialog display
    bool m_dialog_shown {false};
};

} // namespace sight::module::io::bitmap
