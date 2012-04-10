#include <iostream>
#include <cmath>

#include <vtkBitArray.h>
#include <vtkObjectFactory.h>
#include <vtkMath.h>

#include "fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp"

vtkStandardNewMacro(fwVtkWindowLevelLookupTable);

//----------------------------------------------------------------------------
    fwVtkWindowLevelLookupTable::fwVtkWindowLevelLookupTable(int sze, int ext)
: vtkLookupTable(sze, ext)
{
    this->Level = (this->TableRange[0] + this->TableRange[1])/2;
    this->Window = (this->TableRange[1] - this->TableRange[0]);

    this->InverseVideo = 0;

    this->Clamping = 0;

    this->LeftClampValue[0] = 0.0;
    this->LeftClampValue[1] = 0.0;
    this->LeftClampValue[2] = 0.0;
    this->LeftClampValue[3] = 0.0;

    this->RightClampValue[0] = 0.0;
    this->RightClampValue[1] = 0.0;
    this->RightClampValue[2] = 0.0;
    this->RightClampValue[3] = 0.0;

    this->InvertTable = vtkUnsignedCharArray::New();
    this->InvertTable->Register(this);
    this->InvertTable->Delete();
    this->InvertTable->SetNumberOfComponents(4);
    this->InvertTable->Allocate(4*sze,4*ext);
}


fwVtkWindowLevelLookupTable::~fwVtkWindowLevelLookupTable()
{
    this->InvertTable->UnRegister(this);
    this->InvertTable = NULL;
}

void fwVtkWindowLevelLookupTable::BuildInvert()
{
    if (this->GetMTime() < this->InvertTime)
    {
        return;
    }
    if (this->Table->GetNumberOfTuples() < 1)
    {
        return;
    }

    this->InvertTable->SetNumberOfTuples(this->NumberOfColors);

    unsigned char *tableRgba, *invertTableRgba2;

    int n = this->NumberOfColors-1;
    for (int i = 0; i < this->NumberOfColors; i++)
    {
        tableRgba = this->Table->GetPointer(4*i);
        invertTableRgba2 = this->InvertTable->WritePointer(4*(n-i),4);

        invertTableRgba2[0] = tableRgba[0];
        invertTableRgba2[1] = tableRgba[1];
        invertTableRgba2[2] = tableRgba[2];
        invertTableRgba2[3] = tableRgba[3];
    }

    this->InvertTime.Modified();

}


unsigned char * fwVtkWindowLevelLookupTable::GetCurrentPointer(const vtkIdType id)
{
    if(this->InverseVideo)
    {
        this->BuildInvert();
        return this->InvertTable->GetPointer(4*id);
    }
    else
    {
        return this->Table->GetPointer(4*id);
    }
}


//----------------------------------------------------------------------------
void fwVtkWindowLevelLookupTable::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Window: " << this->Window << "\n";
    os << indent << "Level: " << this->Level << "\n";
    os << indent << "InverseVideo: "
        << (this->InverseVideo ? "On\n" : "Off\n");
    os << indent << "LeftClampValue : ("
        << this->LeftClampValue[0] << ", "
        << this->LeftClampValue[1] << ", "
        << this->LeftClampValue[2] << ", "
        << this->LeftClampValue[3] << ")\n";
    os << indent << "RightClampValue : ("
        << this->RightClampValue[0] << ", "
        << this->RightClampValue[1] << ", "
        << this->RightClampValue[2] << ", "
        << this->RightClampValue[3] << ")\n";
}


//----------------------------------------------------------------------------
// Apply log to value, with appropriate constraints.
inline double vtkApplyLogScale(double v, const double range[2],
        const double logRange[2])
{
    // is the range set for negative numbers?
    if (range[0] < 0)
    {
        if (v < 0)
        {
            v = log10(-static_cast<double>(v));
        }
        else if (range[0] > range[1])
        {
            v = logRange[0];
        }
        else
        {
            v = logRange[1];
        }
    }
    else
    {
        if (v > 0)
        {
            v = log10(static_cast<double>(v));
        }
        else if (range[0] < range[1])
        {
            v = logRange[0];
        }
        else
        {
            v = logRange[1];
        }
    }
    return v;
}


//----------------------------------------------------------------------------
// Apply shift/scale to the scalar value v and do table lookup.
inline unsigned char *vtkLinearLookup(double v,
        unsigned char *table,
        double maxIndex,
        double shift, double scale,
        unsigned char *nanColor,
        unsigned char *leftColor,
        unsigned char *rightColor
        )
{

    if (vtkMath::IsNan(v))
    {
        return nanColor;
    }

    double findx = (v + shift)*scale;
    if (findx < 0.0)
    {
        return leftColor;
    }
    else if (findx > maxIndex)
    {
        return rightColor;
    }
    return &table[4*static_cast<unsigned int>(findx)];
    /* round
       return &table[4*(unsigned int)(findx + 0.5f)];
       */
}


