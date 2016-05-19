/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTracking/SArucoTrackerEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

namespace uiTracking
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTracking::SArucoTrackerEditor, ::fwData::Object);
//------------------------------------------------------------------------
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_METHOD_CHANGED_SIG       = "methodChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_CORNERMETHOD_CHANGED_SIG = "cornerMethodChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_CONSTANT_CHANGED_SIG     = "constantChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_BLOCKSIZE_CHANGED_SIG    = "blockSizeChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_BORDERWIDTH_CHANGED_SIG  = "borderWidthChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_PATTERNWIDTH_CHANGED_SIG = "patternWidthChanged";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_TAGS_DISPLAYED_SIG       = "tagsDisplayed";
const ::fwCom::Signals::SignalKeyType SArucoTrackerEditor::s_SPEED_CHANGED_SIG        = "speedChanged";

//------------------------------------------------------------------------------

SArucoTrackerEditor::SArucoTrackerEditor() throw()
{
    m_sigMethodChanged = MethodChangedSignalType::New();
    m_signals( s_METHOD_CHANGED_SIG,  m_sigMethodChanged);

    m_sigCornerMethodChanged = CornerMethodChangedSignalType::New();
    m_signals( s_CORNERMETHOD_CHANGED_SIG,  m_sigCornerMethodChanged);

    m_sigConstantChanged = ConstantChangedSignalType::New();
    m_signals( s_CONSTANT_CHANGED_SIG,  m_sigConstantChanged);

    m_sigBlockSizeChanged = BlockSizeChangedSignalType::New();
    m_signals( s_BLOCKSIZE_CHANGED_SIG,  m_sigBlockSizeChanged);

    m_sigBorderWidthChanged = BorderWidthChangedSignalType::New();
    m_signals( s_BORDERWIDTH_CHANGED_SIG,  m_sigBorderWidthChanged);

    m_sigPatternWidthChanged = PatternWidthChangedSignalType::New();
    m_signals( s_PATTERNWIDTH_CHANGED_SIG,  m_sigPatternWidthChanged);

    m_sigTagsDisplayed = TagsDisplayedSignalType::New();
    m_signals( s_TAGS_DISPLAYED_SIG,  m_sigTagsDisplayed);

    m_sigSpeedChanged = SpeedChangedSignalType::New();
    m_signals( s_SPEED_CHANGED_SIG,  m_sigSpeedChanged);


}

//------------------------------------------------------------------------------

SArucoTrackerEditor::~SArucoTrackerEditor() throw()
{
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::starting() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* container = qtContainer->getQtContainer();

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    container->setLayout(mainLayout);

    QBoxLayout* titleLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    QLabel* title           = new QLabel("Aruco Parameters");
    titleLayout->addWidget(title);

    QVBoxLayout* widgetsLayout = new QVBoxLayout();

    m_methodBox      = new QComboBox();
    m_cornerBox      = new QComboBox();
    m_speedBox       = new QComboBox();
    m_blockSizeSB    = new QDoubleSpinBox();
    m_constantSB     = new QDoubleSpinBox();
    m_borderWidthSB  = new QDoubleSpinBox();
    m_patternWidthSB = new QDoubleSpinBox();
    m_displayTags    = new QCheckBox("Display Tags");

    m_displayTags->setChecked(true);

    QLabel* methodLabel       = new QLabel("Choose Method : ");
    QLabel* cornerLabel       = new QLabel("Choose Method For Corner Refinement : ");
    QLabel* speedLabel        = new QLabel("Choose Speed : ");
    QLabel* constantLabel     = new QLabel("Constant : ");
    QLabel* blockSizeLabel    = new QLabel("Block Size : ");
    QLabel* borderWidthLabel  = new QLabel("Border Width : ");
    QLabel* patternWidthLabel = new QLabel("Pattern Width : ");
    QLabel* debugLabel        = new QLabel("Debug mode : ");

    m_methodBox->insertItem(0,"ADPT_THRES");
    m_methodBox->insertItem(1,"FIXED_THRES");
    m_methodBox->insertItem(2,"CANNY");

    m_cornerBox->insertItem(0,"NONE");
    m_cornerBox->insertItem(1,"HARRYS");
    m_cornerBox->insertItem(2,"SUBPIX");
    m_cornerBox->insertItem(3,"LINES");

    m_speedBox->insertItem(0,"Low");
    m_speedBox->insertItem(1,"Medium low");
    m_speedBox->insertItem(2,"Medium fast");
    m_speedBox->insertItem(3,"Fast");

    widgetsLayout->addWidget(methodLabel);
    widgetsLayout->addWidget(m_methodBox);

    widgetsLayout->addWidget(cornerLabel);
    widgetsLayout->addWidget(m_cornerBox);

    widgetsLayout->addWidget(speedLabel);
    widgetsLayout->addWidget(m_speedBox);

    widgetsLayout->addWidget(constantLabel);
    widgetsLayout->addWidget(m_constantSB);

    widgetsLayout->addWidget(blockSizeLabel);
    widgetsLayout->addWidget(m_blockSizeSB);

    widgetsLayout->addWidget(borderWidthLabel);
    widgetsLayout->addWidget(m_borderWidthSB);

    widgetsLayout->addWidget(patternWidthLabel);
    widgetsLayout->addWidget(m_patternWidthSB);

    widgetsLayout->addWidget(debugLabel);
    widgetsLayout->addWidget(m_displayTags);


    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(widgetsLayout);

    container->setLayout(mainLayout);

    QObject::connect(m_methodBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMethodChanged(int)));
    QObject::connect(m_cornerBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCornerMethodChanged(int)));
    QObject::connect(m_constantSB, SIGNAL(valueChanged(double)), this, SLOT(onConstantChanged(double)));
    QObject::connect(m_blockSizeSB, SIGNAL(valueChanged(double)), this, SLOT(onBlockSizeChanged(double)));
    QObject::connect(m_borderWidthSB, SIGNAL(valueChanged(double)), this, SLOT(onBorderWidthChanged(double)));
    QObject::connect(m_patternWidthSB, SIGNAL(valueChanged(double)), this, SLOT(onPatternWidthChanged(double)));

    QObject::connect(m_displayTags, SIGNAL(stateChanged(int)), this, SLOT(onTagsDisplayed(int)));
    QObject::connect(m_speedBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpeedChanged(int)));

    this->updating();


}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::configuring() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::stopping() throw (fwTools::Failed)
{
    QObject::disconnect(m_methodBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMethodChanged(int)));
    QObject::disconnect(m_cornerBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCornerMethodChanged(int)));
    QObject::disconnect(m_constantSB, SIGNAL(valueChanged(double)), this, SLOT(onConstantChanged(double)));
    QObject::disconnect(m_blockSizeSB, SIGNAL(valueChanged(double)), this, SLOT(onBlockSizeChanged(double)));
    QObject::disconnect(m_borderWidthSB, SIGNAL(valueChanged(double)), this, SLOT(onBorderWidthChanged(double)));
    QObject::disconnect(m_patternWidthSB, SIGNAL(valueChanged(double)), this, SLOT(onPatternWidthChanged(double)));
    QObject::disconnect(m_displayTags, SIGNAL(stateChanged(int)), this, SLOT(onTagsDisplayed(int)));
    QObject::disconnect(m_speedBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpeedChanged(int)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::updating() throw (fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::onMethodChanged(int value)
{
    if(value >=0)
    {
        unsigned int i = value;
        m_sigMethodChanged->asyncEmit(i);
    }
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::onConstantChanged(double value)
{
    //Emit
    m_sigConstantChanged->asyncEmit (value);
}

//------------------------------------------------------------------------------
void SArucoTrackerEditor::onBlockSizeChanged(double value)
{
    //Notify
    m_sigBlockSizeChanged->asyncEmit(value);
}

//------------------------------------------------------------------------------
void SArucoTrackerEditor::onBorderWidthChanged(double value)
{
    //Notify
    m_sigBorderWidthChanged->asyncEmit (value);
}

//------------------------------------------------------------------------------
void SArucoTrackerEditor::onPatternWidthChanged(double value)
{
    //Notify
    m_sigPatternWidthChanged->asyncEmit(value);
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::onCornerMethodChanged(int value)
{
    if(value >=0)
    {
        unsigned int i = value;
        m_sigCornerMethodChanged->asyncEmit(i);
    }
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::onSpeedChanged(int value)
{
    if(value >= 0)
    {
        unsigned int i = value;
        m_sigSpeedChanged->asyncEmit(i);
    }
}

//------------------------------------------------------------------------------

void SArucoTrackerEditor::onTagsDisplayed(int value)
{
    m_sigTagsDisplayed->asyncEmit((value == 2));
}

//------------------------------------------------------------------------------
} // namespace uiTracking
