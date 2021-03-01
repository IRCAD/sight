/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SSliceIndexPositionEditor.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Image.hpp>

#include <functional>

namespace sight::module::ui::qml::image
{

const std::string* SSliceIndexPositionEditor::SLICE_INDEX_FIELDID[ 3 ] =
{
    &data::fieldHelper::Image::m_sagittalSliceIndexId,
    &data::fieldHelper::Image::m_frontalSliceIndexId,
    &data::fieldHelper::Image::m_axialSliceIndexId
};

static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const service::IService::KeyType s_IMAGE_INOUT = "image";

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
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
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
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    this->updateImageInfos(image);
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void SSliceIndexPositionEditor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    image->setField( data::fieldHelper::Image::m_axialSliceIndexId, m_axialIndex);
    image->setField( data::fieldHelper::Image::m_frontalSliceIndexId, m_frontalIndex);
    image->setField( data::fieldHelper::Image::m_sagittalSliceIndexId, m_sagittalIndex);
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
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    if (data::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Get Index
        const std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
        SIGHT_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
        const int index = static_cast<int>(image->getField< data::Integer >( fieldID )->value());

        // Update QSlider
        int max = 0;
        if(image->getNumberOfDimensions() > m_orientation)
        {
            max = static_cast<int>(image->getSize2()[m_orientation]-1);
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

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::onSliceIndex(int index)
{
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    const std::string fieldID = *SLICE_INDEX_FIELDID[m_orientation];
    SIGHT_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ));
    image->getField< data::Integer >( fieldID )->value() = index;

    auto sig = image->signal< data::Image::SliceIndexModifiedSignalType >(
        data::Image::s_SLICE_INDEX_MODIFIED_SIG);
    core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_INDEX_SLOT)));
    sig->asyncEmit(static_cast<int>(m_axialIndex->value()), static_cast<int>(m_frontalIndex->value()),
                   static_cast<int>(m_sagittalIndex->value()));
}

//------------------------------------------------------------------------------

void SSliceIndexPositionEditor::onSliceType( int _type )
{
    Orientation type = static_cast< Orientation >( _type );
    SIGHT_ASSERT("Bad slice type "<<type, type == X_AXIS ||
                 type == Y_AXIS ||
                 type == Z_AXIS );

    const int oldType = static_cast< int > ( m_orientation );
    // Change slice type
    m_orientation = type;

    // Fire the signal
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);

    auto sig = image->signal< data::Image::SliceTypeModifiedSignalType >(
        data::Image::s_SLICE_TYPE_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_TYPE_SLOT)));
        sig->asyncEmit(oldType, _type);
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSliceIndexPositionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

}
