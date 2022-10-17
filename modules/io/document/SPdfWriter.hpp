/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include <data/Image.hpp>

#include <io/base/service/IWriter.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QPdfWriter>
#include <QtWidgets>

namespace sight::module::io::document
{

/**
 *
 * @brief Creates and writes a PDF containing images.
 *
 * The service will take IDs at the configuration. Those IDs can either be UIDs of data::Image or
 * either SID/WID of Qt containers. Converts the images if needed, in order to writes them in the PDF.
 * A scaling is applied to fit the A4 format of the PDF pages.
 *
 * @warning When the visuVTKAdaptor::Snapshot service fills an data::Image, the renderer must not be hidden.
 * For example if there are multiple tabs of renderer, only the image corresponding to the selected tab will be
 * correctly saved in the PDF.
 *
 * @section XML XML configuration
 * @code{.xml}
    <service uid="..." type="sight::module::io::document::SPdfWriter">
        <in group="image">
            <key uid="..."/>
            <key uid="..."/>
        </in>
        <container uid="..."/>
    </service>
 *
 * @endcode
 * @subsection Input Input:
 * - \b image [sight::data::Image] : Defines the UID of the data::Image to write.
 * - \b container(optional) : Defines the SID or the WID of the container to write.
 */

class MODULE_IO_DOCUMENT_CLASS_API SPdfWriter : public sight::io::base::service::IWriter
{
public:

    typedef std::vector<QImage> ImagesScaledListType;
    typedef std::vector<data::Image::sptr> ImagesListType;
    typedef std::vector<std::string> ImagesIDsType;

    typedef std::vector<QWidget*> ContainersListType;
    typedef std::vector<std::string> ContainersIDsType;

    SIGHT_DECLARE_SERVICE(SPdfWriter, sight::io::base::service::IWriter);
    /**
     * @brief Constructor : does nothing
     */
    MODULE_IO_DOCUMENT_API SPdfWriter();

    /**
     * @brief Destructor
     */
    MODULE_IO_DOCUMENT_API ~SPdfWriter() noexcept override;

protected:

    /** @name Service methods ( override from service::IService )
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
     * @brief Configure service. This method is called by configure() from base service ( service::IService )
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
    MODULE_IO_DOCUMENT_API void openLocationDialog() override;

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
    MODULE_IO_DOCUMENT_API sight::io::base::service::IOPathType getIOPathType() const override;

private:

    /**
     * @brief convertFwImageToQImage: converts a data::Image to a QImage
     * @param data::Image to convert
     * @return converted QImage
     */
    static QImage convertFwImageToQImage(const data::Image& image);

    /**
     * @brief List of images IDs to export into the PDF.
     * Filled at the configuring from the XML configuration, and used at starting().
     */
    ImagesIDsType m_imagesUIDs;

    /**
     * @brief List of containers IDs to export into the PDF.
     * Filled at configuring from the XML configuration, and used at starting().
     */
    ContainersIDsType m_containersIDs;

    /**
     * @brief List of containers to export into the PDF.
     */
    ContainersListType m_containersToExport;

    static constexpr std::string_view s_IMAGE_INPUT = "image";
    data::ptr_vector<data::Image, sight::data::Access::in> m_images {this, s_IMAGE_INPUT};
};

} // namespace sight::module::io::document
