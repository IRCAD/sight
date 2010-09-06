/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_GRAPHMSG_HPP_
#define _FWCOMED_GRAPHMSG_HPP_

#include <fwData/Node.hpp>

#include <fwServices/ObjectMsg.hpp>


#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for Graph : store modification information
 * @class   GraphMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API GraphMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((GraphMsg)(::fwServices::ObjectMsg), ( () ), new GraphMsg );

    /**
     * @name Event identifier
     * @{
     */
    /// @brief Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_GRAPH;
    FWCOMED_API static std::string ADD_NODE;
    FWCOMED_API static std::string REMOVE_NODE;
    FWCOMED_API static std::string ADD_EDGE;
    FWCOMED_API static std::string REMOVE_EDGE;
    FWCOMED_API static std::string SELECTED_NODE;
    FWCOMED_API static std::string UNSELECTED_NODE;
    FWCOMED_API static std::string SELECTED_OPERATOR;
    FWCOMED_API static std::string EXECUTE_GRAPH;
    FWCOMED_API static std::string CHANGED_NODE_STATE;
    /// @}

    /**
      * @brief  Constructor : does nothing.
      */
    FWCOMED_API GraphMsg() throw();

    /**
      * @brief  Destuctor : does nothing.
      */
    FWCOMED_API virtual ~GraphMsg() throw();

    /// Add a ADD_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void addedNode( ::fwData::Node::sptr node );
    /// Return the added node contained in the dataInfo of ADD_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getAddedNode() const;

    /// Add a REMOVE_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void removedNode( ::fwData::Node::sptr node );
    /// Return the removed node contained in the dataInfo of REMOVE_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getRemovedNode() const;

    /// Add a SELECTED_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void selectedNode( ::fwData::Node::sptr node );
    /// Return the selected node contained in the dataInfo of SELECTED_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getSelectedNode() const;

    /// Add a UNSELECTED_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void unselectedNode( ::fwData::Node::sptr node );
    /// Return the unselected node contained in the dataInfo of UNSELECTED_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getUnselectedNode() const;

};

} // namespace fwComEd

#endif //_FWCOMED_GRAPHMSG_HPP_

