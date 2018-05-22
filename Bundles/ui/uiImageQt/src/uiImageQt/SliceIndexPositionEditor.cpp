/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiImageQt/SliceIndexPositionEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <QVBoxLayout>
#include <QWidget>

#include <functional>

namespace uiImageQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiImageQt::SliceIndexPositionEditor, ::fwData::Image );

const std::string* SliceIndexPositionEditor::SLICE_INDEX_FIELDID[ 3 ] =
{
    &fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId,
    &fwDataTools::fieldHelper::Image::m_frontalSliceIndexId,
    &fwDataTools::fieldHelper::Image::m_axialSliceIndexId
};

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SliceIndexPositionEditor::SliceIndexPositionEditor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SliceIndexPositionEditor::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SliceIndexPositionEditor::updateSliceType, this);
}

//------------------------------------------------------------------------------

SliceIndexPositionEditor::~SliceIndexPositionEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout( );

    m_sliceSelectorPanel = new ::fwGuiQt::SliceSelector();
    m_sliceSelectorPanel->setEnable(false);

    ::fwGuiQt::SliceSelector::ChangeIndexCallback changeIndexCallback;
    changeIndexCallback = std::bind( &::uiImageQt::SliceIndexPositionEditor::sliceIndexNotification, this,
                                     std::placeholders::_1);
    m_sliceSelectorPanel->setChangeIndexCallback(changeIndexCallback);

    ::fwGuiQt::SliceSelector::ChangeIndexCallback changeTypeCallback;
    changeTypeCallback = std::bind( &::uiImageQt::SliceIndexPositionEditor::sliceTypeNotification, this,
                                    std::placeholders::_1);
    m_sliceSelectorPanel->setChangeTypeCallback(changeTypeCallback);
    layout->addWidget( m_sliceSelectorPanel );
    layout->setContentsMargins(0, 0, 0, 0);

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }
    this->updateImageInfos(image);
    this->updateSliceTypeFromImg(m_orientation);

    qtContainer->setLayout( layout );

    this->updating();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::stopping()
{
    this->destroy();
    m_sliceSelectorPanel = nullptr;
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::configuring()
{
    this->initialize();

    if( this->m_configuration->size() > 0 )
    {
        std::vector< ::fwRuntime::ConfigurationElement::sptr > slideIndexCfg = m_configuration->find("sliceIndex");
        SLM_ASSERT("Only one xml element \"sliceIndex\" is accepted.", slideIndexCfg.size() == 1 );
        SLM_ASSERT("The xml element \"sliceIndex\" is empty.", !(*slideIndexCfg.begin())->getValue().empty() );
        std::string orientation = (*slideIndexCfg.begin())->getValue();
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

void SliceIndexPositionEditor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_sliceSelectorPanel->setEnable(imageIsValid);
    this->updateImageInfos(image);
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::swapping()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }

    image->setField( fwDataTools::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
    image->setField( fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
    image->setField( fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceType(int from, int to)
{
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation = static_cast< Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< Orientation >( to );
    }
    this->updateSliceTypeFromImg(m_orientation);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndexFromImg()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Get Index
        std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
        OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
        unsigned int index = image->getField< ::fwData::Integer >( fieldID )->value();

        // Update QSlider
        int max = 0;
        if(image->getNumberOfDimensions() > m_orientation)
        {
            max = static_cast<int>(image->getSize()[m_orientation]-1);
        }
        m_sliceSelectorPanel->setSliceRange( 0, max );
        m_sliceSelectorPanel->setSliceValue( index );
    }
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceTypeFromImg(Orientation type )
{
    // Update Type Choice
    m_sliceSelectorPanel->setTypeSelection( static_cast< int >( type ) );

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceIndexNotification( unsigned int index)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }

    std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
    OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ));
    image->getField< ::fwData::Integer >( fieldID )->value() = index;

    auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
    ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_INDEX_SLOT)));
    sig->asyncEmit(m_axialIndex->value(), m_frontalIndex->value(), m_sagittalIndex->value());
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceTypeNotification( int _type )
{
    Orientation type = static_cast< Orientation >( _type );
    OSLM_ASSERT("Bad slice type "<<type, type == X_AXIS ||
                type == Y_AXIS ||
                type == Z_AXIS );

    int oldType = static_cast< int > ( m_orientation );
    // Change slice type
    m_orientation = type;

    // Fire the signal
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    if (!image)
    {
        FW_DEPRECATED_MSG("The service image is not correctly set, it must have an 'image' as 'inout'");
        image = this->getObject< ::fwData::Image >();
    }
    auto sig = image->signal< ::fwData::Image::SliceTypeModifiedSignalType >(
        ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_TYPE_SLOT)));
        sig->asyncEmit(oldType, _type);
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SliceIndexPositionEditor::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SliceIndexPositionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    //FIXME hack to support deprecated getObject()
    if (this->getInOut< ::fwData::Image >(s_IMAGE_INOUT))
    {
        connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
        connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
        connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    }

    return connections;
}

//------------------------------------------------------------------------------

}
