/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/geometry/base/SConcatenateMatrices.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry::base
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
    SIGHT_ASSERT("Config must contain one input group named 'matrix'.", inCfgs.size() == 1);

    SIGHT_ASSERT("Missing 'in group=\"matrix\"'", inCfgs[0]->getAttributeValue("group") == s_MATRIX_GROUP_INPUT);

    std::vector<ConfigurationType> matrixCfgs = inCfgs[0]->find("key");

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
    auto outputMatrix = m_output.lock();
    SIGHT_ASSERT("inout '" << s_OUTPUT << "' is not defined", outputMatrix);
    {
        sight::geometry::data::identity(*outputMatrix);

        data::Matrix4 inverse;

        std::size_t index = 0;
        for(const bool invertCurrentMatrix : m_invertVector)
        {
            auto inputMatrix = m_matrices[index++].lock();

            if(invertCurrentMatrix)
            {
                sight::geometry::data::invert(*inputMatrix, inverse);
                sight::geometry::data::multiply(*outputMatrix, inverse, *outputMatrix);
            }
            else
            {
                sight::geometry::data::multiply(*outputMatrix, *inputMatrix, *outputMatrix);
            }
        }
    }

    auto sig = outputMatrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SConcatenateMatrices::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MATRIX_GROUP_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry::base
