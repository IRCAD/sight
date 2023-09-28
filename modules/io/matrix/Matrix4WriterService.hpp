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

#include <io/__/service/writer.hpp>

#include <filesystem>

namespace sight::module::io::matrix
{

/**
 * @brief This service write a data::Matrix4 into a .trf file.
 *
 * This format is basic: an ascii file which contains matrix 4x4 values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::Matrix4WriterService">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::Matrix4]: matrix to save.
 */
class MODULE_IO_MATRIX_CLASS_API Matrix4WriterService : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(Matrix4WriterService, sight::io::service::writer);

    /// Super class of writer services
    typedef sight::io::service::writer SuperClass;

    /** @name Specified writer service methods ( override from sight::io::service::writer )
     * @{
     */

    /**
     * @brief Configure the matrix path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    MODULE_IO_MATRIX_API void openLocationDialog() override;

    /// Return path type managed by the service, here FILE
    MODULE_IO_MATRIX_API sight::io::service::IOPathType getIOPathType() const override;

    ///@}

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
     * @brief Configuring method : calls implementation from `io::writer`
     */
    MODULE_IO_MATRIX_API void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( service::base )
     *
     * This method is used to update the service.
     * The transformation matrix is written with the writer io::writer::Matrix4Writer.
     * Notify writing.
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
