/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "modules/io/document/config.hpp"

#include <data/image.hpp>

#include <io/__/service/writer.hpp>

#include <ui/qt/container/widget.hpp>

#include <QPdfWriter>
#include <QtWidgets>

namespace sight::module::io::document
{

/**
 *
 * @brief Creates and writes a PDF containing images.
 *
 * The service will take IDs at the configuration. Those IDs can either be UIDs of data::image or
 * either SID/WID of Qt containers. Converts the images if needed, in order to writes them in the PDF.
 * A scaling is applied to fit the A4 format of the PDF pages.
 *
 * @warning When the visuVTKAdaptor::Snapshot service fills an data::image, the renderer must not be hidden.
 * For example if there are multiple tabs of renderer, only the image corresponding to the selected tab will be
 * correctly saved in the PDF.
 *
 * @section XML XML configuration
 * @code{.xml}
    <service uid="..." type="sight::module::io::document::pdf_writer">
        <in group="image">
            <key uid="..."/>
            <key uid="..."/>
        </in>
        <container uid="..."/>
    </service>
 *
 * @endcode
 * @subsection Input Input:
 * - \b image [sight::data::image] : Defines the UID of the data::image to write.
 * - \b container(optional) : Defines the SID or the WID of the container to write.
 */

class MODULE_IO_DOCUMENT_CLASS_API pdf_writer : public sight::io::service::writer
{
public:

    using images_scaled_list_t = std::vector<QImage>;
    using images_list_t        = std::vector<data::image::sptr>;
    using images_i_ds_t        = std::vector<std::string>;

    using containers_list_t = std::vector<QWidget*>;
    using containers_i_ds_t = std::vector<std::string>;

    SIGHT_DECLARE_SERVICE(pdf_writer, sight::io::service::writer);
    /**
     * @brief Constructor : does nothing
     */
    MODULE_IO_DOCUMENT_API pdf_writer() = default;

    /**
     * @brief Destructor
     */
    MODULE_IO_DOCUMENT_API ~pdf_writer() noexcept override = default;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief Starting method : Fills the list of Qt containers as well as the list of images got from
     * the configuration.
     *
     * This method is used to initialize the service.
     */
    MODULE_IO_DOCUMENT_API void starting() override;

    /**
     * @brief Stopping method : default does nothing.
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_DOCUMENT_API void stopping() override;

    /**
     * @brief Configure service. This method is called by configure() from base service ( service::base )
     *
     * Gets the images or containers from their IDs.
     *
     */
    MODULE_IO_DOCUMENT_API void configuring() override;

    /**
     * @brief Configure the image path.
     *
     * This method is used to set the file path of the PDF to write.
     *
     */
    MODULE_IO_DOCUMENT_API void open_location_dialog() override;

    /**
     * @brief Updating method. Creates a new PDF.
     *
     * Loops over the containers list and the images list. Creates a new A4 page for each,
     * converts the containers and the images to QImage and writes them into the newly created PDF.
     */
    MODULE_IO_DOCUMENT_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * information about the service.
     *
     * @param[out] _sstream output stream
     */
    MODULE_IO_DOCUMENT_API void info(std::ostream& _sstream) override;
    /// @}

    /// Returns managed path type, here service manages only single file
    MODULE_IO_DOCUMENT_API sight::io::service::path_type_t get_path_type() const override;

private:

    /**
     * @brief convertFwImageToQImage: converts a data::image to a QImage
     * @param data::image to convert
     * @return converted QImage
     */
    static QImage convert_fw_image_to_q_image(const data::image& _image);

    /**
     * @brief List of images IDs to export into the PDF.
     * Filled at the configuring from the XML configuration, and used at starting().
     */
    images_i_ds_t m_images_ui_ds;

    /**
     * @brief List of containers IDs to export into the PDF.
     * Filled at configuring from the XML configuration, and used at starting().
     */
    containers_i_ds_t m_containers_i_ds;

    /**
     * @brief List of containers to export into the PDF.
     */
    containers_list_t m_containers_to_export;

    static constexpr std::string_view IMAGE_INPUT = "image";
    data::ptr_vector<data::image, sight::data::access::in> m_images {this, IMAGE_INPUT};
};

} // namespace sight::module::io::document
