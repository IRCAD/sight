/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <QVBoxLayout>
#include <QWidget>

#include <functional>

namespace sight::module::ui::qt::image
{

namespace imHelper = data::helper::MedicalImage;

static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

const service::IService::KeyType SliceIndexPositionEditor::s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SliceIndexPositionEditor::SliceIndexPositionEditor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SliceIndexPositionEditor::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SliceIndexPositionEditor::updateSliceType, this);
}

//------------------------------------------------------------------------------

SliceIndexPositionEditor::~SliceIndexPositionEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    auto* layout = new QVBoxLayout();

    m_sliceSelectorPanel = new sight::ui::qt::SliceSelector(m_displayAxisSelector, m_displayStepButtons);
    m_sliceSelectorPanel->setEnable(false);
    m_sliceSelectorPanel->setObjectName(serviceID);

    sight::ui::qt::SliceSelector::ChangeIndexCallback changeIndexCallback;
    changeIndexCallback = [this](unsigned&& PH1, auto&& ...){sliceIndexNotification(std::forward<decltype(PH1)>(PH1));};
    m_sliceSelectorPanel->setChangeIndexCallback(changeIndexCallback);

    sight::ui::qt::SliceSelector::ChangeIndexCallback changeTypeCallback;
    changeTypeCallback = [this](auto&& PH1, auto&& ...){sliceTypeNotification(std::forward<decltype(PH1)>(PH1));};
    m_sliceSelectorPanel->setChangeTypeCallback(changeTypeCallback);
    m_sliceSelectorPanel->setTypeSelection(m_orientation);

    layout->addWidget(m_sliceSelectorPanel);
    layout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(layout);

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

    const auto config = this->getConfiguration();

    auto orientation = config.get<std::string>("sliceIndex");

    boost::algorithm::trim(orientation);
    boost::algorithm::to_lower(orientation);

    if(orientation == "axial")
    {
        m_orientation = orientation_t::AXIAL; // Z
    }
    else if(orientation == "frontal")
    {
        m_orientation = orientation_t::FRONTAL; // Y
    }
    else if(orientation == "sagittal")
    {
        m_orientation = orientation_t::SAGITTAL; // X
    }
    else
    {
        SIGHT_FATAL("The value for the xml element \"sliceIndex\" can only be axial, frontal or sagittal.");
    }

    m_displayAxisSelector = config.get<bool>("displayAxisSelector", m_displayAxisSelector);

    m_displayStepButtons = config.get<bool>("displayStepButtons", m_displayStepButtons);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updating()
{
    const auto image = m_image.lock();

    const bool imageIsValid = imHelper::checkImageValidity(image.get_shared());
    m_sliceSelectorPanel->setEnable(imageIsValid);

    m_axialIndex = std::max(
        std::int64_t(0),
        imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL).value_or(0)
    );
    m_frontalIndex = std::max(
        std::int64_t(0),
        imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL).value_or(0)
    );
    m_sagittalIndex = std::max(
        std::int64_t(0),
        imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL).value_or(0)
    );

    this->updateSliceIndexFromImg(*image);
}

//-----------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    if(sagittal != m_sagittalIndex
       || frontal != m_frontalIndex
       || axial != m_axialIndex)
    {
        m_sagittalIndex = sagittal;
        m_frontalIndex  = frontal;
        m_axialIndex    = axial;
    }

    const auto image = m_image.lock();

    namespace imHelper = imHelper;

    imHelper::setSliceIndex(*image, orientation_t::AXIAL, m_axialIndex);
    imHelper::setSliceIndex(*image, orientation_t::FRONTAL, m_frontalIndex);
    imHelper::setSliceIndex(*image, orientation_t::SAGITTAL, m_sagittalIndex);

    this->updateSliceIndexFromImg(*image);
}

//-----------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceType(int from, int to)
{
    if(to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(from);
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(to);
    }

    this->updateSliceTypeFromImg(m_orientation);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceIndexFromImg(sight::data::Image& _image)
{
    if(imHelper::checkImageValidity(_image))
    {
        // Default value take the middle of the size.
        const auto image_size = _image.getSize();
        const auto index      = imHelper::getSliceIndex(_image, m_orientation).value_or(image_size[m_orientation] / 2);

        // Update QSlider
        int max = 0;
        if(_image.numDimensions() > m_orientation)
        {
            max = static_cast<int>(_image.getSize()[m_orientation] - 1);
        }

        m_sliceSelectorPanel->setSliceRange(0, max);
        m_sliceSelectorPanel->setSliceValue(static_cast<int>(index));
    }
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::updateSliceTypeFromImg(const orientation_t& type)
{
    // Update Type Choice
    m_sliceSelectorPanel->setTypeSelection(static_cast<int>(type));

    const auto image = m_image.lock();
    SIGHT_ASSERT("The inout key '" + s_IMAGE_INOUT + "' is not defined.", image);

    this->updateSliceIndexFromImg(*image);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceIndexNotification(unsigned int index)
{
    const auto image = m_image.lock();

    imHelper::setSliceIndex(*image, m_orientation, index);

    std::array idx {
        static_cast<int>(imHelper::getSliceIndex(
                             *image,
                             imHelper::orientation_t::SAGITTAL
        ).value_or(0)),
        static_cast<int>(imHelper::getSliceIndex(
                             *image,
                             imHelper::orientation_t::FRONTAL
        ).value_or(0)),
        static_cast<int>(imHelper::getSliceIndex(
                             *image,
                             imHelper::orientation_t::AXIAL
        ).value_or(0))
    };

    auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>(
        data::Image::s_SLICE_INDEX_MODIFIED_SIG
    );
    core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_INDEX_SLOT)));
    sig->asyncEmit(idx[2], idx[1], idx[0]);
}

//------------------------------------------------------------------------------

void SliceIndexPositionEditor::sliceTypeNotification(int _type)
{
    auto type = static_cast<orientation_t>(_type);
    SIGHT_ASSERT(
        "Bad slice type " << type,
        type == orientation_t::X_AXIS
        || type == orientation_t::Y_AXIS
        || type == orientation_t::Z_AXIS
    );

    const auto oldType = m_orientation;
    // Change slice type
    m_orientation = static_cast<orientation_t>(type);

    // Fire the signal
    {
        const auto image = m_image.lock();

        auto sig = image->signal<data::Image::SliceTypeModifiedSignalType>(
            data::Image::s_SLICE_TYPE_MODIFIED_SIG
        );
        {
            core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_SLICE_TYPE_SLOT)));
            sig->asyncEmit(oldType, _type);
        }
        this->updateSliceIndexFromImg(*image);
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SliceIndexPositionEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, IService::slots::s_UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
