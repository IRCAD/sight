/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Edge.hpp>
#include <fwData/Graph.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>

#include <fwComEd/GraphMsg.hpp>
#include <fwComEd/helper/Composite.hpp>
#include "ctrlSelection/updater/SelectedNodeIOUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SelectedNodeIOUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SelectedNodeIOUpdaterSrv::SelectedNodeIOUpdaterSrv() throw()
{
	 m_managedEvents.push_back( ::fwComEd::GraphMsg::SELECTED_NODE );
	 addNewHandledEvent( ::fwComEd::GraphMsg::SELECTED_NODE );
}

//-----------------------------------------------------------------------------

SelectedNodeIOUpdaterSrv::~SelectedNodeIOUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
	OSLM_WARN("YYY SelectedNodeIOUpdaterSrv::updating " <<  _msg->getGeneralInfo() );

	::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
	::fwData::Graph::csptr cgraph = ::fwData::Graph::dynamicConstCast( _msg->getSubject().lock() );
	::fwData::Graph::sptr graph   = ::boost::const_pointer_cast< ::fwData::Graph >( cgraph );

	::fwComEd::GraphMsg::csptr  msg = ::fwComEd::GraphMsg::dynamicConstCast( _msg );
	OSLM_ASSERT( "not a SELECTED_NODE msg from Graph", msg->hasEvent( ::fwComEd::GraphMsg::SELECTED_NODE ) );

	::fwData::Node::csptr cnode = msg->getSelectedNode();
	::fwData::Node::sptr node =  ::boost::const_pointer_cast< ::fwData::Node >( cnode );


	::fwComEd::helper::Composite compositeHelper( composite );
	// remove previous data
	while ( !composite->getRefMap().empty() )
	{
		compositeHelper.remove( composite->getRefMap().begin()->first );
	}



	bool upStream=true;
    std::vector< ::fwData::Edge::sptr > dataEdges;
    dataEdges = graph->getEdges( node, upStream,  ::fwData::Edge::NATURE_DATA ,"");

    BOOST_FOREACH( ::fwData::Edge::sptr dataEdge , dataEdges )
    {
    	::fwData::Node::sptr dataNode = graph->getSourceNode( dataEdge );
		 std::string compositeKey = dataEdge->getPortID(!upStream);
		 compositeHelper.add( compositeKey , dataNode->getObject() );
    }
    compositeHelper.notify( this->getSptr() );


//    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
//    for (   ManagedEvents::iterator it = m_managedEvents.begin();
//            it != m_managedEvents.end();
//            ++it )
//    {
//    	std::string event         = it->get<0>();
//    	std::string uuid          = it->get<1>();
//    	std::string compositeKey  = it->get<2>();
//    	ctrlSelection::IUpdaterSrv::ActionType action        = it->get<3>();
//
//        //  test if message correspond to a defined event
//        if( _msg->hasEvent( event ) )
//        {
//            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
//            SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");
//
//            // Test if we manage this event from this object message uid
//            if( obj->getUUID() == uuid )
//            {
//            	::fwData::Object::sptr dataInfo = ::boost::const_pointer_cast< ::fwData::Object >(_msg->getDataInfo(  event ));
//            	SLM_ASSERT("no dataInfo set!!!" ,  dataInfo );
//                // Udpate the composite object referenced by the composite key
//            	OSLM_WARN("YYYYYYY SelectedNodeIOUpdaterSrv::updating");
//                this->updateComposite(composite, dataInfo , compositeKey , action );
//            }
//        }
//    }
}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    //this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
