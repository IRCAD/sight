/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include "fwComEd/GraphMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::GraphMsg, ::fwComEd::GraphMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string GraphMsg::NEW_GRAPH            = "NEW_GRAPH";
std::string GraphMsg::CLEANING_GRAPH       = "CLEANING_GRAPH";
std::string GraphMsg::ADD_NODE             = "ADD_NODE";
std::string GraphMsg::REMOVE_NODE          = "REMOVE_NODE";
std::string GraphMsg::REMOVING_NODE        = "REMOVING_NODE";
std::string GraphMsg::ADD_EDGE             = "ADD_EDGE";
std::string GraphMsg::ADDING_EDGE          = "ADDING_EDGE";
std::string GraphMsg::REMOVE_EDGE          = "REMOVE_EDGE";
std::string GraphMsg::SELECTED_NODE        = "SELECTED_NODE";
std::string GraphMsg::UNSELECTED_NODE      = "UNSELECTED_NODE";
std::string GraphMsg::SELECTED_OPERATOR    = "SELECTED_OPERATOR";
std::string GraphMsg::EXECUTE_GRAPH        = "EXECUTE_GRAPH";
std::string GraphMsg::CANCEL_EXECUTE_GRAPH = "CANCEL_EXECUTE_GRAPH";
std::string GraphMsg::CHANGED_NODE_STATE   = "CHANGED_NODE_STATE";
std::string GraphMsg::NODE_PROCESS_ERROR   = "NODE_PROCESS_ERROR";
std::string GraphMsg::NODE_PROCESS_WARN    = "NODE_PROCESS_WARN";
std::string GraphMsg::NODE_PROCESS_DONE    = "NODE_PROCESS_DONE";
std::string GraphMsg::NODE_PROCESS         = "NODE_PROCESS";

//-----------------------------------------------------------------------------

GraphMsg::GraphMsg() throw()
{}

//-----------------------------------------------------------------------------

GraphMsg::~GraphMsg() throw()
{}

//-----------------------------------------------------------------------------


void GraphMsg::addedNode( ::fwData::Node::csptr node )
{
    addEvent( GraphMsg::ADD_NODE , node );
}

//-----------------------------------------------------------------------------

::fwData::Node::csptr GraphMsg::getAddedNode() const
{
    return ::fwData::Node::dynamicConstCast( getDataInfo( GraphMsg::ADD_NODE ) );
}

//-----------------------------------------------------------------------------

void GraphMsg::removedNode( ::fwData::Node::csptr node )
{
    addEvent( GraphMsg::REMOVE_NODE , node );
}

//-----------------------------------------------------------------------------

::fwData::Node::csptr GraphMsg::getRemovedNode() const
{
    return ::fwData::Node::dynamicConstCast( getDataInfo( GraphMsg::REMOVE_NODE ) );
}

//-----------------------------------------------------------------------------

void GraphMsg::removingNode( ::fwData::Node::csptr node )
{
    addEvent( GraphMsg::REMOVING_NODE , node );
}

//-----------------------------------------------------------------------------

::fwData::Node::csptr GraphMsg::getRemovingNode() const
{
    return ::fwData::Node::dynamicConstCast( getDataInfo( GraphMsg::REMOVING_NODE ) );
}

//-----------------------------------------------------------------------------

void GraphMsg::addingEdge( ::fwData::Node::csptr nodeFrom, ::fwData::Node::csptr nodeTo, std::string outputPortId, std::string inputPortId )
{
    ::fwData::Composite::NewSptr edgeInfo;
    (*edgeInfo)["nodeFrom"] = ::fwData::Node::constCast( nodeFrom );
    (*edgeInfo)["nodeTo"] = ::fwData::Node::constCast( nodeTo );
    (*edgeInfo)["outputPortId"] = ::fwData::String::New( outputPortId );
    (*edgeInfo)["inputPortId"] = ::fwData::String::New( inputPortId );
    addEvent( GraphMsg::ADDING_EDGE , edgeInfo );
}

//-----------------------------------------------------------------------------

::boost::tuple< ::fwData::Node::csptr, ::fwData::Node::csptr, std::string, std::string > GraphMsg::getAddingEdge() const
{
    ::fwData::Composite::csptr edgeInfoConst = ::fwData::Composite::dynamicConstCast( getDataInfo( GraphMsg::ADDING_EDGE ) );
    ::fwData::Composite::sptr edgeInfo = ::fwData::Composite::constCast( edgeInfoConst );

    ::fwData::Node::csptr nodeFrom = ::fwData::Node::dynamicConstCast( (*edgeInfo)["nodeFrom"] );
    ::fwData::Node::csptr nodeTo = ::fwData::Node::dynamicConstCast( (*edgeInfo)["nodeTo"] );
    std::string outputPortId = ::fwData::String::dynamicConstCast( (*edgeInfo)["outputPortId"] )->value();
    std::string inputPortId = ::fwData::String::dynamicConstCast( (*edgeInfo)["inputPortId"] )->value();

    return ::boost::make_tuple( nodeFrom, nodeTo, outputPortId, inputPortId);
}

//-----------------------------------------------------------------------------

void GraphMsg::selectedNode( ::fwData::Node::csptr node )
{
    addEvent( GraphMsg::SELECTED_NODE , node );
}

//-----------------------------------------------------------------------------

::fwData::Node::csptr GraphMsg::getSelectedNode() const
{
    return ::fwData::Node::dynamicConstCast( getDataInfo( GraphMsg::SELECTED_NODE ) );
}

//-----------------------------------------------------------------------------

void GraphMsg::unselectedNode( ::fwData::Node::csptr node )
{
    addEvent( GraphMsg::UNSELECTED_NODE , node );
}

//-----------------------------------------------------------------------------

::fwData::Node::csptr GraphMsg::getUnselectedNode() const
{
    return ::fwData::Node::dynamicConstCast( getDataInfo( GraphMsg::UNSELECTED_NODE ) );
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

