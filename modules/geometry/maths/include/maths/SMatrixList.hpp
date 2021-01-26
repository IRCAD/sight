/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "maths/config.hpp"

#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <services/IService.hpp>

namespace maths
{

/**
 * @brief This service manages vectors of matrices and can be connected to the matrices list visualization service (
 * for example SListView on Sight)
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
        <service type="::maths::SMatrixList">
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
 * - \b matrices [data::TransformationMatrix3D]: input matrix
 * - \b selectedMatrix [data::TransformationMatrix3D]: selected matrix
 *
 * @subsection Output Output:
 * - \b vector [data::Vector]: vector containing matrices
 */
class MATHS_CLASS_API SMatrixList final : public services::IService
{

public:

    /// Generates default methods like New, dynamicCast,...
    fwCoreServiceMacro(SMatrixList, services::IService)

    /// Initializes signals.
    MATHS_API SMatrixList() noexcept;

    /// Destroys the service.
    MATHS_API ~SMatrixList() noexcept override;

protected:

    /// Gets parameters from composite.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Stores computed matrices from input vector into a list of strings.
    MATHS_API void updating() override;

    /// This method is used to configure the service parameters.
    MATHS_API void configuring() override;

private:

    /// SLOT: selects a matrix.
    void selectMatrix(int);

    ///SLOT: removes a matrix.
    void removeMatrix(int);

    /// Vector containing input matrices.
    std::vector< data::TransformationMatrix3D::sptr > m_inputVector;

    /// Vector containing output vector.
    std::vector< data::Vector::sptr > m_outputVector;

    /// Vector containing selected vector.
    std::vector< data::TransformationMatrix3D::sptr > m_selectedVector;

    typedef core::com::Signal< void (int, std::string) > MatrixAddedSignalType;
    typedef core::com::Signal< void (int) > MatrixRemovedSignalType;
};
} // namespace maths.