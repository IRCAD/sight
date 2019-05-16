/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/SImagesProbeCursor.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImagesProbeCursor);

#define START_PROBE_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_PROBE_EVENT  vtkCommand::LeftButtonReleaseEvent

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_GROUP = "image";

//------------------------------------------------------------------------------

SImagesProbeCursor::SImagesProbeCursor() noexcept
{
    m_priority = 0.61f; // must be greater than SProbeCursor to be visible
}

//------------------------------------------------------------------------------

SImagesProbeCursor::~SImagesProbeCursor() noexcept
{
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::configuring()
{
    this->SProbeCursor::configuring();

    const ConfigType config = this->getConfigTree().get_child("inout");

    SLM_ASSERT("configured group must be '" + s_IMAGE_GROUP + "'",
               config.get<std::string>("<xmlattr>.group", "") == s_IMAGE_GROUP);

    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type& v, config.equal_range("key"))
    {
        const ::fwServices::IService::ConfigType& specAssoc = v.second;
        const ::fwServices::IService::ConfigType& attr      = specAssoc.get_child("<xmlattr>");
        const std::string name                              = attr.get("name", "");
        m_imagesNames.push_back(name);
    }
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::starting()
{
    this->SProbeCursor::starting();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::updating()
{
    ::fwData::Image::sptr firstImage = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, 0);
    if (firstImage)
    {
        m_helper.updateImageInfos(firstImage);
        this->setVisibility(false);
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::stopping()
{
    this->SProbeCursor::stopping();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::updateView( double world[3] )
{
    const size_t nbImages = this->getKeyGroupSize(s_IMAGE_GROUP);
    SLM_ASSERT("There must be as much images as names", nbImages == m_imagesNames.size());

    std::stringstream txt;

    ::fwData::Image::sptr firstImage = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, 0);
    if (firstImage)
    {
        if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(firstImage))
        {
            m_helper.updateImageInfos(firstImage);

            int index[3];
            m_helper.worldToImageSliceIndex( world, index );
            OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] );

            if (    world[0] < firstImage->getOrigin()[0] ||
                    world[1] < firstImage->getOrigin()[1] ||
                    world[2] < firstImage->getOrigin()[2] ||
                    index[0] < 0 || index[1] < 0 || index[2] < 0 ||
                    static_cast<size_t>(index[0]) >= firstImage->getSize()[0] ||
                    static_cast<size_t>(index[1]) >= firstImage->getSize()[1] ||
                    static_cast<size_t>(index[2]) >= firstImage->getSize()[2]
                    )
            {
                txt << "(---,---,---)" << std::endl;
            }
            else
            {
                ::fwDataTools::helper::Image imageHelper(firstImage);
                txt << (::boost::format("(% 4li,% 4li,% 4li)") % index[0] % index[1] % index[2] ).str() << std::endl;

                // update polyData
                double worldCross[4][3];
                this->computeCrossExtremity(firstImage, index, worldCross);

                vtkPoints* points = m_cursorPolyData->GetPoints();
                for ( int i = 0; i < 4; ++i)
                {
                    OSLM_TRACE("p=" << worldCross[i][0] << "," << worldCross[i][2] << "," << worldCross[i][2] << "," );
                    points->SetPoint(i, worldCross[i]);
                }
                points->Modified();
            }
        }
    }

    for(size_t i = 0; i < nbImages; ++i)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, i);
        if (image)
        {
            if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
            {
                ::fwDataTools::helper::Image imageHelper(image);
                m_helper.updateImageInfos(image);

                int index[3];
                m_helper.worldToImageSliceIndex( world, index );
                OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] << "," );

                if ( !( world[0] < image->getOrigin()[0] ||
                        world[1] < image->getOrigin()[1] ||
                        world[2] < image->getOrigin()[2]  ||
                        index[0] < 0 || index[1] < 0 || index[2] < 0 ||
                        static_cast<size_t>(index[0]) >= image->getSize()[0] ||
                        static_cast<size_t>(index[1]) >= image->getSize()[1] ||
                        static_cast<size_t>(index[2]) >= image->getSize()[2])
                     )
                {
                    std::string greyLevel = imageHelper.getPixelAsString(static_cast<size_t>(index[0]),
                                                                         static_cast<size_t>(index[1]),
                                                                         static_cast<size_t>(index[2]) );
                    txt << m_imagesNames[i] << " : " << greyLevel << std::endl;
                }
            }
        }
    }
    m_textMapper->SetInput(txt.str().c_str());

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImagesProbeCursor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_GROUP, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_GROUP, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_GROUP, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} //namespace visuVTKAdaptor
