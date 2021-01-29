/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/io/file/config.hpp"

#include <io/base/services/IReader.hpp>

#include <filesystem>

namespace sight::modules::io::file
{

/**
 * @brief This service read a data::TransformationMatrix3D from a .trf file.
 *
 * This format is basic: an ascii file which contains matrix 4x4 values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::io::file::TransformationMatrix3DReaderService">
       <inout key="data" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [data::TransformationMatrix3D]: matrix to read.
 */
class MODULE_IO_FILE_CLASS_API TransformationMatrix3DReaderService : public sight::io::base::services::IReader
{

public:
    fwCoreServiceMacro(TransformationMatrix3DReaderService, sight::io::base::services::IReader)

    /// Super class of reader services
    typedef sight::io::base::services::IReader SuperClass;

    /** @name Specified reader service methods ( override from sight::io::base::services::IReader )
     * @{
     */

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_FILE_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_FILE_API virtual void openLocationDialog() override;

    /**
     * @brief   returns  (filename) extension
     */
    MODULE_IO_FILE_API virtual std::vector< std::string > getSupportedExtensions() override;
    /// @}

    /// Return path type managed by the service, here FILE
    MODULE_IO_FILE_API virtual sight::io::base::services::IOPathType getIOPathType() const override;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    /**
     * @brief Starting method. This method is called by start() from base service ( services::IService )
     *
     * This method is used to initialize the service. The starting method is empty for this service.
     */
    MODULE_IO_FILE_API virtual void starting( ) override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( services::IService )
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_FILE_API virtual void stopping( ) override;

    /**
     * @brief Configuring method : calls implementation from `io::IReader`
     */
    MODULE_IO_FILE_API virtual void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( services::IService )
     *
     * This method is used to update the service.
     * The transformation matrix is read with the reader io::base::reader::TransformationMatrix3DReader.
     * Notify reading.
     */
    MODULE_IO_FILE_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     *
     * @param[out] _sstream output stream
     */
    MODULE_IO_FILE_API virtual void info(std::ostream& _sstream ) override;
    /// @}

};

} // namespace sight::modules::io::file
