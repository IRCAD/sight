/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_NODE_HPP_
#define _FWDATA_NODE_HPP_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/Port.hpp"
#include "fwData/Factory.hpp"

namespace fwData
{
/**
 * @class   Node
 * @brief   This class defines a node
 *
 * A node is represented by input and output ports.
 *
 * @see     ::fwData::Port
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Node : public ::fwData::Object
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Node)(::fwData::Object), (()), ::fwData::Factory::New< Node >) ;

    /// Port container
    typedef std::vector< ::fwData::Port::sptr > PortContainer;

    /// Add an input port
    FWDATA_API void addInputPort(::fwData::Port::sptr port);

    /// Add an output port
    FWDATA_API void addOutputPort(::fwData::Port::sptr port);

    /// Get the container of input ports
    FWDATA_API PortContainer & getInputPorts();

    /// Get the container of output ports
    FWDATA_API PortContainer & getOutputPorts();

    /// Set object to the node
    FWDATA_API void setObject( ::fwData::Object::sptr object );

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
    FWDATA_API Port::sptr findPort(const std::string &_identifier, /*const std::string &type,*/ bool _modeInput) const;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Node::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Node::csptr _source );

protected :

    /// constructor
    FWDATA_API Node();

    /// Destructor
    FWDATA_API virtual ~Node();

    /// node object
    ::fwData::Object::sptr m_object;

    //! Input port container
    PortContainer m_inputs;

    //! Output port container
    PortContainer m_outputs;
};

} // namespace fwData

#endif // _FWDATA_NODE_HPP_
