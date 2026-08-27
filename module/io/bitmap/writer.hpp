/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <io/__/service/writer.hpp>
#include <io/bitmap/writer.hpp>

// cspell:ignore nvjpeg

namespace sight::data
{

class image;

} // namespace sight::data
namespace sight::module::io::bitmap
{

/**
 * @brief Bitmap Writer
 *
 * Service to write a bitmap to various format using sight::io::bitmap library.
 *
 * @copydoc sight::io::bitmap::writer
 *
 * @section Signals Signals
 * - \b notification_created(core::notification::base::sptr): emitted to display a progress bar while the
 * image is
 * written
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::io::bitmap::writer">
        <in key="data" uid="..." />
        <file>...</file>
        <dialog>...</dialog>
        <gpu_required>true|false</gpu_required>
        <mode>best|fast</mode>
    </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it is not defined, 'open_location_dialog()' should be called to
 *           define the path.
 * - \b dialog(optional):
 *      \b description: allows to display a label in the file dialog / confirmation dialog.
 *      \b policy:
 *          - \b "never": never show the file save / extension change dialog. (DEFAULT)
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog / extension change dialog
 * - \b gpu_required (optional): returns an error if GPU support is not enabled.
 *   \b mode (optional): set the mode, which defines the compression/speed ratio
 *       - \b "best": emphasis compression over speed
 *       - \b "fast": emphasis speed over compression (DEFAULT)
 */
class writer final : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(writer, sight::io::service::writer);

    /// Trivial constructor / destructor
    /// @{
    writer() noexcept = default;
    ~writer() noexcept final = default;
    /// @}

    /// Show a file selection dialog
    void open_location_dialog() final;

    sight::io::service::path_type_t get_path_type() const final;

protected:

    /// Does nothing
    void starting() final;

    /// Does nothing
    void stopping() final;

    /// Parses the configuration
    void configuring() final;

    /// Write the image
    void updating() final;

private:

    /// Retrieve the backend in the enabled backend list from the given extension
    /// @param _extension the extension of the file to write
    /// @return the found backend
    sight::io::bitmap::backend find_backend(const std::string& _extension) const;

    /// How and When display a dialog
    dialog_policy m_dialog_policy {dialog_policy::never};

    /// Selected backend
    sight::io::bitmap::backend m_selected_backend {sight::io::bitmap::backend::libtiff};

    /// Enabled backends
    std::map<sight::io::bitmap::backend, sight::io::bitmap::writer::mode> m_mode_by_backend {
        {sight::io::bitmap::backend::libtiff, sight::io::bitmap::writer::mode::fast}
    };

    /// Used internally to avoid double dialog display
    bool m_dialog_shown {false};
};

} // namespace sight::module::io::bitmap
