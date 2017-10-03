/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_NODE_HPP__
#define __FWDATA_NODE_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/Port.hpp"

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Node), FWDATA_API);

namespace fwData
{

/**
 * @brief This class defines a node.
 * A node is represented by input and output ports.
 */
class FWDATA_CLASS_API Node : public ::fwData::Object
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Node)(::fwData::Object), (()), ::fwData::factory::New< Node >);
    fwCampMakeFriendDataMacro((fwData)(Node));

    /// Port container
    typedef std::vector< ::fwData::Port::sptr > PortContainer;

    /// Type of 'updated' signal
    typedef ::fwCom::Signal< void () > UpdatedSignalType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Node(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Node();

    /// Add an input port
    FWDATA_API void addInputPort(const ::fwData::Port::sptr& port);

    /// Add an output port
    FWDATA_API void addOutputPort(const ::fwData::Port::sptr& port);

    /// Get the container of input ports
    FWDATA_API PortContainer& getInputPorts();

    /// Get the container of output ports
    FWDATA_API PortContainer& getOutputPorts();

    /// Set object to the node
    FWDATA_API void setObject(const ::fwData::Object::sptr& object );

    /// Get node object
    FWDATA_API ::fwData::Object::sptr getObject() const;

    /**
     * @brief Get the port with given identifier
     *
     * @param[in] _identifier port identifier
     * @param[in] _modeInput if true find input port, else find output port
     *
     * @return input or output port with given identifier
     */
    FWDATA_API Port::sptr findPort(const std::string& _identifier, bool _modeInput) const;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Updated signal key
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_UPDATED_SIG;

protected:

    /// node object
    ::fwData::Object::sptr m_object;

    //! Input port container
    PortContainer m_inputs;

    //! Output port container
    PortContainer m_outputs;

    /// Updated signal
    UpdatedSignalType::sptr m_sigUpdated;
};

} // namespace fwData

#endif // __FWDATA_NODE_HPP__

