/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_PROGRESSVTK2FW_HPP_
#define _VTKIO_PROGRESSVTK2FW_HPP_

#include <vtkAlgorithm.h>
#include <vtkCommand.h>

#include <fwTools/ProgressAdviser.hpp>
#include "vtkIO/config.hpp"



namespace vtkIO
{


class VTKIO_CLASS_API ProgressVtktoFw
{
public:

    /// observed send vtkProgressEvent , observer catch them then forward to its slot
    VTKIO_API ProgressVtktoFw( vtkAlgorithm *observed, ::fwTools::ProgressAdviser *observer, std::string message  );

    /// remove the observation
    VTKIO_API ~ProgressVtktoFw();

protected :
    vtkAlgorithm *m_algo;
    vtkCommand   *m_cmd;

};



} // namespace vtkIO

#endif // _VTKIO_PROGRESSVTK2FW_HPP_
