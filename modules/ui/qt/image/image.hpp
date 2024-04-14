/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <ui/__/editor.hpp>

#include <QObject>

#include <filesystem>

namespace sight::module::ui::qt::image
{

/**
 * @brief   This editor displays an image.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::image::image" >
        <path>...</path>
        <width>...</width>
        <height>...</height>
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b path: path of the image.
 * - \b width (optional): width of the image in pixels.
 * - \b height (optional): height of the image in pixels.
 */
class image : public QObject,
              public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(image, sight::ui::editor);

    /// Constructor. Do nothing.
    image() noexcept;

    /// Destructor. Do nothing.
    ~image() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */
    /**
     * @brief This method launches the editor::starting method.
     */
    void starting() override;

    /**
     * @brief This method launches the editor::stopping method.
     */
    void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    void updating() override;

    /// This method is used to configure the class parameters.
    void configuring() override;

/** @} */

private:

    std::filesystem::path m_path; /// Path of the image
    int m_width {-1};             /// Width of the image (pixels)
    int m_height {-1};            /// Height of the image (pixels)
};

} // namespace sight::module::ui::qt::image
