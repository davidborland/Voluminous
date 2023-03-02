/*=========================================================================

  Name:        VTKPipeline.h

  Author:      David Borland, The Renaissance Computing Institute (RENCI)

  Copyright:   The Renaissance Computing Institute (RENCI)

  Description: Container class for all Voluminous VTK code.

=========================================================================*/


#ifndef VTKPIPELINE_H
#define VTKPIPELINE_H

#include <string>
#include <vector>

#include <vtkSmartPointer.h>


class vtkAlgorithm;
class vtkColorTransferFunction;
class vtkCubeAxesActor;
class vtkImageActor;
class vtkImageData;
class vtkImageResize;
class vtkRenderWindowInteractor;
class vtkRenderer;
class vtkScalarBarActor;
class vtkTextActor;
class vtkXMLMaterial;

class Isosurface;
class Slice;


class vtkRenderWindow;


class VTKPipeline {
public:	
    VTKPipeline(vtkRenderWindowInteractor* rwi,
                vtkImageData* logoImage, vtkImageData* bwLogoImage,
                const std::string& opaqueMaterial, const std::string& translucentMaterial);
    ~VTKPipeline();

    // Load data.
    // This was split into two functions so that OpenVolume could be called via a Qt thread in order to be able
    // to show a file progress dialog.  For OpenVolume, errorMessage must be passed as a pointer to work
    // with QtConcurrent::run().
    bool OpenVolume(const std::string& fileName, std::string* errorMessage);
    bool CreateVisualization(std::string& errorMessage);

    // Save a screenshot
    void SaveScreenshot(const std::string& fileName);
    
    // Get/set isosurface values
    double GetIsovalue1();
    double GetIsovalue2();
    void SetIsovalue1(double value, bool doFast = false);
    void SetIsovalue2(double value, bool doFase = false);

	// Get/set isosurface opacities
/*
	double GetIsovalue1Opacity();
	double GetIsovalue2Opacity();
	void SetIsovalue1Opacity(double opacity);
	void SetIsovalue2Opacity(double opacity);
*/

	// Get/set isosurface visibility
	bool GetIsovalue1Visible();
	bool GetIsovalue1Translucent();
	bool GetIsovalue2Visible();
	bool GetIsovalue2Translucent();
	void SetIsovalue1Visible(bool visible);
	void SetIsovalue1Translucent(bool translucent);
	void SetIsovalue2Visible(bool visible);
	void SetIsovalue2Translucent(bool translucent);

    // Get/set the background color
    void GetBackgroundColor(double rgb[3]);
    void SetBackgroundColor(double r, double g, double b);

    // Get/set the color map type
    enum ColorMapType {
        Color,
        Grayscale
    };
    ColorMapType GetColorMapType();
    void SetColorMapType(ColorMapType);

    // Get/set axes visibility
    bool GetShowAxes();
    void SetShowAxes(bool show);

    // Get/set color legend visibility
    bool GetShowColorLegend();
    void SetShowColorLegend(bool show);

    // Get/set data label visibility
    bool GetShowDataLabel();
    void SetShowDataLabel(bool show);

    // Get/set interactive data magnification
    double GetInteractiveDataMagnification();
    void SetInteractiveDataMagnification(double magnification);

    // Get data range
    void GetDataRange(double range[2]);

    // Get the maximum absolute value of data in the volume
    double GetMaximumAbsoluteValue();

    // Force a render
    void Render();

    // Stereo
    void SetUseStereo(bool useStereo);
    void FlipEyes();

    // Head tracking
    void SetHeadPosition(double x, double y, double z);

protected:
    // Rendering
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderer> logoRenderer;

    // The volume
    vtkSmartPointer<vtkAlgorithm> reader;
    vtkSmartPointer<vtkImageData> volume;

    // Use a downsampled volume when adjusting the isosurface value
    vtkSmartPointer<vtkImageResize> shrinker;

    // Visualization objects
    vtkSmartPointer<vtkCubeAxesActor> axes;
    vtkSmartPointer<vtkScalarBarActor> colorLegend;
    vtkSmartPointer<vtkTextActor> dataLabel;
    vtkSmartPointer<vtkImageActor> logo;
    vtkSmartPointer<vtkImageActor> bwLogo;

    std::vector<Isosurface*> isosurfaces;
    Slice* slices[3];
    vtkSmartPointer<vtkColorTransferFunction> colorMap;

    // Shader strings
    std::string opaqueMaterial;
    std::string translucentMaterial;

    // The range of data values in the volume
    double dataRange[2];

    // The color map type
    ColorMapType colorMapType;

    // Helper function for setting isovalues
    void SetIsovalues(int index1, int index2, double value, bool doFast);

    // Set the color map
    void SetColorMap();

    void SetColorMapToColor();
    void SetColorMapToGrayscale();

    void SetIsosurfacesToColor();
    void SetIsosurfacesToGrayscale();

    // For head tracking
    double cameraPosition[3];
};


#endif