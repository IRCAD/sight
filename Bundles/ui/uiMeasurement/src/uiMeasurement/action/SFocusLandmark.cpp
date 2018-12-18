/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "uiMeasurement/action/SFocusLandmark.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Landmarks.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace uiMeasurement
{
namespace action
{

static const ::fwServices::IService::KeyType s_LANDMARKS_INPUT = "landmarks";
static const ::fwServices::IService::KeyType s_IMAGE_INOUT     = "image";

static const ::fwCom::Slots::SlotKeyType s_SELECT_LANDMARK_SLOT     = "selectLandmark";
static const ::fwCom::Slots::SlotKeyType s_DESELECT_LANDMARK_SLOT   = "deselectLandmark";
static const ::fwCom::Slots::SlotKeyType s_DESELECT_FROM_GROUP_SLOT = "deselectFromGroup";
static const ::fwCom::Slots::SlotKeyType s_RENAME_GROUP_SLOT        = "renameGroup";

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMeasurement::action::SFocusLandmark );

//------------------------------------------------------------------------------

SFocusLandmark::SFocusLandmark( ) noexcept :
    m_index(0),
    m_isSelected(false)
{
    newSlot(s_SELECT_LANDMARK_SLOT, &SFocusLandmark::selectLandmark, this);
    newSlot(s_DESELECT_LANDMARK_SLOT, &SFocusLandmark::deselectLandmark, this);
    newSlot(s_DESELECT_FROM_GROUP_SLOT, &SFocusLandmark::deselectFromGroup, this);
    newSlot(s_RENAME_GROUP_SLOT, &SFocusLandmark::renameGroup, this);
}

//------------------------------------------------------------------------------

SFocusLandmark::~SFocusLandmark() noexcept
{
}

//------------------------------------------------------------------------------

void SFocusLandmark::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
    this->setIsExecutable(m_isSelected);
}

//------------------------------------------------------------------------------

void SFocusLandmark::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SFocusLandmark::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SFocusLandmark::updating()
{
    if (m_isSelected)
    {
        ::fwData::Image::sptr pImage = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(pImage))
        {
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Add landmarks");
            messageBox.setMessage(
                "It is impossible to add image landmarks. There is no loaded image in the software." );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
            return;
        }
        else // Image is defined
        {
            ::fwData::Landmarks::csptr landmarks = this->getInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);

            try
            {
                const ::fwData::Landmarks::PointType& point = landmarks->getPoint(m_groupName, m_index);

                ::fwData::Integer::sptr paramA = ::fwData::Integer::New();
                paramA->value()                =
                    static_cast<int>((point[2] - pImage->getOrigin()[2] ) / pImage->getSpacing()[2] +0.5);
                ::fwData::Integer::sptr paramF = ::fwData::Integer::New();
                paramF->value()                =
                    static_cast<int>((point[1] -  pImage->getOrigin()[1]) / pImage->getSpacing()[1] +0.5);
                ::fwData::Integer::sptr paramS = ::fwData::Integer::New();
                paramS->value()                =
                    static_cast<int>((point[0] -  pImage->getOrigin()[0]) / pImage->getSpacing()[0] +0.5);
                if( paramS->value() >= 0 &&
                    paramF->value() >= 0 &&
                    paramA->value() >= 0 &&
                    pImage->getSize()[0] > static_cast< ::fwData::Image::SizeType::value_type >(paramS->value()) &&
                    pImage->getSize()[1] > static_cast< ::fwData::Image::SizeType::value_type >(paramF->value()) &&
                    pImage->getSize()[2] > static_cast< ::fwData::Image::SizeType::value_type >(paramA->value()) )
                {
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, paramA );
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, paramF );
                    pImage->setField( ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, paramS );

                    // notify
                    auto sig = pImage->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
                        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
                    sig->asyncEmit(paramA->value(), paramF->value(), paramS->value());
                }
                else
                {
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Focus landmarks",
                                                                      "It is impossible to focus image landmarks: "
                                                                      "landmark is outside image.",
                                                                      ::fwGui::dialog::IMessageDialog::WARNING);
                }
            }
            catch (::fwData::Exception& e )
            {
                ::fwGui::dialog::MessageDialog::showMessageDialog("Focus landmarks",
                                                                  "It is impossible to focus image landmarks: "
                                                                  + std::string(e.what()),
                                                                  ::fwGui::dialog::IMessageDialog::WARNING);
            }

        }
    }
}

//------------------------------------------------------------------------------

void SFocusLandmark::selectLandmark(std::string groupName, size_t index)
{
    m_groupName  = groupName;
    m_index      = index;
    m_isSelected = true;
    this->setIsExecutable(m_isSelected);
}

//------------------------------------------------------------------------------

void SFocusLandmark::deselectLandmark(std::string groupName, size_t index)
{
    if (m_groupName == groupName && m_index == index)
    {
        m_groupName  = "";
        m_index      = 0;
        m_isSelected = false;
    }
    this->setIsExecutable(m_isSelected);
}

//------------------------------------------------------------------------------

void SFocusLandmark::deselectFromGroup(std::string groupName)
{
    if (m_groupName == groupName)
    {
        m_groupName  = "";
        m_index      = 0;
        m_isSelected = false;
    }
    this->setIsExecutable(m_isSelected);
}

//------------------------------------------------------------------------------

void SFocusLandmark::renameGroup(std::string oldGroupName, std::string newGroupName)
{
    if (m_groupName == oldGroupName)
    {
        m_groupName = newGroupName;
    }
}

//------------------------------------------------------------------------------

SFocusLandmark::KeyConnectionsMap SFocusLandmark::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_SELECTED_SIG, s_SELECT_LANDMARK_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_DESELECTED_SIG, s_DESELECT_LANDMARK_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_REMOVED_SIG, s_DESELECT_LANDMARK_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_REMOVED_SIG, s_DESELECT_FROM_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_RENAMED_SIG, s_RENAME_GROUP_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiMeasurement
