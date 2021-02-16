/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/image/SliceIndexPositionEditor.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <service/macros.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <QVBoxLayout>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>

#include <functional>

namespace sight::modules::ui::qt::image
{

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::modules::ui::qt::image::SliceIndexPositionEditor,
                         ::sight::data::Image )

const std::string* SliceIndexPositionEditor::SLICE_INDEX_FIELDID[ 3 ] =
{
    &data::tools::fieldHelper::Image::m_sagittalSliceIndexId,
    &data::tools::fieldHelper::Image::m_frontalSliceIndexId,
    &data::tools::fieldHelper::Image::m_axialSliceIndexId
};

static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const service::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SliceIndexPositionEditor::SliceIndexPositionEditor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SliceIndexPositionEditor::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SliceIndexPositionEditor::updateSliceType, this);
    this->registerObject(s_IMAGE_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SliceIndexPositionEditor::~SliceIndexPositionEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout( );

    m_sliceSelectorPanel = new sight::ui::qt::SliceSelector();
    m_sliceSelectorPanel->setEnable(false);

    sight::ui::qt::SliceSelector::ChangeIndexCallback changeIndexCallback;
    changeIndexCallback = std::bind( &SliceIndexPositionEditor::sliceIndexNotification, this, std::placeholders::_1);
    m_sliceSelectorPanel->setChangeIndexCallback(changeIndexCallback);

    sight::ui::qt::SliceSelector::ChangeIndexCallback changeTypeCallback;
    changeTypeCallback = std::bind( &SliceIndexPositionEditor::sliceTypeNotification, this, std::placeholders::_1);
    m_sliceSelectorPanel->setChangeTypeCallback(changeTypeCallback);
    layout->addWidget( m_sliceSelectorPanel );
    layout->setContentsMargins(0, 0, 0, 0);

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);
    m_helper.updateImageInfos(image);
    this->updateSliceTypeFromImg(m_helper.getOrientation());

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
        std::vector< core::runtime::ConfigurationElement::sptr > slideIndexCfg = m_configuration->find("sliceIndex");
        SLM_ASSERT("Only one xml element \"sliceIndex\" is accepted.", slideIndexCfg.size() == 1 );
        SLM_ASSERT("The xml element \"sliceIndex\" is empty.", !(*slideIndexCfg.begin())->getValue().empty() );
        std::string orientation = (*slideIndexCfg.begin())->getValue();
        ::boost::algorithm::trim(orientation);
        ::boost::algorithm::to_lower(orientation);

        if(orientation == "axial" )
        {
            m_helper.setOrientation(data::tools::helper::MedicalImage::Z_AXIS);
        }
        else if(orientation == "frontal" )
        {
            m_helper.setOrientation(data::tools::helper::MedicalImage::Y_AXIS);
        }
        else if(orientation == "sagittal" )
        {
            m_helper.setOrientation(data::tools::helper::MedicalImage::X_AXIS);
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
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    const bool imageIsValid = data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_sliceSelectorPanel->setEnable(imageIsValid);
    m_helper.updateImageInfos(image);
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
    const int indexes[] = {sagittal, frontal, axial};
    m_helper.setSliceIndex(indexes);

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    data::Integer::sptr indexesPtr[3];
    m_helper.getSliceIndex(indexesPtr);
    image->setField( data::tools::fieldHelper::Image::m_axialSliceIndexId, indexesPtr[2]);
    image->setField( data::tools::fieldHelper::Image::m_frontalSliceIndexId, indexesPtr[1]);
    image->setField( data::tools::fieldHelper::Image::m_sagittalSliceIndexId, indexesPtr[0]);
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceType(int from, int to)
{
    if( to == static_cast< int > (m_helper.getOrientation()) )
    {
        m_helper.setOrientation(from);
    }
    else if(from == static_cast<int>(m_helper.getOrientation()))
    {
        m_helper.setOrientation(to);
    }
    this->updateSliceTypeFromImg(m_helper.getOrientation());
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::info( std::ostream& )
{
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndexFromImg()
{
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    if (data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Get Index
        const std::string fieldID = *SLICE_INDEX_FIELDID[m_helper.getOrientation()];
        SLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ) );
        const int index = static_cast<int>(image->getField< data::Integer >( fieldID )->value());

        // Update QSlider
        int max = 0;
        if(image->getNumberOfDimensions() > m_helper.getOrientation())
        {
            max = static_cast<int>(image->getSize2()[m_helper.getOrientation()]-1);
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

    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceIndexNotification( unsigned int index)
{
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    const std::string fieldID = *SLICE_INDEX_FIELDID[m_helper.getOrientation()];
    SLM_ASSERT("Field "<<fieldID<<" is missing", image->getField( fieldID ));
    image->getField< data::Integer >( fieldID )->value() = index;

    auto sig = image->signal< data::Image::SliceIndexModifiedSignalType >(
        data::Image::s_SLICE_INDEX_MODIFIED_SIG);
    core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_INDEX_SLOT)));
    data::Integer::sptr indexes[3];
    m_helper.getSliceIndex(indexes);
    sig->asyncEmit(static_cast<int>(indexes[2]->value()), static_cast<int>(indexes[1]->value()),
                   static_cast<int>(indexes[0]->value()));
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceTypeNotification( int _type )
{
    Orientation type = static_cast< Orientation >( _type );
    SLM_ASSERT("Bad slice type "<<type, type == data::tools::helper::MedicalImage::X_AXIS ||
               type == data::tools::helper::MedicalImage::Y_AXIS ||
               type == data::tools::helper::MedicalImage::Z_AXIS );

    const int oldType = static_cast< int > (m_helper.getOrientation());
    // Change slice type
    m_helper.setOrientation(type);

    // Fire the signal
    data::Image::sptr image = this->getInOut< data::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    auto sig = image->signal< data::Image::SliceTypeModifiedSignalType >(
        data::Image::s_SLICE_TYPE_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_TYPE_SLOT)));
        sig->asyncEmit(oldType, _type);
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SliceIndexPositionEditor::getAutoConnections() const
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
