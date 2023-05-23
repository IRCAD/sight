/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/IContainer.hpp"

#include <array>
#include <iostream>

namespace sight::data
{

/**
 * @brief This class represents a 3D transformation matrix (4x4).
 *
 * Our convention is a row-major representation.
 */
class DATA_CLASS_API Matrix4 final : public IContainer<std::array<double,
                                                                  16> >
{
public:

    SIGHT_DECLARE_CLASS(Matrix4, IContainer<Matrix4::container_type>, factory::New<Matrix4>);

    /**
     * @brief Default constructor
     */
    DATA_API Matrix4();
    DATA_API Matrix4(std::initializer_list<value_type> init_list);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Matrix4(Object::Key key);

    //! @brief destructor
    DATA_API ~Matrix4() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using IContainer<Matrix4::container_type>::IContainer;
    using IContainer<Matrix4::container_type>::operator=;

    DATA_API Matrix4& operator=(std::initializer_list<value_type> init_list);

    /**
     * @{
     * @brief Get/Set value of the coefficient in the given position (matrix[l][c])
     */
    [[nodiscard]] constexpr Matrix4::value_type& operator()(std::size_t l, std::size_t c) noexcept;
    [[nodiscard]] constexpr const Matrix4::value_type& operator()(std::size_t l, std::size_t c) const noexcept;
    /// @}

    /// maximum size of the matrix (MATRIX_SIZE x MATRIX_SIZE)
    static const std::size_t MATRIX_SIZE = 4;

    /// Print the coefficients of the matrix
    friend std::ostream& operator<<(std::ostream& s, const Matrix4& mat)
    {
        for(std::size_t l = 0 ; l < MATRIX_SIZE ; l++)
        {
            for(std::size_t c = 0 ; c < MATRIX_SIZE ; c++)
            {
                s << mat(l, c) << "\t";
            }

            s << std::endl;
        }

        return s;
    }

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

    [[nodiscard]] static constexpr container_type identity() noexcept;

protected:

    static constexpr container_type s_IDENTITY = {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };
};

//------------------------------------------------------------------------------

constexpr Matrix4::value_type& Matrix4::operator()(std::size_t l, std::size_t c) noexcept
{
    const std::size_t pos = l * MATRIX_SIZE + c;
    return this->at(pos);
}

//------------------------------------------------------------------------------

constexpr const Matrix4::value_type& Matrix4::operator()(std::size_t l, std::size_t c) const noexcept
{
    const std::size_t pos = l * MATRIX_SIZE + c;
    return this->at(pos);
}

//-----------------------------------------------------------------------------

constexpr Matrix4::container_type Matrix4::identity() noexcept
{
    return Matrix4::s_IDENTITY;
}

} // namespace sight::data
