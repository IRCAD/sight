/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/dicom_series.hpp>
#include <data/model_series.hpp>
#include <data/object.hpp>
#include <data/series.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/writer/series.hpp>

#include <filesystem>

namespace sight::data
{

class series_set;

} // namespace sight::data

namespace sight::module::io::dicom
{

/**
 * @brief DICOM 3D Writer (ModelSeries)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::io::dicom::surface_segmentation_writer" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::model_series]: ModelSeries to save in Dicom.
 */
class surface_segmentation_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(surface_segmentation_writer, sight::io::service::writer);

    /**
     * @brief Constructor
     */
    surface_segmentation_writer() noexcept;

    /**
     * @brief Destructor
     */
    ~surface_segmentation_writer() noexcept override;

    /// Propose a directory selection where to save the DICOM files.
    void open_location_dialog() override;

protected:

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Configuring method. This method is used to configure the service.
    void configuring() override;

    /// Write the ImageSeries in DICOM format.
    void updating() override;

    /// Return path type managed by the service, here FOLDER
    sight::io::service::path_type_t get_path_type() const override;

private:

    /**
     * @brief Write a Series to a folder in DICOM format
     * @param[in] _filename DICOM file name
     * @param[in] _model ModelSeries that must be written
     */
    void save_surface_segmentation(
        const std::filesystem::path _filename,
        const data::model_series::csptr& _model
    );
};

} // namespace sight::module::io::dicom
