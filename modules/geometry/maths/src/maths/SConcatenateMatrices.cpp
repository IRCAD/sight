/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "maths/SConcatenateMatrices.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/tools/TransformationMatrix3D.hpp>

#include <services/macros.hpp>

fwServicesRegisterMacro( ::sight::services::IController, ::maths::SConcatenateMatrices, data::TransformationMatrix3D)

static const services::IService::KeyType s_MATRIX_GROUP_INOUT = "matrix";
static const services::IService::KeyType s_OUTPUT = "output";

namespace maths
{

// ----------------------------------------------------------------------------

SConcatenateMatrices::SConcatenateMatrices() noexcept
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::configuring()
{
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    std::vector<ConfigurationType> inCfgs = m_configuration->find("in");
    SLM_ASSERT("Config must contain one input group named 'matrix'.", inCfgs.size() == 1);

    SLM_ASSERT("Missing 'in group=\"matrix\"'", inCfgs[0]->getAttributeValue("group") == s_MATRIX_GROUP_INOUT);

    std::vector<ConfigurationType> matrixCfgs = inCfgs[0]->find("key");

    for (ConfigurationType cfg : matrixCfgs)
    {
        bool invertCurrentMatrix  = false;
        const std::string inverse = cfg->getAttributeValue("inverse");
        if (!inverse.empty())
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
    auto outputMatrix = this->getInOut< data::TransformationMatrix3D>(s_OUTPUT);
    SLM_ASSERT("inout '" + s_OUTPUT + "' is not defined", outputMatrix);
    {
        data::mt::ObjectWriteLock outputMatrixLock(outputMatrix);

        data::tools::TransformationMatrix3D::identity(outputMatrix);

        auto inverse = data::TransformationMatrix3D::New();

        size_t index = 0;
        for (const bool invertCurrentMatrix : m_invertVector)
        {
            auto inputMatrix = this->getInput< data::TransformationMatrix3D>(s_MATRIX_GROUP_INOUT, index++);
            data::mt::ObjectReadLock inputMatrixLock(inputMatrix);

            if (invertCurrentMatrix)
            {
                data::tools::TransformationMatrix3D::invert(inputMatrix, inverse);
                data::tools::TransformationMatrix3D::multiply(outputMatrix, inverse, outputMatrix);
            }
            else
            {
                data::tools::TransformationMatrix3D::multiply(outputMatrix, inputMatrix, outputMatrix);
            }
        }
    }

    auto sig = outputMatrix->signal< data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SConcatenateMatrices::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_MATRIX_GROUP_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// ----------------------------------------------------------------------------

} // namespace maths
