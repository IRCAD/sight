/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MATHS_SSWITCHMATRICES_HPP__
#define __MATHS_SSWITCHMATRICES_HPP__

#include "maths/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/helper/SigSlotConnection.hpp>
#include <fwServices/IController.hpp>


namespace maths
{

/**
 * @brief   This service concatenates the matrices.
 * @class   SConcatenateMatrices
 */
class MATHS_CLASS_API SSwitchMatrices : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SSwitchMatrices)(::fwServices::IController));

    /**
     * @brief Constructor.
     */
    MATHS_API SSwitchMatrices() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SSwitchMatrices() throw ()
    {
    }

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_SLOT;
    typedef ::fwCom::Slot< void () > SwitchSlotType;

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_TO_SLOT;
    typedef ::fwCom::Slot<void (int)> SwitchToSlotType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @verbatim
       <service impl="::maths::SConcatenateMatrices" type="::fwServices::IController">
           <matrix>matrix1Uid</matrix>
           <matrix>matrix2Uid</matrix>
           <default>1</default>
       </service>
       @endverbatim
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

    ///Switch to next Matrix
    MATHS_API void switchMatrix() throw (fwTools::Failed);

    ///Switch to Matrix "index"
    MATHS_API void switchToMatrix(int index) throw (fwTools::Failed);



private:

    struct TransformMatrix
    {
        ::fwData::TransformationMatrix3D::sptr m_matrix;
        std::string m_uid;
    };

    ::fwData::TransformationMatrix3D::sptr m_OuputMatrix;

    typedef std::vector< TransformMatrix > MatrixVectorType;

    MatrixVectorType m_matrixVector; ///< The vector of TransformMatrix struct

    size_t m_indexOfDesiredMatrix;

    ::fwServices::helper::SigSlotConnection::sptr m_connections; ///< connection to matrices
};

} //namespace maths

#endif  // __MATHS_SSWITCHMATRICES_HPP__
