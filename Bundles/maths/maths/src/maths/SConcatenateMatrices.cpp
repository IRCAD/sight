/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "maths/SConcatenateMatrices.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::maths::SConcatenateMatrices, ::fwData::TransformationMatrix3D);

static const ::fwServices::IService::KeyType s_MATRIX_GROUP_INOUT = "matrix";
static const ::fwServices::IService::KeyType s_OUTPUT             = "output";

namespace maths
{

// ----------------------------------------------------------------------------

SConcatenateMatrices::SConcatenateMatrices() noexcept
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > inCfgs = m_configuration->find("in");
    SLM_ASSERT("Config must contain one input group named 'matrix'.", inCfgs.size() == 1);

    SLM_ASSERT("Missing 'in group=\"matrix\"'", inCfgs[0]->getAttributeValue("group") == s_MATRIX_GROUP_INOUT);

    std::vector< ConfigurationType > matrixCfgs = inCfgs[0]->find("key");

    for(ConfigurationType cfg : matrixCfgs)
    {
        bool invertCurrentMatrix  = false;
        const std::string inverse = cfg->getAttributeValue("inverse");
        if(!inverse.empty())
        {
            invertCurrentMatrix = (inverse == "true");
        }
        m_invertVector.push_back(invertCurrentMatrix);
    }
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::starting()
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::stopping()
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::updating()
{
    auto outputMatrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_OUTPUT);
    SLM_ASSERT("inout '" + s_OUTPUT + "' is not defined", outputMatrix);
    {
        ::fwData::mt::ObjectWriteLock outputMatrixLock(outputMatrix);

        ::fwDataTools::TransformationMatrix3D::identity(outputMatrix);

        auto inverse = ::fwData::TransformationMatrix3D::New();

        size_t index = 0;
        for( const bool invertCurrentMatrix : m_invertVector)
        {
            auto inputMatrix = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_GROUP_INOUT, index++);
            ::fwData::mt::ObjectReadLock inputMatrixLock(inputMatrix);

            if( invertCurrentMatrix )
            {
                ::fwDataTools::TransformationMatrix3D::invert(inputMatrix, inverse);
                ::fwDataTools::TransformationMatrix3D::multiply(outputMatrix, inverse, outputMatrix);
            }
            else
            {
                ::fwDataTools::TransformationMatrix3D::multiply(outputMatrix, inputMatrix, outputMatrix);
            }
        }
    }

    auto sig = outputMatrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SConcatenateMatrices::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_MATRIX_GROUP_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// ----------------------------------------------------------------------------

}  // namespace maths
