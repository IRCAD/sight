/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_SADDMATRICES_HPP__
#define __MATHS_SADDMATRICES_HPP__

#include "maths/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>


namespace maths
{

/**
 * @brief   This service adds two or more matrices.
 */
class MATHS_CLASS_API SAddMatrices : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SAddMatrices)(::fwServices::IController));

    /**
     * @brief Constructor.
     */
    MATHS_API SAddMatrices() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SAddMatrices() throw ()
    {
    }

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
       <service impl="::maths::SAddMatrices" type="::fwServices::IController">
           <matrix>matrix1Uid</matrix>
           <matrix inverse="true" connect="false">matrix2Uid</matrix>
           <matrix>matrix3Uid</matrix>
       </service>
       @endcode
     */
    MATHS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    MATHS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    MATHS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    void swapping() throw (fwTools::Failed)
    {
    }

    /// Does nothing.
    MATHS_API void updating() throw (fwTools::Failed);

private:

    /**
     * @brief Multiply two matrices.
     * @param[in] trfA First matrix.
     * @param[in] trfB Second matrix.
     * @param[out] _output Output matrix.
     */
    void addMatrices(::fwData::TransformationMatrix3D::sptr trfA, ::fwData::TransformationMatrix3D::sptr trfB,
                     ::fwData::TransformationMatrix3D::sptr _output);

    struct TransformMatrix
    {
        ::fwData::TransformationMatrix3D::sptr m_matrix;
        std::string m_uid;
        bool m_inverse;
        bool m_connect;
    };

    typedef std::vector< TransformMatrix > MatrixVectorType;

    MatrixVectorType m_matrixVector; ///< The vector of TransformMatrix struct

    ::fwCom::helper::SigSlotConnection m_connections; ///< connection to matrices
};

} //namespace maths

#endif  // __MATHS_SADDMATRICES_HPP__
