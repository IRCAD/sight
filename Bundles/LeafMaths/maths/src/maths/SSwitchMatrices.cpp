/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "maths/SSwitchMatrices.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::maths::SSwitchMatrices, ::fwData::TransformationMatrix3D);

namespace maths
{

const ::fwCom::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_SLOT    = "switchMatrix";
const ::fwCom::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_TO_SLOT = "switchToMatrix";

// ----------------------------------------------------------------------------

SSwitchMatrices::SSwitchMatrices() throw () :
    m_indexOfDesiredMatrix(0)
{
    newSlot(s_SWITCH_SLOT, &SSwitchMatrices::switchMatrix, this);
    newSlot(s_SWITCH_TO_SLOT, &SSwitchMatrices::switchToMatrix, this);

    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > matrixCfgs = m_configuration->find("matrix");

    for(ConfigurationType cfg : matrixCfgs)
    {
        TransformMatrix currentMatrix;
        currentMatrix.m_uid = cfg->getValue();

        m_matrixVector.push_back(currentMatrix);
    }
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::starting() throw (fwTools::Failed)
{
    for( TransformMatrix &currentMatrix : m_matrixVector)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(currentMatrix.m_uid);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not found.", obj);

        currentMatrix.m_matrix = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not a TransformationMatrix3D", currentMatrix.m_matrix);

        m_connections->connect(currentMatrix.m_matrix, this->getSptr(), this->getObjSrvConnections());
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::stopping() throw (fwTools::Failed)
{
    m_connections->disconnect();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::updating() throw (fwTools::Failed)
{
    ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >();

    ::fwData::TransformationMatrix3D::sptr desiredMatrix = m_matrixVector[m_indexOfDesiredMatrix].m_matrix;

    matrix->shallowCopy(desiredMatrix);

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::switchMatrix() throw (fwTools::Failed)
{
    ++m_indexOfDesiredMatrix;
    if(m_indexOfDesiredMatrix >= m_matrixVector.size())
    {
        m_indexOfDesiredMatrix = 0;
    }
    this->updating();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::switchToMatrix(size_t index) throw (fwTools::Failed)
{
    if(index < m_matrixVector.size())
    {
        m_indexOfDesiredMatrix = index;
    }
    else
    {
        SLM_WARN("Desired index don't exists, switch to first matrix");
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------


}  // namespace maths
