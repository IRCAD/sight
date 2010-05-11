/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "ctrlSelection/manager/MedicalImageManagerSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::MedicalImageManagerSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

MedicalImageManagerSrv::MedicalImageManagerSrv() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS );
}

//-----------------------------------------------------------------------------

MedicalImageManagerSrv::~MedicalImageManagerSrv() throw()
{}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    SLM_FATAL_IF("Received message must be compositeMsg", compositeMsg == 0 );

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getAddedFields();
        convertImages( fields );
    }

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getSwappedNewFields();
        convertImages( fields );
    }
}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::convertImages( ::fwData::Composite::sptr _composite )
{
    for(    ::fwData::Composite::Container::iterator  objectId = _composite->getRefMap().begin();
            objectId != _composite->getRefMap().end();
            ++objectId )
    {
        BOOST_FOREACH( std::string key, m_imageCompositeKeys )
        {
            if( objectId->first == key )
            {
                ::fwData::Image::sptr pImg = ::fwData::Image::dynamicCast( objectId->second );
                std::pair<bool, bool> MinMaxTFAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( pImg );
                bool landMarksAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks( pImg );
                bool sliceIndexAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageSliceIndex( pImg );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::configuring()  throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElementContainer updaters = m_configuration->findAllConfigurationElement("update");

    SLM_ASSERT("Problem with configuration for MedicalImageManagerSrv type, missing element \"update\"", updaters.size() != 0 );
    OSLM_DEBUG( "updaters.size() = " << updaters.size() );
    m_imageCompositeKeys.clear();
    for(    ::fwRuntime::ConfigurationElementContainer::Iterator item = updaters.begin();
            item != updaters.end();
            ++item )
    {
        SLM_FATAL_IF( "Sorry, attribute \"imageCompositeKey\" is missing", !(*item)->hasAttribute("imageCompositeKey") );
        std::string imageCompositeKey =  (*item)->getExistingAttributeValue("imageCompositeKey");
        m_imageCompositeKeys.push_back(imageCompositeKey);
    }
}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageManagerSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
