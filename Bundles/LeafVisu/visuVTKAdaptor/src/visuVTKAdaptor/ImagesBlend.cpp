/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImagesBlend.hpp"

#include "visuVTKAdaptor/Image.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMath/Compare.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwVtkIO/vtk.hpp>

#include <boost/foreach.hpp>

#include <vtkImageBlend.h>
#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::ImagesBlend, ::fwData::Composite );

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType ImagesBlend::s_CHANGE_MODE_SLOT                  = "changeMode";
const ::fwCom::Slots::SlotKeyType ImagesBlend::s_CHANGE_CHECKERBOARD_DIVISION_SLOT = "changeCheckerboardDivision";

//------------------------------------------------------------------------------

ImagesBlend::ImagesBlend() noexcept :
    m_imageAlgorithm(nullptr),
    m_checkerboardDivision(10)
{
    newSlot(s_CHANGE_MODE_SLOT, &ImagesBlend::changeMode, this);
    newSlot(s_CHANGE_CHECKERBOARD_DIVISION_SLOT, &ImagesBlend::changeCheckerboardDivision, this);
}

//------------------------------------------------------------------------------

ImagesBlend::~ImagesBlend() noexcept
{
    m_imageAlgorithm = nullptr;
}

//------------------------------------------------------------------------------

void ImagesBlend::doStart()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Image register is empty", !m_imageRegisterId.empty());

    // Try to downcast as an vtkImageBlend first
    m_imageAlgorithm = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    if(nullptr == m_imageAlgorithm)
    {
        // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
        vtkImageCheckerboard* imageCheckerboard =
            vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));

        if(nullptr != imageCheckerboard)
        {
            // Set the number of subdivision
            imageCheckerboard->SetNumberOfDivisions(m_checkerboardDivision, m_checkerboardDivision,
                                                    m_checkerboardDivision);

            // Assign as an vtkThreadedImageAlgorithm
            m_imageAlgorithm = imageCheckerboard;
        }
    }

    // If we have a null m_imageAlgorithm, then we have a problem Houston
    SLM_ASSERT("Stored image algorithm doesn't contain a vtkImageBlend or vtkImageCheckerboard",
               nullptr != m_imageAlgorithm);

    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doStop()
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doSwap()
{
    SLM_TRACE_FUNC();
    doUpdate();
}

//------------------------------------------------------------------------------

void ImagesBlend::doUpdate()
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doConfigure()
{
    assert(m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("vtkimageregister"))
    {
        this->setVtkImageRegisterId(m_configuration->getAttributeValue("vtkimageregister"));
    }

    // Get the default division count for checkerboard algorithm
    if(m_configuration->hasAttribute("checkerboardDivision") )
    {
        m_checkerboardDivision = std::stoi(m_configuration->getAttributeValue("checkerboardDivision"));
    }

    std::vector< ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("image");
    SLM_ASSERT("Missing tag 'image' ", !configs.empty());
    for(::fwRuntime::ConfigurationElement::sptr element :  configs)
    {
        SPTR(ImageInfo) info = std::shared_ptr< ImageInfo >(new ImageInfo());
        SLM_ASSERT("Missing attribute 'objectId'", element->hasAttribute("objectId"));
        std::string objectId = element->getAttributeValue("objectId");

        if(element->hasAttribute("opacity") )
        {
            info->m_imageOpacity = ::boost::lexical_cast<double>(element->getAttributeValue("opacity"));
        }
        if(element->hasAttribute("tfalpha") )
        {
            info->m_useTFAlfa = element->getAttributeValue("tfalpha") == "yes";
        }
        if(element->hasAttribute("selectedTFKey") )
        {
            info->m_selectedTFKey = element->getAttributeValue("selectedTFKey");
        }
        if(element->hasAttribute("tfSelectionFwID") )
        {
            info->m_tfSelectionFwID = element->getAttributeValue("tfSelectionFwID");
        }

        m_imageIds.push_back(objectId);
        m_imagesInfo[objectId] = info;
    }
}

//------------------------------------------------------------------------------

bool ImagesBlend::checkImageInformations()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Image::SizeType size;
    ::fwData::Image::SpacingType spacing;
    ::fwData::Image::OriginType origin;

    bool haveSameInfo = true;

    for(std::string id :  m_imageIds)
    {
        ::fwData::Image::csptr img;
        if (!this->isVersion2() && composite->find(id) != composite->end())
        {
            img = ::fwData::Image::dynamicCast((*composite)[id]);
        }
        else
        {
            img = this->getSafeInput< ::fwData::Image >(id);
        }
        if (img && ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img ))
        {
            if (size.empty() && spacing.empty() && origin.empty())
            {
                size    = img->getSize();
                spacing = img->getSpacing();
                origin  = img->getOrigin();
            }
            else
            {
                if (  size != img->getSize() ||
                      !::fwMath::isContainerEqual< const ::fwData::Image::SpacingType >(spacing,
                                                                                        img->getSpacing()) ||
                      !::fwMath::isContainerEqual< const ::fwData::Image::OriginType >(origin,
                                                                                       img->getOrigin()) )
                {
                    OSLM_ERROR("imgA size : " << size[0] << " / " << size[1] << " / "<< size[2] );
                    OSLM_ERROR("imgA spacing : " << spacing[0] << " / " << spacing[1] << " / "<< spacing[2] );
                    OSLM_ERROR("imgA origin : " << origin[0] << " / " << origin[1] << " / "<< origin[2] );

                    OSLM_ERROR(
                        "imgB size : " << img->getSize()[0] << " / " << img->getSize()[1] << " / "<<
                        img->getSize()[2] );
                    OSLM_ERROR(
                        "imgB spacing : " << img->getSpacing()[0] << " / " << img->getSpacing()[1] << " / "<<
                        img->getSpacing()[2] );
                    OSLM_ERROR(
                        "imgB origin : " << img->getOrigin()[0] << " / " << img->getOrigin()[1] << " / "<<
                        img->getOrigin()[2] );

                    haveSameInfo = false;
                    std::string errorMsg = "Warning : images in blend have not the same";
                    errorMsg += (size != img->getSize()) ? " size" : "";
                    errorMsg += (spacing != img->getSpacing()) ? " spacing" : "";
                    errorMsg += (origin != img->getOrigin()) ? " origin" : "";
                    errorMsg += ".\n Background image size, spacing and origin are use.";
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Images blending",
                                                                      errorMsg,
                                                                      ::fwGui::dialog::IMessageDialog::WARNING);
                    break;
                }
            }
        }
    }

    return haveSameInfo;
}

