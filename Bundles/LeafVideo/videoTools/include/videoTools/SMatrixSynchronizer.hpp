/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__
#define __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__


#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>
#include <arData/MatrixTL.hpp>

#include <fwCore/base.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwData/TransformationMatrix3D.hpp>



namespace videoTools
{
/**
 * @brief   SMatrixSynchronizer reads the content of a MatrixTL into a TransformationMatrix3D.
 */
class VIDEOTOOLS_CLASS_API SMatrixSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixSynchronizer)(::arServices::ISynchronizer));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SMatrixSynchronizer() noexcept
    {
    }

    /// Slot definition
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_MATRIX_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> UpdateMatrixSlotType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
       <service impl="::videoTools::SMatrixSynchronizer" type="::arServices::ISynchronizer">
            <inout>Matrix</inout>
            <in>MatrixTL</in>
       </service>
     * @endcode
     *
     * @subsection In-Out In-Out
     * - \b TL [::arData::MatrixTL]:  Defines the uid of the MatrixTL containing the matrices.
     *
     */

    VIDEOTOOLS_API void configuring();

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting();

    /// Does nothing.
    VIDEOTOOLS_API void stopping();

    /// Does nothing.
    VIDEOTOOLS_API void updating()
    {
    }

private:

    /// slot to update the TransformationMatrix3D with the timeline buffer
    UpdateMatrixSlotType::sptr m_slotUpdateMatrix;
    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// This function fills the TransformationMatrix3D with the current buffer content of the MatrixTL
    void updateMatrix(::fwCore::HiResClock::HiResClockType timestamp);


};

} //namespace videoTools
#endif  // __VIDEOTOOLS_SMATRIXSYNCHRONIZER_HPP__
