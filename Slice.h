/*=========================================================================

  Name:        Slice.h

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for a slice of the volume.

=========================================================================*/


#ifndef SLICE_H


#include "vtkSmartPointer.h"

class vtkActorCollection;
class vtkAlgorithmOutput;
class vtkClipPolyData;
class vtkColorTransferFunction;
class vtkCutter;
class vtkPlane;


class Slice {
public:
    Slice(vtkAlgorithmOutput* volume, vtkColorTransferFunction* colorMap, 
          int direction, double clipValue, const double center[3], const double size[3]);
    ~Slice();

    void SetInput(vtkAlgorithmOutput* volume);

    vtkActorCollection* GetActors();

    void SetClipValue(double value);

protected:
    vtkSmartPointer<vtkCutter> cutter;
    vtkSmartPointer<vtkPlane> plane;
    vtkSmartPointer<vtkClipPolyData> clipperPos;
    vtkSmartPointer<vtkClipPolyData> clipperNeg;
    vtkSmartPointer<vtkActorCollection> actors;
};


#endif