//------------------------------------------------------------------------------

void ImagesBlend::addImageAdaptors()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    this->checkImageInformations();

    int addedImageCount = 0;
    ::fwData::Image::csptr lastValidImage;
    SPTR(ImageInfo) lastValidInfo;

    for(std::string id :  m_imageIds)
    {
        ::fwData::Image::csptr img;
        if (!this->isVersion2() && composite->find(id) != composite->end())
        {
            img = ::fwData::Image::dynamicCast((*composite)[id]);
        }
        else
        {
            img = this->getSafeInput< ::fwData::Image >(id);

        }
        if (img)
        {
            SPTR(ImageInfo) info = m_imagesInfo[id];

            SLM_ASSERT("No image with the id '" << id << "' found", img);

            info->m_connections.disconnect();

            info->m_connections.connect(img, ::fwData::Image::s_MODIFIED_SIG, this->getSptr(), s_UPDATE_SLOT);
            info->m_connections.connect(img, ::fwData::Image::s_BUFFER_MODIFIED_SIG, this->getSptr(),
                                        s_UPDATE_SLOT);

            bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
            if (imageIsValid)
            {
                this->addImage(img, info);

                ++addedImageCount;
                lastValidImage = img;
                lastValidInfo  = info;
            }
        }
    }

    if(addedImageCount == 1 && nullptr != vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId)))
    {
        this->addImage(lastValidImage, lastValidInfo);
    }
}

//------------------------------------------------------------------------------

void ImagesBlend::removeImageAdaptors()
{
    BOOST_REVERSE_FOREACH(std::string id, m_imageIds)
    {
        SPTR(ImageInfo) info = m_imagesInfo[id];
        info->m_connections.disconnect();
    }
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ImagesBlend::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

void ImagesBlend::changeMode(std::string _value, std::string _key)
{
    if( _key == "ImageSource" )
    {
        // Select the right algorithm
        m_imageRegisterId = _value;

        // Try to downcast as an vtkImageBlend first
        m_imageAlgorithm = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

        if(nullptr == m_imageAlgorithm)
        {
            // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
            vtkImageCheckerboard* imageCheckerboard =
                vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));

            if(nullptr != imageCheckerboard)
            {
                // Set the number of subdivision
                imageCheckerboard->SetNumberOfDivisions(m_checkerboardDivision, m_checkerboardDivision,
                                                        m_checkerboardDivision);

                // Assign as an vtkThreadedImageAlgorithm
                m_imageAlgorithm = imageCheckerboard;
            }
        }

        // Update
        this->doUpdate();
    }
}

//------------------------------------------------------------------------------

void ImagesBlend::addImage(::fwData::Image::csptr img, CSPTR(ImageInfo)info)
{
    ::fwRenderVTK::IAdaptor::sptr imageAdaptorService
        = ::fwServices::add< ::fwRenderVTK::IAdaptor >( img, "::visuVTKAdaptor::Image");

    imageAdaptorService->setRenderService(this->getRenderService());
    imageAdaptorService->setRendererId( this->getRendererId() );
    imageAdaptorService->setPickerId( this->getPickerId() );
    imageAdaptorService->setTransformId( this->getTransformId() );
    imageAdaptorService->setAutoRender( this->getAutoRender() );

    ::visuVTKAdaptor::Image::sptr imageAdaptor = ::visuVTKAdaptor::Image::dynamicCast(imageAdaptorService);

    imageAdaptor->setVtkImageRegister(m_imageAlgorithm);
    imageAdaptor->setImageOpacity(info->m_imageOpacity);
    imageAdaptor->setAllowAlphaInTF(info->m_useTFAlfa);
    imageAdaptor->setSelectedTFKey( info->m_selectedTFKey );
    imageAdaptor->setTFSelectionFwID( info->m_tfSelectionFwID );

    m_registeredImages[img->getID()] = imageAdaptorService;
    this->registerService(imageAdaptorService);

    imageAdaptorService->start();
}

//------------------------------------------------------------------------------

void ImagesBlend::changeCheckerboardDivision(const int division)
{
    m_checkerboardDivision = division;

    // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
    vtkImageCheckerboard* imageCheckerboard =
        vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    if(nullptr != imageCheckerboard)
    {
        // Set the number of subdivision
        imageCheckerboard->SetNumberOfDivisions(m_checkerboardDivision, m_checkerboardDivision,
                                                m_checkerboardDivision);

        // Assign as an vtkThreadedImageAlgorithm
        m_imageAlgorithm = imageCheckerboard;
    }

    this->doUpdate();
}

} //namespace visuVTKAdaptor
