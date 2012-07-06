/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <wx/colour.h>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "uiImageWx/SliceIndexPositionEditor.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::SliceIndexPositionEditor , ::fwData::Image ) ;

const std::string* SliceIndexPositionEditor::SLICE_INDEX_FIELDID[ 3 ] =
{
    &fwComEd::Dictionary::m_sagittalSliceIndexId,
    &fwComEd::Dictionary::m_frontalSliceIndexId,
    &fwComEd::Dictionary::m_axialSliceIndexId
};

//------------------------------------------------------------------------------

SliceIndexPositionEditor::SliceIndexPositionEditor() throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
}

//------------------------------------------------------------------------------

SliceIndexPositionEditor::~SliceIndexPositionEditor() throw()
{}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::starting() throw(::fwTools::Failed)
{
    this->create();

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    SLM_ASSERT("container not instanced", container);

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );

    m_sliceSelectorPanel = new ::fwWX::SliceSelector( container );
    m_sliceSelectorPanel->setEnable(false);

    ::fwWX::SliceSelector::ChangeIndexCallback changeIndexCallback;
    changeIndexCallback = ::boost::bind( &::uiImage::SliceIndexPositionEditor::sliceIndexNotification, this, _1);
    m_sliceSelectorPanel->setChangeIndexCallback(changeIndexCallback);

    ::fwWX::SliceSelector::ChangeIndexCallback changeTypeCallback;
    changeTypeCallback = ::boost::bind( &::uiImage::SliceIndexPositionEditor::sliceTypeNotification, this, _1);
    m_sliceSelectorPanel->setChangeTypeCallback(changeTypeCallback);
    sizer->Add( m_sliceSelectorPanel, 1, wxALL|wxEXPAND, 1 );

    this->updateSliceType(m_orientation);

    container->SetSizer( sizer );
    container->Layout();
    this->updating();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::stopping() throw(::fwTools::Failed)
{
    if(m_sliceSelectorPanel)
    {
        delete m_sliceSelectorPanel;
        m_sliceSelectorPanel = 0;
    }
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    SLM_ASSERT("container not instanced", container);

    wxContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->initialize();

    if( this->m_configuration->size() > 0 )
    {
        ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
        SLM_ASSERT("Sorry, only one xml element \"sliceIndex\" is accepted.", this->m_configuration->size() == 1 && (*iter)->getName() == "sliceIndex" );
        SLM_ASSERT("Sorry, xml element \"sliceIndex\" is empty.", ! (*iter)->getValue().empty() );
        std::string  orientation = (*iter)->getValue();
        ::boost::algorithm::trim(orientation);
        ::boost::algorithm::to_lower(orientation);

        if(orientation == "axial" )
        {
            m_orientation = Z_AXIS;
        }
        else if(orientation == "frontal" )
        {
            m_orientation = Y_AXIS;
        }
        else if(orientation == "sagittal" )
        {
            m_orientation = X_AXIS;
        }
        else
        {
            SLM_FATAL("The value for the xml element \"sliceIndex\" can only be axial, frontal or sagittal.");
        }
    }
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_sliceSelectorPanel->setEnable(imageIsValid);
    this->updateImageInfos(image);
    this->updateSliceIndex();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imageMessage = fwComEd::ImageMsg::dynamicConstCast( _msg );

    if ( imageMessage )
    {
        if ( imageMessage->hasEvent( fwComEd::ImageMsg::BUFFER ) )
        {
            this->updating();
        }
        if ( imageMessage->hasEvent( fwComEd::ImageMsg::SLICE_INDEX ) )
        {
            imageMessage->getSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
            ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
            image->setField( fwComEd::Dictionary::m_axialSliceIndexId  , m_axialIndex);
            image->setField( fwComEd::Dictionary::m_frontalSliceIndexId , m_frontalIndex);
            image->setField( fwComEd::Dictionary::m_sagittalSliceIndexId, m_sagittalIndex);
            this->updateSliceIndex();
        }
        if ( imageMessage->hasEvent( fwComEd::ImageMsg::CHANGE_SLICE_TYPE ) )
        {
            ::fwData::Object::csptr cObjInfo = imageMessage->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
            ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
            ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

            ::fwData::Integer::sptr fromSliceType = ::fwData::Integer::dynamicCast( info->getContainer()["fromSliceType"] );
            ::fwData::Integer::sptr toSliceType = ::fwData::Integer::dynamicCast( info->getContainer()["toSliceType"] );

            if( toSliceType->value() == static_cast< int > ( m_orientation ) )
            {
                m_orientation = static_cast< Orientation > ( fromSliceType->value() );
            }
            else if(fromSliceType->value() == static_cast<int>(m_orientation))
            {
                m_orientation = static_cast< Orientation >( toSliceType->value() );
            }
            this->updateSliceType(m_orientation);
        }
    }
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndex()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    // Get Index
    std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
    OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
    unsigned int index = image->getField< ::fwData::Integer >( fieldID )->value();

    // Update wxSlider
    int max = image->getSize()[m_orientation]-1;
    m_sliceSelectorPanel->setSliceRange( 0, max );
    m_sliceSelectorPanel->setSliceValue( index );
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceType(Orientation type )
{
    // Update Type Choice
    m_sliceSelectorPanel->setTypeSelection( static_cast< int >( type ) );

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    // Get Index
     std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
     OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ));
    unsigned int index = image->getField< ::fwData::Integer >( fieldID )->value();
    int max = image->getSize()[m_orientation]-1;
    m_sliceSelectorPanel->setSliceRange( 0, max );
    m_sliceSelectorPanel->setSliceValue( index );
    this->updateSliceIndex();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceIndexNotification( unsigned int index)
{
    // Fire the message
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->setSliceIndex( m_axialIndex, m_frontalIndex, m_sagittalIndex);
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    SliceIndexPositionEditor::SLICE_INDEX_FIELDID[m_orientation];
    std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
    OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
    image->getField< ::fwData::Integer >( fieldID )->value() = index;

    ::fwServices::IEditionService::notify(this->getSptr(),  image, msg);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceTypeNotification( int _type )
{
    Orientation type = static_cast< Orientation >( _type );
    OSLM_ASSERT("Bad slice type "<<type, type == X_AXIS ||
            type == Y_AXIS ||
            type == Z_AXIS );

    // Change data info
    ::fwData::Composite::NewSptr info;
    ::fwData::Integer::NewSptr fromSliceType;
    ::fwData::Integer::NewSptr toSliceType;
    fromSliceType->value() = static_cast< int > ( m_orientation ) ;
    toSliceType->value() = static_cast< int > ( type ) ;
    info->getContainer()["fromSliceType"] = fromSliceType;
    info->getContainer()["toSliceType"] = toSliceType;

    // Change slice type
    m_orientation = type;

    // Fire the message
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE, info ) ;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwServices::IEditionService::notify(this->getSptr(),  image, msg);
    this->updateSliceIndex();
}
}

