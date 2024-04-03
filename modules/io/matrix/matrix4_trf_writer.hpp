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

#include <io/__/service/writer.hpp>

#include <filesystem>

namespace sight::module::io::matrix
{

/**
 * @brief This service write a data::matrix4 into a .trf file.
 *
 * This format is basic: an ascii file which contains matrix 4x4 values.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::matrix4writer_service">
       <in key="data" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::matrix4]: matrix to save.
 */
class matrix4_trf_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(matrix4_trf_writer, sight::io::service::writer);

    /// Super class of writer services
    using super_class = sight::io::service::writer;

    /** @name Specified writer service methods ( override from sight::io::service::writer )
     * @{
     */

    /**
     * @brief Configure the matrix path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    void open_location_dialog() override;

    /// Return path type managed by the service, here FILE
    sight::io::service::path_type_t get_path_type() const override;

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
    void starting() override;

    /**
     * @brief Stopping method. This method is called by stop() from base service ( service::base )
     *
     * The stopping method is empty for this service.
     */
    void stopping() override;

    /**
     * @brief Configuring method : calls implementation from `io::writer`
     */
    void configuring() override;

    /**
     * @brief Updating method. This method is called by update() from base service ( service::base )
     *
     * This method is used to update the service.
     * The transformation matrix is written with the writer io::writer::matrix4_writer.
     * Notify writing.
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
