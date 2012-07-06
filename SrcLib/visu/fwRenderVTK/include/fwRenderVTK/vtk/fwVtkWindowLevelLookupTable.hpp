#ifndef __FWRENDERVTK_FWVTKWINDOWLEVELLOOKUPTABLE_HPP__
#define __FWRENDERVTK_FWVTKWINDOWLEVELLOOKUPTABLE_HPP__

#include <vtkLookupTable.h>

#include "fwRenderVTK/config.hpp"

///Reinplementation of vtkWindowLevelLookupTable : add specific out-of-bounds colors
class FWRENDERVTK_API fwVtkWindowLevelLookupTable : public vtkLookupTable
{
public:
  static fwVtkWindowLevelLookupTable *New();
  vtkTypeMacro(fwVtkWindowLevelLookupTable,vtkLookupTable);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the window for the lookup table.  The window is the difference
  // between TableRange[0] and TableRange[1].
  void SetWindow(double window) {
    this->InverseVideo = window < 0;
    if(this->InverseVideo)
    {
        window = -window;
    }
    if (window < 1e-5) { window = 1e-5; }
    this->Window = window;
    this->SetTableRange(this->Level - this->Window/2.0,
                        this->Level + this->Window/2.0); };
  vtkGetMacro(Window,double);

  void BuildInvert();

  // Description:
  // Set the Level for the lookup table.  The level is the average of
  // TableRange[0] and TableRange[1].
  void SetLevel(double level) {
    this->Level = level;
    this->SetTableRange(this->Level - this->Window/2.0,
                        this->Level + this->Window/2.0); };
  vtkGetMacro(Level,double);

  // Description:
  // Set  on or off.  You can achieve the same effect by
  // switching the LeftClampValue and the RightClampValue.
  vtkSetMacro(Clamping,int);
  vtkGetMacro(Clamping,int);
  vtkBooleanMacro(Clamping,int);

  // Description:
  // Set the minimum table value.  All lookup table entries below the
  // start of the ramp will be set to this color.  After you change
  // this value, you must re-build the lookup table.
  vtkSetVector4Macro(LeftClampValue,double);
  vtkGetVector4Macro(LeftClampValue,double);

  // Description:
  // Set the maximum table value. All lookup table entries above the
  // end of the ramp will be set to this color.  After you change
  // this value, you must re-build the lookup table.
  vtkSetVector4Macro(RightClampValue,double);
  vtkGetVector4Macro(RightClampValue,double);

  unsigned char *GetCurrentPointer(const vtkIdType id);


  // Description:
  // map a set of scalars through the lookup table
  void MapScalarsThroughTable2(void *input, unsigned char *output,
                               int inputDataType, int numberOfValues,
                               int inputIncrement, int outputIncrement);



protected:
  fwVtkWindowLevelLookupTable(int sze=256, int ext=256);
  ~fwVtkWindowLevelLookupTable();

  double Window;
  double Level;
  int InverseVideo;

  int Clamping;
  double RightClampValue[4];
  double LeftClampValue[4];


  vtkUnsignedCharArray *InvertTable;
  vtkTimeStamp InvertTime;

private:
  fwVtkWindowLevelLookupTable(const fwVtkWindowLevelLookupTable&);  // Not implemented.
  void operator=(const fwVtkWindowLevelLookupTable&);  // Not implemented.
};

#endif //__FWRENDERVTK_FWVTKWINDOWLEVELLOOKUPTABLE_HPP__


