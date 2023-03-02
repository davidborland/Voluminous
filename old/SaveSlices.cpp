///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        SaveSlices.cpp
//
// Author:      David Borland
//
// Description: Saves all slices through a volume
//
/////////////////////////////////////////////////////////////////////////////////////////////// 


#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkImageResample.h>
#include <vtkTIFFWriter.h>
#include <vtkXMLImageDataReader.h>


int main(int argc, char** argv) {
    // Load the data
    std::string fileName;
    if (argc > 1) {
        fileName = argv[1];
    }
    else {
        fileName = "Data/diffdensity_scaled.vti";
    }

    vtkXMLImageDataReader* reader = vtkXMLImageDataReader::New();
    reader->SetFileName(fileName.c_str());
    reader->Update();


    // Get data info
    double range[2];
    reader->GetOutput()->GetScalarRange(range);

    int extent[6];
    reader->GetOutput()->GetExtent(extent);

    printf("%d %d %d %d %d %d\n", extent[0], extent[1], extent[2], extent[3], extent[4], extent[5]);

    double origin[3];
    reader->GetOutput()->GetOrigin(origin);

    printf("%f %f %f\n", origin[0], origin[1], origin[2]);

    double spacing[3];
    reader->GetOutput()->GetSpacing(spacing);

    printf("%f %f %f\n", spacing[0], spacing[1], spacing[2]);

    double size[3];
    for (int i = 0; i < 3; i++) {
        size[i] = (double)(extent[2 * i + 1] - extent[2 * i]) * spacing[i];
    }

    printf("%f %f %f\n", size[0], size[1], size[2]);

    double center[3];
    for (int i = 0; i < 3; i++) {
        center[i] = origin[i] + size[i] * 0.5;
    }

    printf("%f %f %f\n", center[0], center[1], center[2]);


    // Create the slicer
    vtkImageReslice* slicer = vtkImageReslice::New();
    slicer->SetInputConnection(reader->GetOutputPort());
    slicer->SetOutputDimensionality(2);


    // Create the resampler
    vtkImageResample* resampler = vtkImageResample::New();
    resampler->SetInputConnection(slicer->GetOutputPort());
    resampler->SetAxisMagnificationFactor(0, 4.0);
    resampler->SetAxisMagnificationFactor(1, 4.0);
    resampler->SetAxisMagnificationFactor(2, 1.0);


    // Create the colormap using a color transfer function
    vtkColorTransferFunction* colorMap = vtkColorTransferFunction::New();
    colorMap->AddRGBPoint(range[0], 0.0, 0.0, 1.0);   
    colorMap->AddRGBPoint(0.0, 0.75, 0.75, 0.75);
    colorMap->AddRGBPoint(-range[0], 1.0, 0.0, 0.0);
    colorMap->AddRGBPoint((range[1] + range[0]) * 0.5 - range[0], 1.0, 1.0, 0.0);
    colorMap->AddRGBPoint(range[1], 0.9, 0.9, 0.9);

    double color1[3];
    double color2[3];
    double v1 = 0.1;
    double v2 = 0.05;
    colorMap->GetColor(range[0] * v1, color1);
    colorMap->AddRGBPoint(range[0] * v2, color1[0], color1[1], color1[2]);          
    colorMap->GetColor(-range[0] * v1, color2);
    colorMap->AddRGBPoint(-range[0] * v2, color2[0], color2[1], color2[2]);

    double color3[3];
    double color4[3];
    colorMap->GetColor(-0.0002, color3);
    colorMap->GetColor(0.0002, color4);


    colorMap->RemoveAllPoints();
    colorMap->AddRGBPoint(range[0], 0.0, 0.0, 1.0); 
    colorMap->AddRGBPoint(-range[0], 1.0, 0.0, 0.0);
    colorMap->AddRGBPoint((range[1] + range[0]) * 0.5 - range[0], 1.0, 1.0, 0.0);
    colorMap->AddRGBPoint(range[1], 0.9, 0.9, 0.9);

    colorMap->AddRGBPoint(range[0] * v2, color1[0], color1[1], color1[2]);          
    colorMap->AddRGBPoint(-range[0] * v2, color2[0], color2[1], color2[2]);

    colorMap->AddRGBPoint(-0.0002, color3[0], color3[1], color3[2]);
    colorMap->AddRGBPoint(0.0002, color4[0], color4[1], color4[2]);
    colorMap->AddRGBPoint(-0.00019, 1.0, 1.0, 1.0);
    colorMap->AddRGBPoint(0.00019, 1.0, 1.0, 1.0);


    // Create the colormapper
    vtkImageMapToColors* colorMapper = vtkImageMapToColors::New();
    colorMapper->SetInputConnection(resampler->GetOutputPort());
    colorMapper->SetLookupTable(colorMap);
    colorMapper->SetOutputFormatToRGB();


    // Creat the writer
    vtkTIFFWriter* writer = vtkTIFFWriter::New();
    writer->SetInputConnection(colorMapper->GetOutputPort());
    writer->SetCompressionToPackBits();


    // Generate the slices
    int i;
    double pos;

    int volStartSlice = 77;
    int numVolSlices = 255 - 2 * (volStartSlice + 1);
    int numSlices = 120;
    double posIncrement = (numVolSlices * spacing[0]) / (double)numSlices;
    double startPos = center[0] - (numSlices / 2) * spacing[0];
    double endPos = center[0] + (numSlices / 2) * spacing[0];

    // X
    i = 0;

    slicer->SetResliceAxesDirectionCosines(0, 0, 1,
                                           0, 1, 0,
                                           1, 0, 0);

    // Out to edge
    for (pos = center[0]; pos <= endPos; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_X_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(pos, 0, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_X_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Back to other edge
    for (; pos >= startPos; pos -= posIncrement) {
        fileName = "Data/Slices/UraniumSlice_X_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(pos, 0, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_X_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Finish in center
    for (; pos <= center[0]; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_X_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(pos, 0, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }




    // Y
    i = 0;

    slicer->SetResliceAxesDirectionCosines(1, 0, 0,
                                           0, 0, 1,
                                           0, 1, 0);

    // Out to edge
    for (pos = center[0]; pos <= endPos; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Y_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, pos, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_Y_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Back to other edge
    for (; pos >= startPos; pos -= posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Y_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, pos, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_Y_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Finish in center
    for (; pos <= center[0]; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Y_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, pos, 0);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }




    // Z
    i = 0;

    slicer->SetResliceAxesDirectionCosines(1, 0, 0,
                                           0, 1, 0,
                                           0, 0, 1);

    // Out to edge
    for (pos = center[0]; pos <= endPos; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Z_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, 0, pos);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_Z_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Back to other edge
    for (; pos >= startPos; pos -= posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Z_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, 0, pos);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Pause
    for (int j = 0; j < 15; j++) {
        fileName = "Data/Slices/UraniumSlice_Z_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }

    // Finish in center
    for (; pos <= center[0]; pos += posIncrement) {
        fileName = "Data/Slices/UraniumSlice_Z_";
        if (i < 10) fileName += "00";
        else if (i < 100) fileName += "0";

        char buffer[8];
        sprintf(buffer, "%d", i);

        fileName += buffer;
        fileName += ".tif";
            
        slicer->SetResliceAxesOrigin(0, 0, pos);

        writer->SetFileName(fileName.c_str());
        writer->Update();

        i++;
    }
}