//----------------------------------------------------------------------------
// There is a little more to this than simply taking the log10 of the
// two range values: we do conversion of negative ranges to positive
// ranges, and conversion of zero to a 'very small number'
void fwVtkWindowLevelLookupTableLogRange(const double range[2], double logRange[2])
{
    double rmin = range[0];
    double rmax = range[1];

    if (rmin == 0)
    {
        rmin = 1.0e-6*(rmax - rmin);
        if (rmax < 0)
        {
            rmin = -rmin;
        }
    }
    if (rmax == 0)
    {
        rmax = 1.0e-6*(rmin - rmax);
        if (rmin < 0)
        {
            rmax = -rmax;
        }
    }
    if (rmin < 0 && rmax < 0)
    {
        logRange[0] = log10(-static_cast<double>(rmin));
        logRange[1] = log10(-static_cast<double>(rmax));
    }
    else if (rmin > 0 && rmax > 0)
    {
        logRange[0] = log10(static_cast<double>(rmin));
        logRange[1] = log10(static_cast<double>(rmax));
    }
}



//----------------------------------------------------------------------------
// accelerate the mapping by copying the data in 32-bit chunks instead
// of 8-bit chunks
    template<class T>
void fwVtkWindowLevelLookupTableMapData(fwVtkWindowLevelLookupTable *self, T *input,
        unsigned char *output, int length,
        int inIncr, int outFormat)
{
    int i = length;
    double *range = self->GetTableRange();
    double maxIndex = self->GetNumberOfColors() - 1;
    double shift, scale;
    unsigned char *table = self->GetCurrentPointer(0);
    unsigned char *cptr;
    double alpha;

    unsigned char nanColor[4];
    unsigned char selfLeftColor[4];
    unsigned char selfRightColor[4];

    for (int c = 0; c < 4; c++)
    {
        nanColor[c] = static_cast<unsigned char>(self->GetNanColor()[c]*255.0);
        selfLeftColor[c] = static_cast<unsigned char>(self->GetLeftClampValue()[c]*255.0);
        selfRightColor[c] = static_cast<unsigned char>(self->GetRightClampValue()[c]*255.0);
    }


    unsigned char *leftColor = selfLeftColor;
    unsigned char *rightColor = selfRightColor;

    if(self->GetClamping())
    {
        leftColor = &table[0];
        rightColor = &table[(self->GetNumberOfColors()-1) * 4];
    }

    if ( (alpha=self->GetAlpha()) >= 1.0 ) //no blending required
    {
        if (self->GetScale() == VTK_SCALE_LOG10)
        {
            double val;
            double logRange[2];
            fwVtkWindowLevelLookupTableLogRange(range, logRange);
            shift = -logRange[0];
            if (logRange[1] <= logRange[0])
            {
                scale = VTK_DOUBLE_MAX;
            }
            else
            {
                /* while this looks like the wrong scale, it is the correct scale
                 * taking into account the truncation to int that happens below. */
                // scale = (maxIndex + 1)/(logRange[1] - logRange[0]);
                // Fixed scale : finaly, no truncation happends below
                scale = (maxIndex)/(logRange[1] - logRange[0]);
            }
            if (outFormat == VTK_RGBA)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_RGB)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_LUMINANCE_ALPHA)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    *output++ = cptr[3];
                    input += inIncr;
                }
            }
            else // outFormat == VTK_LUMINANCE
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    input += inIncr;
                }
            }
        }//if log scale

        else //not log scale
        {
            shift = -range[0];
            if (range[1] <= range[0])
            {
                scale = VTK_DOUBLE_MAX;
            }
            else
            {
                /* while this looks like the wrong scale, it is the correct scale
                 * taking into account the truncation to int that happens below. */
                // scale = (maxIndex + 1)/(range[1] - range[0]);
                // Fixed scale : finaly, no truncation happends below
                scale = (maxIndex)/(range[1] - range[0]);
            }

            if (outFormat == VTK_RGBA)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_RGB)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_LUMINANCE_ALPHA)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    *output++ = cptr[3];
                    input += inIncr;
                }
            }
            else // outFormat == VTK_LUMINANCE
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    input += inIncr;
                }
            }
        }//if not log lookup
    }//if blending not needed

    else //blend with the specified alpha
    {
        if (self->GetScale() == VTK_SCALE_LOG10)
        {
            double val;
            double logRange[2];
            fwVtkWindowLevelLookupTableLogRange(range, logRange);
            shift = -logRange[0];
            if (logRange[1] <= logRange[0])
            {
                scale = VTK_DOUBLE_MAX;
            }
            else
            {
                /* while this looks like the wrong scale, it is the correct scale
                 * taking into account the truncation to int that happens below. */
                // scale = (maxIndex + 1)/(logRange[1] - logRange[0]);
                // Fixed scale : finaly, no truncation happends below
                scale = (maxIndex)/(logRange[1] - logRange[0]);
            }
            if (outFormat == VTK_RGBA)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = static_cast<unsigned char>((*cptr)*alpha); cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_RGB)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_LUMINANCE_ALPHA)
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    *output++ = static_cast<unsigned char>(alpha*cptr[3]);
                    input += inIncr;
                }
            }
            else // outFormat == VTK_LUMINANCE
            {
                while (--i >= 0)
                {
                    val = vtkApplyLogScale(*input, range, logRange);
                    cptr = vtkLinearLookup(val, table, maxIndex, shift, scale, nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    input += inIncr;
                }
            }
        }//log scale with blending

        else //no log scale with blending
        {
            shift = -range[0];
            if (range[1] <= range[0])
            {
                scale = VTK_DOUBLE_MAX;
            }
            else
            {
                /* while this looks like the wrong scale, it is the correct scale
                 * taking into account the truncation to int that happens below. */
                // scale = (maxIndex + 1)/(range[1] - range[0]);
                // Fixed scale : finaly, no truncation happends below
                scale = (maxIndex)/(range[1] - range[0]);
            }

            if (outFormat == VTK_RGBA)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = static_cast<unsigned char>((*cptr)*alpha); cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_RGB)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    *output++ = *cptr++;
                    input += inIncr;
                }
            }
            else if (outFormat == VTK_LUMINANCE_ALPHA)
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    *output++ = static_cast<unsigned char>(cptr[3]*alpha);
                    input += inIncr;
                }
            }
            else // outFormat == VTK_LUMINANCE
            {
                while (--i >= 0)
                {
                    cptr = vtkLinearLookup(*input, table, maxIndex, shift, scale,
                            nanColor, leftColor, rightColor);
                    *output++ = static_cast<unsigned char>(cptr[0]*0.30 + cptr[1]*0.59 +
                            cptr[2]*0.11 + 0.5);
                    input += inIncr;
                }
            }
        }//no log scale
    }//alpha blending

}




