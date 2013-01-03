/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QPushButton>

#include <itkSubtractImageFilter.h>

#include <itkIO/itk.hpp>

#include <fwCore/spyLog.hpp>

// Service associated data
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/helper/Composite.hpp>


#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

// Services tools
#include <fwServices/Base.hpp>

#include "basicRegistration/SImagesSubstract.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

using fwTools::fwID;

fwServicesRegisterMacro( ::gui::editor::IEditor, ::basicRegistration::SImagesSubstract, ::fwData::Composite );


namespace basicRegistration
{

SImagesSubstract::SImagesSubstract() throw()
: ::gui::editor::IEditor(),
  mpComputeButton(0)
{

}

SImagesSubstract::~SImagesSubstract() throw()
{}

void SImagesSubstract::configuring() throw ( ::fwTools::Failed )
{
    this->initialize();
}

void SImagesSubstract::starting() throw ( ::fwTools::Failed )
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    mpComputeButton           = new QPushButton(tr("Compute"), container );
    QObject::connect(mpComputeButton, SIGNAL(clicked( )), this, SLOT(OnCompute()));

    layout->addWidget(mpComputeButton, 0);
    container->setLayout( layout );
}

void SImagesSubstract::stopping() throw ( ::fwTools::Failed )
{
    QObject::disconnect(mpComputeButton, SIGNAL(clicked( )), this, SLOT(OnCompute()));
    this->getContainer()->clean();
    this->destroy();

}

void SImagesSubstract::updating() throw ( ::fwTools::Failed )
{
    ::fwTools::Type REQUESTED_TYPE = ::fwTools::Type::create("int16");

    const std::string image1Name("image1");
    const std::string image2Name("image2");
    const std::string imageResultName("imageResult");

    ::fwData::Composite::sptr compositeVisu =  this->getObject< ::fwData::Composite >();
    ::fwData::Image::sptr image1 = ::fwData::Image::dynamicCast(::fwTools::fwID::getObject(image1Name));
    ::fwData::Image::sptr image2 = ::fwData::Image::dynamicCast(::fwTools::fwID::getObject(image2Name));
    ::fwData::Image::NewSptr imageResult;

    OSLM_ASSERT("Sorry, " << image1Name << " object is not an image", image1);
    OSLM_ASSERT("Sorry, " << image2Name << " object is not an image", image2);

    // Test if the both images have the same type and it is signed short.
    bool isSameType =( ((image1->getDataArray())->getType() == (image1->getDataArray())->getType())&& ((image1->getDataArray())->getType() == REQUESTED_TYPE));

    if(isSameType)
    {
        // test if the both images have the same size.
        bool isSameSize = (image1->getSize() == image2->getSize());
        if(isSameSize)
        {
            typedef itk::Image< ::boost::int16_t, 3 > ImageType;

            ImageType::Pointer  itkImage1 = ::itkIO::itkImageFactory< ImageType >( image1 ) ;
            SLM_ASSERT("Unable to convert fwData::Image to itkImage", itkImage1);

            ImageType::Pointer  itkImage2 = ::itkIO::itkImageFactory< ImageType >( image2 ) ;
            SLM_ASSERT("Unable to convert fwData::Image to itkImage", itkImage2);

            ImageType::Pointer output;

            //Create filter
            typedef ::itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractImageFilterType;
            SubtractImageFilterType::Pointer filter;
            filter = SubtractImageFilterType::New() ;
            assert(filter);

            filter->SetInput1( itkImage1 );
            filter->SetInput2( itkImage2 );
            filter->Update();
            output = filter->GetOutput();
            assert(output->GetSource());
            ::itkIO::dataImageFactory< ImageType >( output, imageResult, true );

            ::fwComEd::helper::Composite compositeHelper(compositeVisu);
            if ( compositeVisu->find(imageResultName) != compositeVisu->end() )
            {
                compositeHelper.swap(imageResultName, imageResult);
            }
            else
            {
                compositeHelper.add(imageResultName, imageResult);
            }
            compositeHelper.notify(this->getSptr());
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                              "Sorry, Image must have the same size.",
                                                              ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          "Sorry, Image must have a signed short type.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

void SImagesSubstract::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

void SImagesSubstract::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
//    this->stopping();
//    this->starting();
    this->updating();

}

void SImagesSubstract::OnCompute()
{
    this->updating();
}

} // namespace basicRegistration


