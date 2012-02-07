/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <QApplication>
#include <QTextBrowser>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>

#include <QTextDocumentWriter>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextTableFormat>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/macros.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include "uiGenericQt/action/ShowRevInfo.hpp"

namespace uiGeneric
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::uiGeneric::action::ShowRevInfo , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

ShowRevInfo::ShowRevInfo( ) throw()
{}

//------------------------------------------------------------------------------

ShowRevInfo::~ShowRevInfo() throw()
{}

//------------------------------------------------------------------------------

void ShowRevInfo::info(std::ostream &_sstream )
{
    _sstream << "ShowRevInfo" << std::endl;
}

//------------------------------------------------------------------------------
void ShowRevInfo::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------
void ShowRevInfo::updating( ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Revision info"));

    QTextBrowser * htmlView = new QTextBrowser (dialog);
    htmlView->setDocument(this->generateReport());
    htmlView->setOpenExternalLinks(true);
    htmlView->setMinimumSize(800, 600);

    QPushButton* okButton = new QPushButton(QObject::tr("Ok"));
    QPushButton* genButton = new QPushButton(QObject::tr("Save report"));
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(genButton);
    hLayout->addWidget(okButton);
    hLayout->setContentsMargins(5,5,5,5);

    QFrame* line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(htmlView, 0);
    layout->addWidget(line, 0);
    layout->addLayout(hLayout, 0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    dialog->setLayout( layout );

    QObject::connect(genButton, SIGNAL(clicked()), this, SLOT(saveReport()));
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    dialog->exec();
}

//------------------------------------------------------------------------------

void ShowRevInfo::starting() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowRevInfo::updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void ShowRevInfo::stopping() throw (::fwTools::Failed)
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

QTextDocument* ShowRevInfo::generateReport()
{
    QTextDocument* document = new QTextDocument();
    QTextCursor cursor(document);

    QTextCharFormat defaultFormat;
    defaultFormat.setFontPointSize(12);

    QTextCharFormat boldFormat = defaultFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat titleFormat = boldFormat;

    titleFormat.setFontPointSize(20);

    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignHCenter);

    cursor.setBlockFormat(centerFormat);
    cursor.insertText(QString("Revision info"), titleFormat);
    cursor.insertBlock();
    cursor.insertBlock();

    QTextBlockFormat leftFormat;
    leftFormat.setAlignment(Qt::AlignLeft);
    cursor.setBlockFormat(leftFormat);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setMargin(0);
    tableFormat.setBorder(1);
    tableFormat.setPadding(0);
    tableFormat.setHeaderRowCount(0);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    ::boost::filesystem::path bundlePath = ::boost::filesystem::current_path() / "Bundles";
    ::boost::filesystem::path libPath    = ::boost::filesystem::current_path() / "share";
    const FindMapType& bundleMap = this->findRevInfo(bundlePath);
    const FindMapType& libMap =  this->findRevInfo(libPath);

    RevMapType revMap;
    this->getRevMap(bundleMap, revMap, true);
    this->getRevMap(libMap, revMap, false);

    cursor.movePosition(QTextCursor::EndOfBlock);
    cursor.movePosition(QTextCursor::NextBlock);

    cursor.insertBlock();
    cursor.insertTable( revMap.size()+1, 3, tableFormat);

    cursor.setBlockFormat(centerFormat);
    cursor.insertText(tr("Revision"), boldFormat);
    cursor.movePosition(QTextCursor::NextCell);

    cursor.setBlockFormat(centerFormat);
    cursor.insertText(tr("Bundle"), boldFormat);
    cursor.movePosition(QTextCursor::NextCell);

    cursor.setBlockFormat(centerFormat);
    cursor.insertText(tr("Lib"), boldFormat);
    cursor.movePosition(QTextCursor::NextCell);

    BOOST_FOREACH(RevMapType::value_type val, revMap)
    {
        cursor.insertHtml(QString::fromStdString(val.first).replace("\n","<br/>"));
        cursor.movePosition(QTextCursor::NextCell);
        cursor.setBlockFormat(centerFormat);
        BOOST_FOREACH(std::string name, val.second.first)
        {
            cursor.insertHtml(QString::fromStdString("<br/>" + name));
        }

        cursor.movePosition(QTextCursor::NextCell);
        cursor.setBlockFormat(centerFormat);
        BOOST_FOREACH(std::string name, val.second.second)
        {
            cursor.insertHtml(QString::fromStdString("<br/>" + name));
        }
        cursor.movePosition(QTextCursor::NextCell);
    }
    return document;
}

//------------------------------------------------------------------------------

const ShowRevInfo::FindMapType ShowRevInfo::findRevInfo(const ::boost::filesystem::path& findPath)
{
    FindMapType findMap;

    std::string filename;
    std::string findname;
    for (::boost::filesystem::recursive_directory_iterator iter(findPath), end; iter != end; ++iter)
    {
#if BOOST_FILESYSTEM_VERSION > 2
        filename = iter->path().filename().string();
#else
        filename =  iter->path().leaf();
#endif
        if(filename == "rev.info")
        {
#if BOOST_FILESYSTEM_VERSION > 2
            findname = iter->path().parent_path().filename().string();
#else
            findname = iter->path().leaf();
#endif
            findname = iter->path().parent_path().filename().string();
            findMap[findname] = iter->path();
        }
    }
    return findMap;
}

//------------------------------------------------------------------------------


void ShowRevInfo::getRevMap(const ShowRevInfo::FindMapType& findMap, ShowRevInfo::RevMapType& revMap, bool isBundle)
{
    BOOST_FOREACH(FindMapType::value_type val, findMap)
    {
        size_t length;
        std::string buf;
        std::ifstream file;
        file.open(val.second.string().c_str(), std::ios::binary );

        if (!file.is_open())
        {
            OSLM_ERROR( "RevInfo file loading error for " << val.second.string());
            throw std::ios_base::failure("Unable to open " + val.second.string());
        }

        file.seekg (0, std::ios::end);
        length = file.tellg();
        file.seekg (0, std::ios::beg);
        buf.resize(length);
        char *buffer = &buf[0];

        file.read (buffer, length);
        file.close();

        if(isBundle)
        {
            revMap[buf].first.insert(val.first);
        }
        else
        {
            revMap[buf].second.insert(val.first);
        }
    }
}

//------------------------------------------------------------------------------

void ShowRevInfo::saveReport()
{
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a location to save revision info report");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("ODT","*.odt");
    dialogFile.addFilter("HTML","*.html");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if( result )
    {
        ::boost::filesystem::path filename = result->getPath();
        _sDefaultPath = filename.parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        
        QTextDocument* document = this->generateReport();

        QTextDocumentWriter writer(QString::fromStdString(filename.string()));
        if( ::boost::filesystem::extension(filename) == ".odt" )
        {
            writer.setFormat("ODF");
        }
        else
        {
            writer.setFormat("HTML");
        }
        writer.write(document);
        delete document;
    }
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace uiGeneric
