/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef IOGDCMQT_DICOMDIREDITOR_HPP
#define IOGDCMQT_DICOMDIREDITOR_HPP

#include <QDialog>
#include <QWidget>
#include <QPointer>
#include <QTreeWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>

#include <boost/filesystem/path.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwServices/IService.hpp>
#include <gui/editor/IDialogEditor.hpp>
#include <fwData/location/ILocation.hpp>

#include <ioGdcmQt/Types.hpp>
#include "ioGdcmQt/config.hpp"

namespace ioGdcm
{

namespace ui
{
/**
 * @class DicomdirEditor
 * @brief Display the  DICOMDIR files content with a tree widget and allows to select a reader to view the selected patient, study or serie.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class IOGDCMQT_CLASS_API DicomdirEditor : public QDialog
{
    Q_OBJECT

public:

    IOGDCMQT_API DicomdirEditor(QWidget* parent, ::boost::filesystem::path& dicomdirPath) throw();
    IOGDCMQT_API virtual  ~DicomdirEditor() throw();
    IOGDCMQT_API void setReader(const std::map < ::ioGdcm::DicomReader, std::string >& readerList);
    IOGDCMQT_API std::pair< ::ioGdcm::DicomReader, std::vector< ::boost::filesystem::path > > show();
    IOGDCMQT_API void setPath (::boost::filesystem::path& path) {m_dicomdirPath  = path;}


private :
    /**
     * @brief create the content of the dialog box (Add a tree widget and a combo box for reader).
     */
    void createContent();
    std::vector< ::boost::filesystem::path> getDicomFiles(QTreeWidgetItem *item);


private:
    typedef std::map<std::string, std::string> DatasetType;
    typedef std::map< std::string, DatasetType > PatientMapType;
    typedef std::map< std::string, DatasetType > StudyMapType;
    typedef std::map< std::string, DatasetType > SeriesMapType;

    typedef std::list <std::string> FileListType;
    typedef std::map< std::string, FileListType > PatientFilesMapType;
    typedef std::map< std::string, FileListType > StudyFilesMapType;
    typedef std::map< std::string, FileListType > SerieFilessMapType;

    typedef std::map< std::string, FileListType > SelectionMapType;
    typedef std::pair < ::ioGdcm::DicomReader, std::string > readerListType;


    QPointer< QTreeWidget > m_DicomdirStructure;
    QPointer< QComboBox > m_readers;
    QPointer< QPushButton > m_open;

    ::boost::filesystem::path m_dicomdirPath;
    bool m_bServiceIsConfigured;

private Q_SLOTS:

    void onSelection();

};
} //namespace ui
} //namespace ioGdcm

#endif // IOGDCMQT_DICOMDIREDITOR_HPP
