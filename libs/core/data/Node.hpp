/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"
#include "data/Port.hpp"

#include <vector>

namespace sight::data
{

/**
 * @brief This class defines a node.
 * A node is represented by input and output ports.
 */
class DATA_CLASS_API Node : public Object
{
public:

    SIGHT_DECLARE_CLASS(Node, Object, factory::New<Node>);

    /// Port container
    typedef std::vector<Port::sptr> PortContainer;

    /// Type of 'updated' signal
    typedef core::com::Signal<void ()> UpdatedSignalType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Node(Object::Key key);

    /// Destructor
    DATA_API ~Node() override;

    /// Add an input port
    DATA_API void addInputPort(const Port::sptr& port);

    /// Add an output port
    DATA_API void addOutputPort(const Port::sptr& port);

    /// Get the container of input ports
    DATA_API PortContainer& getInputPorts();
    DATA_API const PortContainer& getInputPorts() const;

    /// Get the container of output ports
    DATA_API PortContainer& getOutputPorts();
    DATA_API const PortContainer& getOutputPorts() const;

    /// Set object to the node
    DATA_API void setObject(const Object::sptr& object);

    /// Get node object
    DATA_API Object::sptr getObject() const;

    /**
     * @brief Get the port with given identifier
     *
     * @param[in] _identifier port identifier
     * @param[in] _modeInput if true find input port, else find output port
     *
     * @return input or output port with given identifier
     */
    DATA_API Port::sptr findPort(const std::string& _identifier, bool _modeInput) const;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Updated signal key
    DATA_API static const core::com::Signals::SignalKeyType s_UPDATED_SIG;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Node& other) const noexcept;
    DATA_API bool operator!=(const Node& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// node object
    Object::sptr m_object;

    //! Input port container
    PortContainer m_inputs;

    //! Output port container
    PortContainer m_outputs;

    /// Updated signal
    UpdatedSignalType::sptr m_sigUpdated;
};

} // namespace sight::data
