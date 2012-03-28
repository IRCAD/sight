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

#include <fwTools/fwID.hpp>

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
    addNewHandledEvent( ::fwComEd::GraphMsg::UNSELECTED_NODE );
    addNewHandledEvent( ::fwComEd::GraphMsg::SELECTED_NODE );
    m_upStream = false;
}

//-----------------------------------------------------------------------------

SelectedNodeIOUpdaterSrv::~SelectedNodeIOUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void SelectedNodeIOUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    OSLM_TRACE("SelectedNodeIOUpdaterSrv::updating " <<  _msg->getGeneralInfo() );

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwData::Graph::csptr cgraph = ::fwData::Graph::dynamicConstCast( _msg->getSubject().lock() );
    ::fwData::Graph::sptr graph   = ::boost::const_pointer_cast< ::fwData::Graph >( cgraph );

    ::fwComEd::helper::Composite compositeHelper( composite );

    if ( _msg->hasEvent( ::fwComEd::GraphMsg::SELECTED_NODE ) || _msg->hasEvent( ::fwComEd::GraphMsg::UNSELECTED_NODE ) )
    {
        // remove previous data for SELECTED_NODE and UNSELECTED_NODE
        while ( !composite->getContainer().empty() )
        {
            compositeHelper.remove( composite->begin()->first );
        }

        if ( _msg->hasEvent( ::fwComEd::GraphMsg::SELECTED_NODE ) )
        {
            ::fwComEd::GraphMsg::csptr  msg = ::fwComEd::GraphMsg::dynamicConstCast( _msg );
            ::fwData::Node::csptr cnode = msg->getSelectedNode();
            ::fwData::Node::sptr node =  ::boost::const_pointer_cast< ::fwData::Node >( cnode );
            // repopulate composite
            std::vector< ::fwData::Edge::sptr > dataEdges;
            dataEdges = graph->getEdges( node, m_upStream,  ::fwData::Edge::NATURE_DATA );

            BOOST_FOREACH( ::fwData::Edge::sptr dataEdge , dataEdges )
            {
                ::fwData::Node::sptr dataNode = graph->getNode( dataEdge , m_upStream );
                 std::string compositeKey = dataEdge->getPortID(!m_upStream);
                 compositeHelper.add( compositeKey , dataNode->getObject() );
            }
        }
        compositeHelper.notify( this->getSptr() );
    }

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
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector < ConfigurationType > vectConfig = m_configuration->find("mode");
    SLM_ASSERT("Missing <mode> tag!", !vectConfig.empty());
    std::string modeval = vectConfig.at(0)->getValue(); // "input" or "output"
    m_upStream = modeval=="input"?true:false;


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
