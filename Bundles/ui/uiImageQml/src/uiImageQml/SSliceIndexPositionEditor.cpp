/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiImageQml/SSliceIndexPositionEditor.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <functional>

namespace uiImageQml
{

const std::string* SSliceIndexPositionEditor::SLICE_INDEX_FIELDID[ 3 ] =
{
    &fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId,
    &fwDataTools::fieldHelper::Image::m_frontalSliceIndexId,
    &fwDataTools::fieldHelper::Image::m_axialSliceIndexId
};

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SSliceIndexPositionEditor::SSliceIndexPositionEditor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SSliceIndexPositionEditor::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SSliceIndexPositionEditor::updateSliceType, this);

    this->registerObject(s_IMAGE_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SSliceIndexPositionEditor::~SSliceIndexPositionEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::starting()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos(image);
    this->updateSliceTypeFromImg(m_orientation);

    this->updating();
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::stopping()
{

}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::configuring()
{

}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    this->updateImageInfos(image);
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void SSliceIndexPositionEditor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    image->setField( fwDataTools::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
    image->setField( fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
    image->setField( fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void SSliceIndexPositionEditor::configureSliceIndex(int sliceIndex)
{
    m_orientation = static_cast< Orientation >(sliceIndex);
    this->updateSliceTypeFromImg(m_orientation);
}

//-----------------------------------------------------------------------------

void SSliceIndexPositionEditor::updateSliceType(int from, int to)
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

void SSliceIndexPositionEditor::updateSliceIndexFromImg()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if (::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Get Index
        const std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
        OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
        const int index = image->getField< ::fwData::Integer >( fieldID )->value();

        // Update QSlider
        int max = 0;
        if(image->getNumberOfDimensions() > m_orientation)
        {
            max = static_cast<int>(image->getSize()[m_orientation]-1);
        }
        this->setSliceRange( 0, max );
        this->setSliceValue( index );
    }
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::updateSliceTypeFromImg(Orientation type )
{
    // Update Type Choice
    this->setSliceType(static_cast< int >( type ));

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::onSliceIndex(int index)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
    OSLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ));
    image->getField< ::fwData::Integer >( fieldID )->value() = index;

    auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
    ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_INDEX_SLOT)));
    sig->asyncEmit(m_axialIndex->value(), m_frontalIndex->value(), m_sagittalIndex->value());
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::onSliceType( int _type )
{
    Orientation type = static_cast< Orientation >( _type );
    OSLM_ASSERT("Bad slice type "<<type, type == X_AXIS ||
                type == Y_AXIS ||
                type == Z_AXIS );

    const int oldType = static_cast< int > ( m_orientation );
    // Change slice type
    m_orientation = type;

    // Fire the signal
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    auto sig = image->signal< ::fwData::Image::SliceTypeModifiedSignalType >(
        ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_TYPE_SLOT)));
        sig->asyncEmit(oldType, _type);
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSliceIndexPositionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

}