//----------------------------------------------------------------------------
// Although this is a relatively expensive calculation,
// it is only done on the first render. Colors are cached
// for subsequent renders.
    template<class T>
void fwVtkWindowLevelLookupTableMapMag(fwVtkWindowLevelLookupTable *self, T *input,
        unsigned char *output, int length,
        int inIncr, int outFormat)
{
    double tmp, sum;
    double *mag;
    int i, j;

    mag = new double[length];
    for (i = 0; i < length; ++i)
    {
        sum = 0;
        for (j = 0; j < inIncr; ++j)
        {
            tmp = static_cast<double>(*input);
            sum += (tmp * tmp);
            ++input;
        }
        mag[i] = sqrt(sum);
    }

    fwVtkWindowLevelLookupTableMapData(self, mag, output, length, 1, outFormat);

    delete [] mag;
}


//----------------------------------------------------------------------------
void fwVtkWindowLevelLookupTable::MapScalarsThroughTable2(void *input,
        unsigned char *output,
        int inputDataType,
        int numberOfValues,
        int inputIncrement,
        int outputFormat)
{
    if (this->UseMagnitude && inputIncrement > 1)
    {
        switch (inputDataType)
        {
            vtkTemplateMacro(
                    fwVtkWindowLevelLookupTableMapMag(this,static_cast<VTK_TT*>(input),output,
                        numberOfValues,inputIncrement,outputFormat);
                    return
                    );
            case VTK_BIT:
            vtkErrorMacro("Cannot comput magnitude of bit array.");
            break;
            default:
            vtkErrorMacro(<< "MapImageThroughTable: Unknown input ScalarType");
        }
    }

    switch (inputDataType)
    {
        case VTK_BIT:
            {
                vtkIdType i, id;
                vtkBitArray *bitArray = vtkBitArray::New();
                bitArray->SetVoidArray(input,numberOfValues,1);
                vtkUnsignedCharArray *newInput = vtkUnsignedCharArray::New();
                newInput->SetNumberOfValues(numberOfValues);
                for (id=i=0; i<numberOfValues; i++, id+=inputIncrement)
                {
                    newInput->SetValue(i, bitArray->GetValue(id));
                }
                fwVtkWindowLevelLookupTableMapData(this,
                        static_cast<unsigned char*>(newInput->GetPointer(0)),
                        output,numberOfValues,
                        inputIncrement,outputFormat);
                newInput->Delete();
                bitArray->Delete();
            }
            break;

            vtkTemplateMacro(
                    fwVtkWindowLevelLookupTableMapData(this,static_cast<VTK_TT*>(input),output,
                        numberOfValues,inputIncrement,outputFormat)
                    );
        default:
            vtkErrorMacro(<< "MapImageThroughTable: Unknown input ScalarType");
            return;
    }
}

