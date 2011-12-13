/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransfertFunction.hpp>
#include <fwData/Composite.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "ctrlSelection/SSynchronizeWindowingTF.hpp"

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwServices::IController, ::ctrlSelection::SSynchronizeWindowingTF, ::fwData::Image ) ;

//-----------------------------------------------------------------------------

SSynchronizeWindowingTF::SSynchronizeWindowingTF() throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::WINDOWING );
    addNewHandledEvent( ::fwComEd::ImageMsg::TRANSFERTFUNCTION );

    // add default TF selection field
    m_tfSelectionFieldIds.push_back(::fwComEd::Dictionary::m_transfertFunctionId);
}

//-----------------------------------------------------------------------------

SSynchronizeWindowingTF::~SSynchronizeWindowingTF() throw()
{}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    if (message->hasEvent(::fwComEd::ImageMsg::WINDOWING) ||
        message->hasEvent(::fwComEd::ImageMsg::TRANSFERTFUNCTION))
    {
        this->updateTFFromMinMax();
    }
}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::starting()  throw ( ::fwTools::Failed )
{
    this->updateTFFromMinMax();
}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::swapping()  throw ( ::fwTools::Failed )
{
    this->updateTFFromMinMax();
}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::configuring()  throw ( ::fwTools::Failed )
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("tfSelection");

    BOOST_FOREACH ( ::fwRuntime::ConfigurationElement::sptr element, vectConfig)
    {
        OSLM_FATAL_IF( "missing 'id' attribute for " << this->getID() << "configuration", !element->hasAttribute("id") );
        std::string id = element->getAttributeValue("id");
        OSLM_FATAL_IF("'id' attribute must not be empty", id.empty());
        m_tfSelectionFieldIds.push_back(id);
    }
}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

void SSynchronizeWindowingTF::updateTFFromMinMax()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    SLM_ASSERT("Image must have Windowing min/max fields",
            image->getFieldSize( ::fwComEd::Dictionary::m_windowMinId ) &&
            image->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId ));

    ::fwData::Integer::sptr min = image->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMinId );
    ::fwData::Integer::sptr max = image->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMaxId );

    ::fwData::Composite::sptr tfCompo = image->getFieldSingleElement< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );

    BOOST_FOREACH(TFFieldIdContainerType::value_type fieldId, m_tfSelectionFieldIds)
    {
        ::fwData::String::sptr tfName;
        if(image->getFieldSize(fieldId) == 0)
        {
            tfName = ::fwData::String::New(::fwData::TransfertFunction::defaultTransfertFunctionName);
            image->setFieldSingleElement( fieldId, tfName );
        }
        else
        {
            tfName = image->getFieldSingleElement< ::fwData::String >( fieldId );
        }

        // If TF doesn't exist : set default BW TF
        // This case can occur if the TF is deleted by TF editor
        if (tfCompo->find(tfName->value()) == tfCompo->end())
        {
            OSLM_WARN("TF '" << *tfName << "' doesn't exist => set BW TF");
            ::fwComEd::fieldHelper::MedicalImageHelpers::setBWTF(image, fieldId);

            ::fwComEd::ImageMsg::NewSptr msg;
            msg->addEvent(::fwComEd::ImageMsg::TRANSFERTFUNCTION) ;
            ::fwServices::IEditionService::notify( this->getSptr(),  image, msg );

            tfName = image->getFieldSingleElement< ::fwData::String >( fieldId );
        }

        ::fwData::TransfertFunction::sptr pTF = ::fwData::TransfertFunction::dynamicCast( (*tfCompo)[tfName->value()] );
        ::fwComEd::fieldHelper::MedicalImageHelpers::updateTFFromMinMax(min, max, pTF);
    }
}

//-----------------------------------------------------------------------------


} // ctrlSelection
