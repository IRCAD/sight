/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <io/__/service/reader.hpp>

#include <filesystem>

namespace sight::module::io::matrix
{

/**
 * @brief This service read a data::matrix4 from a .trf file.
 *
 * This format is basic: an ascii file which contains matrix 4x4 values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::matrix4reader_service">
       <inout key="data" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::matrix4]: matrix to read.
 */
class matrix4_trf_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(matrix4_trf_reader, sight::io::service::reader);

    /// Super class of reader services
    using super_class = sight::io::service::reader;

    /** @name Specified reader service methods ( override from sight::io::service::reader )
     * @{
     */

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    void open_location_dialog() override;

    /**
     * @brief   returns  (filename) extension
     */
    std::vector<std::string> get_supported_extensions() override;
    /// @}

    /// Return path type managed by the service, here FILE
    sight::io::service::path_type_t get_path_type() const override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( service::base )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    void starting() override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( service::base )
     *
     * The stopping method is empty for this service.
     */
    void stopping() override;

    /**
     * @brief Configuring method : calls implementation from `io::reader`
     */
    void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( service::base )
     *
     * This method is used to update the service.
     * The transformation matrix is read with the reader io::reader::matrix4_reader.
     * Notify reading.
     */
    void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    void info(std::ostream& _sstream) override;
    /// @}
};

} // namespace sight::module::io::matrix
