/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVtkIO/config.hpp"

#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{
class ModelSeries;
}

namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write a ::fwData::Reconstruction.
 *
 * Write a ::fwData::Reconstruction as .obj file using the VTK lib and the obj format.
 */
class ModelSeriesObjWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (ModelSeriesObjWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >),
        (()),
        ::fwDataIO::writer::factory::New< ModelSeriesObjWriter >
        );

    fwCoreAllowSharedFromThis();

    //! Constructor.
    FWVTKIO_API ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! Destructor.
    FWVTKIO_API ~ModelSeriesObjWriter();

    //! Writing operator.
    FWVTKIO_API void write() override;

    /**
     * @return ".obj"
     */
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace fwVtkIO
