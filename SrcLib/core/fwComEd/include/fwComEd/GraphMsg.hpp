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
 * @brief   Object message specialized for PlaneList : store modification information
 * @class   GraphMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API GraphMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((GraphMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new GraphMsg );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_GRAPH;
    FWCOMED_API static std::string ADD_NODE;
    FWCOMED_API static std::string REMOVE_NODE;
    FWCOMED_API static std::string ADD_EDGE;
    FWCOMED_API static std::string REMOVE_EDGE;
    FWCOMED_API static std::string SELECTED_NODE;
    FWCOMED_API static std::string SELECTED_OPERATOR;
    FWCOMED_API static std::string EXECUTE_GRAPH;

    /**
      * @brief  Constructor : does nothing.
      */
    FWCOMED_API GraphMsg() throw();

    /**
      * @brief  Destuctor : does nothing.
      */
    FWCOMED_API virtual ~GraphMsg() throw();

    FWCOMED_API virtual void addedNode( ::fwData::Node::sptr node );
    FWCOMED_API virtual ::fwData::Node::csptr getAddedNode() const;

    FWCOMED_API virtual void removedNode( ::fwData::Node::sptr node );
    FWCOMED_API virtual ::fwData::Node::csptr getRemovedNode() const;

    FWCOMED_API virtual void selectedNode( ::fwData::Node::sptr node );
    FWCOMED_API virtual ::fwData::Node::csptr getSelectedNode() const;

};

} // namespace fwComEd

#endif //_FWCOMED_GRAPHMSG_HPP_

