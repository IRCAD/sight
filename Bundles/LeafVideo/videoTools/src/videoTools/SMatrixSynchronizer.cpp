/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include "videoTools/SMatrixSynchronizer.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <extData/MatrixTL.hpp>
#include <extData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwComEd/helper/Array.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::videoTools::SMatrixSynchronizer,
                        ::fwData::TransformationMatrix3D);

// ----------------------------------------------------------------------------

namespace videoTools
{

const ::fwCom::Slots::SlotKeyType SMatrixSynchronizer::s_UPDATE_MATRIX_SLOT = "updateMatrix";

// ----------------------------------------------------------------------------

SMatrixSynchronizer::SMatrixSynchronizer() throw () : m_lastTimestamp(0)
{
    m_slotUpdateMatrix = ::fwCom::newSlot(&SMatrixSynchronizer::updateMatrix, this);

    ::fwCom::HasSlots::m_slots(s_UPDATE_MATRIX_SLOT, m_slotUpdateMatrix);



    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType TLConfig = m_configuration->findConfigurationElement("TL");
    SLM_ASSERT("element 'TL' is missing.", TLConfig);
    m_matrixUid = TLConfig->getValue();
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::starting() throw (fwTools::Failed)
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_matrixUid);
    SLM_ASSERT("Object '" + m_matrixUid + "' is not fount", obj);
    m_matrixTL = ::extData::MatrixTL::dynamicCast(obj);

}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::updateMatrix(::fwCore::HiResClock::HiResClockType timestamp)
{

    ::fwData::TransformationMatrix3D::sptr matrix3D = this->getObject< ::fwData::TransformationMatrix3D >();
    if (timestamp > m_lastTimestamp)
    {
        ::fwCore::HiResClock::HiResClockType currentTimestamp = m_matrixTL->getNewerTimestamp();
        CSPTR(::extData::MatrixTL::BufferType) buffer         = m_matrixTL->getClosestBuffer(currentTimestamp);
        OSLM_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);
        m_lastTimestamp = currentTimestamp;

        const float* values = buffer->getElement(0);

        for(unsigned int i = 0; i < 4; ++i)
        {
            for(unsigned int j = 0; j < 4; ++j)
            {
                matrix3D->setCoefficient(i,j,values[i*4+j]);
            }
        }

        auto sig = matrix3D->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

}

// ----------------------------------------------------------------------------


}  // namespace videoTools
