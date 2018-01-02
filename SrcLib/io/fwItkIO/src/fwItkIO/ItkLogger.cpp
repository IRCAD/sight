/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
