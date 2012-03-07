/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRANSFORMATIONMATRIX3D_HPP_
#define _FWDATA_TRANSFORMATIONMATRIX3D_HPP_

#include <vector>
#include <iostream>
#include <assert.h>

#include "fwData/Object.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{

/**
 * @class   TransformationMatrix3D
 * @brief   This class represents a 3D transformation matrix (4x4)
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API TransformationMatrix3D : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (TransformationMatrix3D)(::fwData::Object),
            (()), ::fwData::Factory::New< TransformationMatrix3D >) ;

    typedef double TM3DType;
    typedef std::vector<TM3DType> TMCoefArray;

    //duplication methods
    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( TransformationMatrix3D::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( TransformationMatrix3D::csptr _source );


    fwGettersSettersDocMacro(Coefficients, vCoefficients, TMCoefArray, the elements of the matrix)

    /**
     * @{
     * @brief Get/Set value of the coefficient in the given position (matrix[l][c])
     */
    FWDATA_API double getCoefficient(int l, int c);
    FWDATA_API void setCoefficient(int l, int c, TM3DType val);
    // @}

    /// maximum size of the matrix (MATRIX_SIZE x MATRIX_SIZE)
    static const int MATRIX_SIZE = 4;

    /// Print the coefficients of the matrix
    friend std::ostream& operator<<(std::ostream& s, TransformationMatrix3D mat)
    {
        for(int l = 0; l < MATRIX_SIZE; l++)
        {
            for(int c = 0; c < MATRIX_SIZE; c++)
            {
                s << mat.getCoefficient(l, c)<< "\t";
            }
            s << std::endl;
        }
        return s;
    }

private :

    //! @brief constructor
    FWDATA_API TransformationMatrix3D();

    //! @brief destructor
    FWDATA_API virtual ~TransformationMatrix3D();

    //! Matrix coefficient number (4x4). m_vCoefficients[0] to m_vCoefficients[3] is the first row of the matrix
    TMCoefArray m_vCoefficients;
};

}; // namespace fwData

#endif // _FWDATA_TRANSFORMATIONMATRIX3D_HPP_
