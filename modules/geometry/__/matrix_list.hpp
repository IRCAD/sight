/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/geometry/__/config.hpp"

#include <data/matrix4.hpp>
#include <data/vector.hpp>

#include <service/base.hpp>

namespace sight::module::geometry
{

/**
 * @brief This service manages vectors of matrices and can be connected to the matrices list visualization service (
 * for example list_view on Sight)
 *
 * This service allows to insert or remove matrices from a list into vector group.
 * When an input matrix is updated, this matrix is pushed into the outputVector.
 * When the service receive the "selectMatrix" slot, the selected matrix is updated.
 * The number of elements in each group (matrices, selectedMatrix, vector) must be the same.
 * If we have 2 elements in selectedMatrix group, the 2 selected matrices are updated in the same time.
 * The same with adding or removing a matrix from the vector.
 *
 * @section Signals Signals
 * - \b matrixAdded(int index, std::string text): Emitted when a matrix is inserted on a vector
 * - \b matrixRemoved(int index): Emitted when a matrix is removed from the list
 *
 * @section Slots Slots
 * - \b selectMatrix(int index, std::string text): Called when a matrix is selected
 * - \b removeMatrix(int index): Called when a matrix is removed from the list
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::matrix_list">
            <inout group="matrices">
                <key uid="..." />
                <key uid="..." />
                ...
             </inout>
             <inout group="selectedMatrix">
                 <key uid="..." />
                 <key uid="..." />
                 ...
             </inout>
             <out group="vector">
                 <key uid="..." />
                 <key uid="..." />
                 ...
             </out>
       </service>
   @endcode
 *
 * @subsection InOut InOut:
 * - \b matrices [sight::data::matrix4]: input matrix
 * - \b selectedMatrix [sight::data::matrix4]: selected matrix
 *
 * @subsection Output Output:
 * - \b vector [sight::data::vector]: vector containing matrices
 */
class MODULE_GEOMETRY_CLASS_API matrix_list final : public service::base
{
public:

    /// Generates default methods like New, dynamicCast,...
    SIGHT_DECLARE_SERVICE(matrix_list, service::base);

    /// Initializes signals.
    MODULE_GEOMETRY_API matrix_list() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_API ~matrix_list() noexcept override;

protected:

    /// Gets parameters from composite.
    MODULE_GEOMETRY_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_API void stopping() override;

    /// Stores computed matrices from input vector into a list of strings.
    MODULE_GEOMETRY_API void updating() override;

    /// This method is used to configure the service parameters.
    MODULE_GEOMETRY_API void configuring() override;

private:

    /// SLOT: selects a matrix.
    void selectMatrix(int /*index*/);

    ///SLOT: removes a matrix.
    void removeMatrix(int /*_index*/);

    typedef core::com::signal<void (int, std::string)> matrix_added_signal_t;
    typedef core::com::signal<void (int)> matrix_removed_signal_t;

    static constexpr std::string_view s_MATRICES_INOUT = "matrices";
    static constexpr std::string_view s_SELECTED_INOUT = "selectedMatrix";
    static constexpr std::string_view s_VECTOR_INOUT   = "vector";

    data::ptr_vector<data::matrix4, data::Access::inout> m_inputVector {this, s_MATRICES_INOUT};
    data::ptr_vector<data::matrix4, data::Access::inout> m_selectedVector {this, s_SELECTED_INOUT};
    data::ptr_vector<data::vector, data::Access::out> m_outputVector {this, s_VECTOR_INOUT};
};

} // namespace sight::module::geometry.
