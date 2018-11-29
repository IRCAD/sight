/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <fwCore/base.hpp>

#include <itkOutputWindow.h>

/**
 * @brief Outputs ITK messages to the fw4spl log.
 */
class ItkLogger : public ::itk::OutputWindow
{
public:
    typedef ItkLogger Self;
    typedef ::itk::SmartPointer<Self> Pointer;

    itkTypeMacro(ItkLogger, ::itk::Object);

    static Pointer New();

    virtual void DisplayText(const char* _txt) override;
    virtual void DisplayErrorText(const char* _txt) override;
    virtual void DisplayWarningText(const char* _txt) override;
    virtual void DisplayDebugText(const char* _txt) override;

private:
    ItkLogger();
    virtual ~ItkLogger();
};

//------------------------------------------------------------------------------

ItkLogger::Pointer ItkLogger::New()
{
    return ItkLogger::Pointer(new ItkLogger());
}

//------------------------------------------------------------------------------

ItkLogger::ItkLogger()
{

}

//------------------------------------------------------------------------------

ItkLogger::~ItkLogger()
{

}

//------------------------------------------------------------------------------

void ItkLogger::DisplayText(const char* _txt)
{
    OSLM_INFO("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayErrorText(const char* _txt)
{
    OSLM_ERROR("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayWarningText(const char* _txt)
{
    OSLM_WARN("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayDebugText(const char* _txt)
{
    OSLM_DEBUG("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

struct LoggerInstantiator
{
    LoggerInstantiator()
    {
        ItkLogger::Pointer logger = ItkLogger::New();
        ::itk::OutputWindow::SetInstance(logger);
        logger->Delete();
    }
};

//------------------------------------------------------------------------------

static LoggerInstantiator instantiator;
