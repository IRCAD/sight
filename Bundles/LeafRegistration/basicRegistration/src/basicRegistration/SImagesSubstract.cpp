/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QPushButton>

#include <fwItkIO/itk.hpp>

#include <fwCore/spyLog.hpp>

// Service associated data
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/helper/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

// Services tools
#include <fwServices/macros.hpp>

#include "basicRegistration/SImagesSubstract.hpp"

#include <itkSubtractImageFilter.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using fwTools::fwID;

fwServicesRegisterMacro( ::gui::editor::IEditor, ::basicRegistration::SImagesSubstract, ::fwData::Composite );

namespace basicRegistration
{

SImagesSubstract::SImagesSubstract() noexcept :
    ::gui::editor::IEditor(),
    mpComputeButton(0)
{

}

SImagesSubstract::~SImagesSubstract() noexcept
{
}

//------------------------------------------------------------------------------

void SImagesSubstract::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SImagesSubstract::starting()
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    mpComputeButton = new QPushButton(tr("Compute"), container );
    QObject::connect(mpComputeButton, SIGNAL(clicked( )), this, SLOT(OnCompute()));

    layout->addWidget(mpComputeButton, 0);
    container->setLayout( layout );
}

//------------------------------------------------------------------------------

void SImagesSubstract::stopping()
{
    this->destroy();

}

//------------------------------------------------------------------------------

void SImagesSubstract::updating()
{
    ::fwTools::Type REQUESTED_TYPE = ::fwTools::Type::create("int16");

    const std::string image1Name("image1");
    const std::string image2Name("image2");
    const std::string imageResultName("imageResult");

    ::fwData::Composite::sptr compositeVisu = this->getObject< ::fwData::Composite >();
    ::fwData::Image::sptr image1            = ::fwData::Image::dynamicCast(::fwTools::fwID::getObject(image1Name));
    ::fwData::Image::sptr image2            = ::fwData::Image::dynamicCast(::fwTools::fwID::getObject(image2Name));
    ::fwData::Image::sptr imageResult       = ::fwData::Image::New();

    OSLM_ASSERT("The object " << image1Name << " is not an image", image1);
    OSLM_ASSERT("The object " << image2Name << " is not an image", image2);

    // Test if the both images have the same type and it is signed short.
    bool isSameType =
        ( ((image1->getDataArray())->getType() == (image1->getDataArray())->getType())&&
          ((image1->getDataArray())->getType() == REQUESTED_TYPE));

    if(isSameType)
    {
        // test if the both images have the same size.
        bool isSameSize = (image1->getSize() == image2->getSize());
        if(isSameSize)
        {
            typedef itk::Image< ::boost::int16_t, 3 > ImageType;

            ImageType::Pointer itkImage1 = ::fwItkIO::itkImageFactory< ImageType >( image1 );
            SLM_ASSERT("Unable to convert fwData::Image to itkImage", itkImage1);

            ImageType::Pointer itkImage2 = ::fwItkIO::itkImageFactory< ImageType >( image2 );
            SLM_ASSERT("Unable to convert fwData::Image to itkImage", itkImage2);

            ImageType::Pointer output;

            //Create filter
            typedef ::itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractImageFilterType;
            SubtractImageFilterType::Pointer filter;
            filter = SubtractImageFilterType::New();
            assert(filter);

            filter->SetInput1( itkImage1 );
            filter->SetInput2( itkImage2 );
            filter->Update();
            output = filter->GetOutput();
            assert(output->GetSource());
            ::fwItkIO::dataImageFactory< ImageType >( output, imageResult, true );

            ::fwDataTools::helper::Composite compositeHelper(compositeVisu);
            if ( compositeVisu->find(imageResultName) != compositeVisu->end() )
            {
                compositeHelper.swap(imageResultName, imageResult);
            }
            else
            {
                compositeHelper.add(imageResultName, imageResult);
            }
            compositeHelper.notify();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                              "Both images must have the same size.",
                                                              ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          "Both Images must have signed short as type.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void SImagesSubstract::swapping()
{
    // Classic default approach to update service when oject change
//    this->stopping();
//    this->starting();
    this->updating();

}

//------------------------------------------------------------------------------

void SImagesSubstract::OnCompute()
{
    this->updating();
}

} // namespace basicRegistration

