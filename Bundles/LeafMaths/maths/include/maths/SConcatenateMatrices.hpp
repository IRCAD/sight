/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_SCONCATENATEMATRICES_HPP__
#define __MATHS_SCONCATENATEMATRICES_HPP__

#include "maths/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>


namespace maths
{
/**
 * @brief   This service concatenates N matrices together.
 *
 * The computation of the concatenation is triggered when updating the service.
 * Auto-connections to the matrices can also be used to force the service to recompute the output matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::maths::SConcatenateMatrices">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." inverse="true"/>
                <key uid="..." autoConnect="yes"/>
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: List of matrix keys to concatenate. For each input matrix, it is
 * possible to invert it before multiplying with it by specifying \b inverse="true".
 * The autoConnect is connected to the update slot, thus is will trigger a new concatenation.
 * @subsection In-Out In-Out:
 * - \b output [::fwData::TransformationMatrix3D]: Output matrix.
 */

class MATHS_CLASS_API SConcatenateMatrices : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SConcatenateMatrices)(::fwServices::IController));

    /**
     * @brief Constructor.
     */
    MATHS_API SConcatenateMatrices() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SConcatenateMatrices() throw ()
    {
    }

protected:
    /// This method is used to configure the service.
    MATHS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    MATHS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    MATHS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    MATHS_API void updating() throw (fwTools::Failed);

private:

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

#endif  // __MATHS_SCONCATENATEMATRICES_HPP__
