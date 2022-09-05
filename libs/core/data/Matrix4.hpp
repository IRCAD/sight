/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/tools/compare.hpp"

#include "data/factory/new.hpp"
#include "data/Object.hpp"

#include <array>
#include <iostream>

namespace sight::data
{

/**
 * @brief This class represents a 3D transformation matrix (4x4).
 */
class DATA_CLASS_API Matrix4 : public Object
{
public:

    SIGHT_DECLARE_CLASS(Matrix4, Object, factory::New<Matrix4>);

    typedef double TM3DType;
    typedef std::array<TM3DType, 16> TMCoefArray;
    typedef std::array<std::array<TM3DType, 4>, 4> MatrixType;

    /**
     * @brief Default constructor
     */
    DATA_API Matrix4();

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Matrix4(Object::Key key);

    //! @brief destructor
    DATA_API ~Matrix4() override;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Getters/setters
    TMCoefArray& getCoefficients();
    const TMCoefArray& getCoefficients() const;
    void setCoefficients(const TMCoefArray& _vCoefficients);

    /**
     * @{
     * @brief Get/Set value of the coefficient in the given position (matrix[l][c])
     */
    DATA_API TM3DType getCoefficient(std::size_t l, std::size_t c) const;
    DATA_API void setCoefficient(std::size_t l, std::size_t c, TM3DType val);
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
                s << mat.getCoefficient(l, c) << "\t";
            }

            s << std::endl;
        }

        return s;
    }

    /**
     * @brief Returns matrix coefficients as a 4x4 matrix (Row major).
     * @return 4x4 matrix (std::array< std::array< double, 4> 4>).
     */
    DATA_API MatrixType getMatrix4x4() const;

    /**
     * @brief Sets coefficients as a 4x4 matrix (Row major).
     * @param _matrix : matrix coefficients as std::array< std::array< double, 4 > 4 >.
     */
    DATA_API void setMatrix4x4(const MatrixType& _matrix);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Matrix4& other) const noexcept;
    DATA_API bool operator!=(const Matrix4& other) const noexcept;
    /// @}

protected:

    //! Matrix coefficient number (4x4). m_vCoefficients[0] to m_vCoefficients[3] is the first row of the matrix
    TMCoefArray m_vCoefficients {};

    static constexpr TMCoefArray s_IDENTITY = {1., 0., 0., 0.,
                                               0., 1., 0., 0.,
                                               0., 0., 1., 0.,
                                               0., 0., 0., 1.
    };
};

//-----------------------------------------------------------------------------

inline Matrix4::TMCoefArray& Matrix4::getCoefficients()
{
    return this->m_vCoefficients;
}

//-----------------------------------------------------------------------------

inline const Matrix4::TMCoefArray& Matrix4::getCoefficients() const
{
    return this->m_vCoefficients;
}

//-----------------------------------------------------------------------------

inline void Matrix4::setCoefficients(const Matrix4::TMCoefArray& _vCoefficients)
{
    this->m_vCoefficients = _vCoefficients;
}

//------------------------------------------------------------------------------

inline Matrix4::TM3DType Matrix4::getCoefficient(std::size_t l, std::size_t c) const
{
    std::size_t pos = l * MATRIX_SIZE + c;
    return m_vCoefficients.at(pos);
}

//------------------------------------------------------------------------------

inline void Matrix4::setCoefficient(std::size_t l, std::size_t c, Matrix4::TM3DType val)
{
    std::size_t pos = l * MATRIX_SIZE + c;
    m_vCoefficients.at(pos) = val;
}

//------------------------------------------------------------------------------

inline Matrix4::MatrixType Matrix4::getMatrix4x4() const
{
    // linear index to 2d.
    const Matrix4::MatrixType matrix4x4 {{
        {m_vCoefficients[0], m_vCoefficients[1], m_vCoefficients[2], m_vCoefficients[3]},
        {m_vCoefficients[4], m_vCoefficients[5], m_vCoefficients[6], m_vCoefficients[7]},
        {m_vCoefficients[8], m_vCoefficients[9], m_vCoefficients[10], m_vCoefficients[11]},
        {m_vCoefficients[12], m_vCoefficients[13], m_vCoefficients[14], m_vCoefficients[15]}
    }
    };
    return matrix4x4;
}

//-----------------------------------------------------------------------------

inline void Matrix4::setMatrix4x4(const Matrix4::MatrixType& _matrix)
{
    // 2d to linear index.
    m_vCoefficients = {
        _matrix[0][0], _matrix[0][1], _matrix[0][2], _matrix[0][3],
        _matrix[1][0], _matrix[1][1], _matrix[1][2], _matrix[1][3],
        _matrix[2][0], _matrix[2][1], _matrix[2][2], _matrix[2][3],
        _matrix[3][0], _matrix[3][1], _matrix[3][2], _matrix[3][3]
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::data
