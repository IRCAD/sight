/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImagesBlend.hpp"

#include "visuVTKAdaptor/Image.hpp"

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

#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include <boost/foreach.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImagesBlend, ::fwData::Composite );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

ImagesBlend::ImagesBlend() throw() : m_imageBlend(nullptr)
{
}

//------------------------------------------------------------------------------

ImagesBlend::~ImagesBlend() throw()
{
    m_imageBlend = nullptr;
}

//------------------------------------------------------------------------------

void ImagesBlend::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Image register is empty", !m_imageRegisterId.empty());
    m_imageBlend = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    doUpdate();
}

//------------------------------------------------------------------------------

void ImagesBlend::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doConfigure() throw(fwTools::Failed)
{
    assert(m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("vtkimageregister") )
    {
        this->setVtkImageRegisterId( m_configuration->getAttributeValue("vtkimageregister") );
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

        typedef std::pair< std::string, std::string > ImagesIdPair;
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
                ::fwRenderVTK::IVtkAdaptorService::sptr imageAdaptor;
                imageAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( img,
                                                                                       "::visuVTKAdaptor::Image");
                imageAdaptor->setRenderService(this->getRenderService());
                imageAdaptor->setRenderId( this->getRenderId() );
                imageAdaptor->setPickerId( this->getPickerId() );
                imageAdaptor->setTransformId( this->getTransformId() );
                imageAdaptor->setAutoRender( this->getAutoRender() );

                ::visuVTKAdaptor::Image::sptr IA;
                IA = ::visuVTKAdaptor::Image::dynamicCast(imageAdaptor);
                IA->setVtkImageRegister(m_imageBlend);
                IA->setImageOpacity(info->m_imageOpacity);
                IA->setAllowAlphaInTF(info->m_useTFAlfa);
                IA->setSelectedTFKey( info->m_selectedTFKey );
                IA->setTFSelectionFwID( info->m_tfSelectionFwID );

                m_registeredImages[img->getID()] = imageAdaptor;
                this->registerService(imageAdaptor);

                imageAdaptor->start();
            }
        }
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


} //namespace visuVTKAdaptor
