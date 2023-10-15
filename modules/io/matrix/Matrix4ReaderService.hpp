/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/io/matrix/config.hpp"

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
   <service type="sight::module::io::matrix::Matrix4ReaderService">
       <inout key="data" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::matrix4]: matrix to read.
 */
class MODULE_IO_MATRIX_CLASS_API Matrix4ReaderService : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(Matrix4ReaderService, sight::io::service::reader);

    /// Super class of reader services
    typedef sight::io::service::reader SuperClass;

    /** @name Specified reader service methods ( override from sight::io::service::reader )
     * @{
     */

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_MATRIX_API void openLocationDialog() override;

    /**
     * @brief   returns  (filename) extension
     */
    MODULE_IO_MATRIX_API std::vector<std::string> getSupportedExtensions() override;
    /// @}

    /// Return path type managed by the service, here FILE
    MODULE_IO_MATRIX_API sight::io::service::IOPathType getIOPathType() const override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( service::base )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    MODULE_IO_MATRIX_API void starting() override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( service::base )
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_MATRIX_API void stopping() override;

    /**
     * @brief Configuring method : calls implementation from `io::reader`
     */
    MODULE_IO_MATRIX_API void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( service::base )
     *
     * This method is used to update the service.
     * The transformation matrix is read with the reader io::reader::matrix4_reader.
     * Notify reading.
     */
    MODULE_IO_MATRIX_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    MODULE_IO_MATRIX_API void info(std::ostream& _sstream) override;
    /// @}
};

} // namespace sight::module::io::matrix
