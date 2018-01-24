/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QPushButton>

#include <fwItkIO/itk.hpp>

#include <fwCore/spyLog.hpp>

// Service associated data
#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

// Services tools
#include <fwServices/macros.hpp>

#include "basicRegistration/SImagesSubstract.hpp"

#include <itkSubtractImageFilter.h>

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::basicRegistration::SImagesSubstract );

namespace basicRegistration
{

SImagesSubstract::SImagesSubstract() noexcept :
    ::fwGui::editor::IEditor(),
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
    QObject::connect(mpComputeButton, SIGNAL(clicked()), this, SLOT(OnCompute()));

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

    ::fwData::Image::csptr image1     = this->getInput< ::fwData::Image>("image1");
    ::fwData::Image::csptr image2     = this->getInput< ::fwData::Image>("image2");
    ::fwData::Image::sptr imageResult = this->getInOut< ::fwData::Image>("result");

    // Test if the both images have the same type and it is signed short.
    bool isSameType =
        ( ((image1->getDataArray())->getType() == (image2->getDataArray())->getType())&&
          ((image1->getDataArray())->getType() == REQUESTED_TYPE));

    if(isSameType)
    {
        // test if the both images have the same size.
        bool isSameSize = (image1->getSize() == image2->getSize());
        if(isSameSize)
        {
            typedef itk::Image< std::int16_t, 3 > ImageType;

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

            auto sig = imageResult->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
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
    this->updating();
}

//------------------------------------------------------------------------------

void SImagesSubstract::OnCompute()
{
    this->updating();
}
// -----------------------------------------------------------------------------
} // namespace basicRegistration
