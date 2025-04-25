/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <data/image.hpp>
#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <ui/__/editor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::image
{

/**
 * @brief   image_info service allows to display image pixel information when it receives the mouse cursor coordinates.
 *
 * @warning The interaction must be received in pixel coordinates (for example from the the adaptor
 *         ::visuVTKAdaptor::imagePickerInteractor)
 *
 * @section Slots Slots
 * - \b get_interaction(data::tools::picking_info): display image pixel information
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="image_info" type="sight::module::ui::qt::image::image_info" auto_connect="true">
        <in key="image" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b image [sight::data::image]: image used to retrieve the pixel value.
 */
class image_info : public QObject,
                   public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(image_info, sight::ui::editor);

    image_info() noexcept;

    ~image_info() noexcept override;

protected:

    /// Starts editor.
    void starting() override;

    /// Stops editor.
    void stopping() override;

    /// Check if the image is valid, if not the editor is disabled
    void updating() override;

    /// Initializes the editor
    void configuring() override;

    void info(std::ostream& _sstream) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    /// Slot: get the picking information to display the pixel value
    void get_interaction(data::tools::picking_info _info);

    /// Label to display the pixel value
    QPointer<QLineEdit> m_value_text;

    static constexpr std::string_view IMAGE = "image";

    data::ptr<data::image, data::access::in> m_image {this, IMAGE};
};

} // namespace sight::module::ui::qt::image
