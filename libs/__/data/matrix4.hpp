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

#include "data/container.hpp"

#include <array>
#include <iostream>

namespace sight::data
{

/**
 * @brief This class represents a 3D transformation matrix (4x4).
 *
 * Our convention is a row-major representation.
 */
class DATA_CLASS_API matrix4 final : public data::container<std::array<double,
                                                                       16> >
{
public:

    SIGHT_DECLARE_CLASS(matrix4, container<matrix4::container_t>);

    /**
     * @brief Default constructor
     */
    DATA_API matrix4();
    DATA_API matrix4(std::initializer_list<value_type> _init_list);

    //! @brief destructor
    DATA_API ~matrix4() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using container<matrix4::container_t>::container;
    using container<matrix4::container_t>::operator=;

    DATA_API matrix4& operator=(std::initializer_list<value_type> _init_list);

    /**
     * @{
     * @brief Get/Set value of the coefficient in the given position (matrix[l][c])
     */
    [[nodiscard]] constexpr matrix4::value_type& operator()(std::size_t _l, std::size_t _c) noexcept;
    [[nodiscard]] constexpr const matrix4::value_type& operator()(std::size_t _l, std::size_t _c) const noexcept;
    /// @}

    /// maximum size of the matrix (MATRIX_SIZE x MATRIX_SIZE)
    static const std::size_t MATRIX_SIZE = 4;

    /// Print the coefficients of the matrix
    friend std::ostream& operator<<(std::ostream& _s, const matrix4& _mat)
    {
        for(std::size_t l = 0 ; l < MATRIX_SIZE ; l++)
        {
            for(std::size_t c = 0 ; c < MATRIX_SIZE ; c++)
            {
                _s << _mat(l, c) << "\t";
            }

            _s << std::endl;
        }

        return _s;
    }

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

    [[nodiscard]] static constexpr container_t identity() noexcept;

protected:

    static constexpr container_t IDENTITY = {
        1., 0., 0., 0.,
        0., 1., 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    };
};

//------------------------------------------------------------------------------

constexpr matrix4::value_type& matrix4::operator()(std::size_t _l, std::size_t _c) noexcept
{
    const std::size_t pos = _l * MATRIX_SIZE + _c;
    return this->at(pos);
}

//------------------------------------------------------------------------------

constexpr const matrix4::value_type& matrix4::operator()(std::size_t _l, std::size_t _c) const noexcept
{
    const std::size_t pos = _l * MATRIX_SIZE + _c;
    return this->at(pos);
}

//-----------------------------------------------------------------------------

constexpr matrix4::container_t matrix4::identity() noexcept
{
    return matrix4::IDENTITY;
}

} // namespace sight::data
