/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "opImageFilter/SImageExtruder.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <imageFilterOp/ImageExtruder.hpp>

namespace opImageFilter
{

static const std::string s_MESHES_INPUT = "meshes";
static const std::string s_IMAGE_INPUT  = "image";

static const std::string s_IMAGE_INOUT = "extrudedImage";

static const ::fwCom::Slots::SlotKeyType s_ADD_RECONSTRUCTIONS_SLOT = "addReconstructions";

//------------------------------------------------------------------------------

SImageExtruder::SImageExtruder()
{
    newSlot(s_ADD_RECONSTRUCTIONS_SLOT, &SImageExtruder::addReconstructions, this);
}

//------------------------------------------------------------------------------

SImageExtruder::~SImageExtruder()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::configuring()
{

}

//------------------------------------------------------------------------------

void SImageExtruder::starting()
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageExtruder::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_ADD_RECONSTRUCTIONS_SLOT);
    connections.push(s_MESHES_INPUT, ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);

    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SImageExtruder::updating()
{
    const ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    SLM_ASSERT("Input '" + s_IMAGE_INPUT + "' does not exist.", image);
    ::fwData::mt::ObjectReadLock imageLock(image);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Copy the image into the output.
        const ::fwData::Image::sptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
        {
            const ::fwData::mt::ObjectWriteLock imageOutLock(imageOut);

            SLM_ASSERT("The image must be in 3 dimensions", image->getNumberOfDimensions() == 3);

            imageOut->deepCopy(image);

            const auto sig = imageOut->signal< ::fwData::Image::ModifiedSignalType>(::fwData::Image::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        imageLock.unlock();

        const ::fwMedData::ModelSeries::csptr meshes = this->getInput< ::fwMedData::ModelSeries >(s_MESHES_INPUT);
        SLM_ASSERT("Input '" + s_MESHES_INPUT + "' does not exist.", meshes);
        ::fwData::mt::ObjectReadLock meshLock(meshes);

        ::fwMedData::ModelSeries::ReconstructionVectorType reconstructions = meshes->getReconstructionDB();

        this->addReconstructions(reconstructions);
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::stopping()
{
}

//------------------------------------------------------------------------------

void SImageExtruder::addReconstructions(::fwMedData::ModelSeries::ReconstructionVectorType _reconstructions) const
{
    const ::fwData::Image::csptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
    ::fwData::mt::ObjectReadLock imageOutLock(imageOut);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(imageOut))
    {
        imageOutLock.unlock();

        for(const ::fwData::Reconstruction::csptr reconstruction : _reconstructions)
        {
            ::fwData::mt::ObjectReadLock reconstructionLock(reconstruction);

            const ::fwData::Mesh::csptr mesh = reconstruction->getMesh();
            ::fwData::mt::ObjectReadLock meshLock(mesh);
            this->extrudeMesh(mesh);
        }
    }
}

//------------------------------------------------------------------------------

void SImageExtruder::extrudeMesh(const ::fwData::Mesh::csptr _mesh) const
{
    const ::fwData::Image::sptr imageOut = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Inout '" + s_IMAGE_INOUT + "' does not exist.", imageOut);
    const ::fwData::mt::ObjectWriteLock imageOutLock(imageOut);

    // Extrude the image.
    ::imageFilterOp::ImageExtruder::extrude(imageOut, _mesh);

    // Send signals.
    const auto sig =
        imageOut->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

} // namespace opImageFilter.
