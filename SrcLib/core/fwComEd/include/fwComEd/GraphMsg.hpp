/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_GRAPHMSG_HPP_
#define _FWCOMED_GRAPHMSG_HPP_

#include "boost/tuple/tuple.hpp"

#include <fwData/Node.hpp>
#include <fwData/Edge.hpp>

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
    FWCOMED_API static std::string CLEANING_GRAPH;
    FWCOMED_API static std::string ADD_NODE;
    FWCOMED_API static std::string REMOVE_NODE; // msg notified AFTER removing a node
    FWCOMED_API static std::string REMOVING_NODE;  // msg notified TO REMOVE a node
    FWCOMED_API static std::string ADD_EDGE; // msg notified AFTER adding an edge
    FWCOMED_API static std::string ADDING_EDGE; // msg notified TO ADD an edge
    FWCOMED_API static std::string REMOVE_EDGE;
    FWCOMED_API static std::string SELECTED_NODE;
    FWCOMED_API static std::string UNSELECTED_NODE;
    FWCOMED_API static std::string SELECTED_OPERATOR;
    FWCOMED_API static std::string EXECUTE_GRAPH;
    FWCOMED_API static std::string CANCEL_EXECUTE_GRAPH;
    FWCOMED_API static std::string CHANGED_NODE_STATE;
    FWCOMED_API static std::string NODE_PROCESS_ERROR; // datainfo = node to process
    FWCOMED_API static std::string NODE_PROCESS_WARN;  // datainfo = node to process
    FWCOMED_API static std::string NODE_PROCESS_DONE;  // datainfo = node to process
    FWCOMED_API static std::string NODE_PROCESS;       // datainfo = node to process
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
    FWCOMED_API virtual void addedNode( ::fwData::Node::csptr node );
    /// Return the added node contained in the dataInfo of ADD_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getAddedNode() const;

    /// Add a REMOVE_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void removedNode( ::fwData::Node::csptr node );
    /// Return the removed node contained in the dataInfo of REMOVE_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getRemovedNode() const;

    /// Add a REMOVING_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void removingNode( ::fwData::Node::csptr node );
    /// Return the removed node contained in the dataInfo of REMOVING_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getRemovingNode() const;

    /// Add a ADDING_EDGE event with a dataInfo containing the node.
    FWCOMED_API virtual void addingEdge( ::fwData::Node::csptr nodeFrom, ::fwData::Node::csptr nodeTo, std::string outputPortId, std::string inputPortId );
    /// Return the removed node contained in the dataInfo of ADDING_EDGE event
    FWCOMED_API virtual ::boost::tuple< ::fwData::Node::csptr, ::fwData::Node::csptr, std::string, std::string > getAddingEdge() const;


    /// Add a SELECTED_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void selectedNode( ::fwData::Node::csptr node );
    /// Return the selected node contained in the dataInfo of SELECTED_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getSelectedNode() const;

    /// Add a UNSELECTED_NODE event with a dataInfo containing the node.
    FWCOMED_API virtual void unselectedNode( ::fwData::Node::csptr node );
    /// Return the unselected node contained in the dataInfo of UNSELECTED_NODE event
    FWCOMED_API virtual ::fwData::Node::csptr getUnselectedNode() const;

};

} // namespace fwComEd

#endif //_FWCOMED_GRAPHMSG_HPP_

