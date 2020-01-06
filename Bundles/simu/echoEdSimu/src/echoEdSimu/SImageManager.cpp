/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "echoEdSimu/SImageManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/vtk.hpp>

#include <filesystem>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SImageManager, ::fwData::Composite );

const std::string s_ctPath   = "./Data/CTImages";
const std::string s_echoPath = "./Data/EchoImages";

const ::fwCom::Slots::SlotKeyType SImageManager::s_UPDATE_SINGLE_SLOT = "updateSingle";
const ::fwCom::Slots::SlotKeyType SImageManager::s_UPDATE_BOTH_SLOT   = "updateBoth";

const std::string s_ctImageKey   = "echoCTImage";
const std::string s_echoImageKey = "echoImage";

const int s_minI = 95;
const int s_maxI = 194;
const int s_minJ = 160;
const int s_maxJ = 259;

//-----------------------------------------------------------------------------

SImageManager::SImageManager() noexcept :
    m_cranioCaudalIndex(s_minI),
    m_radialIndex(s_minJ)
{
    newSlot( s_UPDATE_SINGLE_SLOT, &SImageManager::updateSingle, this );
    newSlot( s_UPDATE_BOTH_SLOT, &SImageManager::updateBoth, this );
}

//------------------------------------------------------------------------------

SImageManager::~SImageManager() noexcept
{
}

//------------------------------------------------------------------------------

void SImageManager::configuring()
{
}

//------------------------------------------------------------------------------

void SImageManager::starting()
{
    m_ctImage   = this->getInOut< ::fwData::Image >(s_ctImageKey);
    m_echoImage = this->getInOut< ::fwData::Image >(s_echoImageKey);
}

//------------------------------------------------------------------------------

void SImageManager::stopping()
{
}

//------------------------------------------------------------------------------

void SImageManager::updateSingle(int i, std::string key)
{
    if(key == "craniocaudal")
    {
        this->updateBoth(i, m_radialIndex);
    }
    else if(key == "radial")
    {
        this->updateBoth(m_cranioCaudalIndex, i);
    }
}

//------------------------------------------------------------------------------

void SImageManager::load(std::filesystem::path dir, ImagesVecType& images)
{
    unsigned int dI = s_maxI - s_minI +1;
    unsigned int dJ = s_maxJ - s_minJ +1;

    images.resize(dI);

    for (unsigned int i = 0; i < dI; ++i)
    {
        images[i].resize(dJ);
        const unsigned int idI = i + s_minI;
        for (unsigned int j = 0; j < dJ; ++j)
        {
            const unsigned int idJ = j + s_minJ;
            std::stringstream str;
            str << "image_" << idI << "_" << idJ << ".vtk";
            std::filesystem::path path(dir / str.str());

            if (std::filesystem::exists(path))
            {
                ::fwData::Image::sptr image         = ::fwData::Image::New();
                ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();
                reader->setFile(path);
                reader->setObject(image);
                reader->read();

                images[i][j] = image;
            }
        }
    }
}

//------------------------------------------------------------------------------

void SImageManager::updating()
{
}

//------------------------------------------------------------------------------

void SImageManager::swapping()
{
}

//------------------------------------------------------------------------------

void SImageManager::updateBoth(int i, int j)
{
    if (i < s_minI || i > s_maxI || j < s_minJ || j > s_maxJ)
    {
        OSLM_ERROR("Image [" << i - s_minI << "][" << j - s_minJ <<"] index out of bound");
        return;
    }
    m_cranioCaudalIndex = i;
    m_radialIndex       = j;

    std::stringstream str;
    str << "image_" << m_cranioCaudalIndex << "_" << m_radialIndex << ".vtk";
    std::filesystem::path path1(s_ctPath + "/" + str.str());
    std::filesystem::path path2(s_echoPath + "/" + str.str());

    ::fwVtkIO::ImageReader::sptr reader1 = ::fwVtkIO::ImageReader::New();
    reader1->setFile(path1);
    reader1->setObject(m_ctImage);
    reader1->read();

    ::fwVtkIO::ImageReader::sptr reader2 = ::fwVtkIO::ImageReader::New();
    reader2->setFile(path2);
    reader2->setObject(m_echoImage);
    reader2->read();

    auto sig1 = m_ctImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig1->asyncEmit();

    auto sig2 = m_echoImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace echoEdSimu
