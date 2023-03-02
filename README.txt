Voluminous Volume Visualization Tool


Authors:  David Borland.  Visualization lead.
          Jeffrey L. Tilson.  Scientific application lead.

		  
Organization:  The Renaissance Computing Institute (RENCI)
               The University of North Carolina at Chapel Hill
			   
Website:  http://www.renci.org/focus-areas/visualization/visualization-projects/Voluminous
			   
Software Contact:  borland@renci.org
Scientific Contact:  jtilson@renci.org
			   
Version:  1.0

Date:  12/20/2012



Description: 

Voluminous is a tool developed to enable the visualization of volumetric 
scalar fields and in particular for finding and emphasizing locations in 
the field that separate positive and negative regions. It was originally 
developed for the visualization and analysis of computational chemistry 
electron density fields where the input volume represents the difference 
between two scalar fields. However, other scientific disciplines that 
require the interpretation of the difference between fields may also 
benefit from Voluminous. 



Toolkits: 

Voluminous is developed using Qt for the application framework and GUI, 
and the Visualization ToolKit (VTK) for the visualization. 

File Types: 

The application loads files in VTK's legacy structured points (.vtk) 
format and VTK's XML Image Data (.vti) format. Sample code for 
converting to the structured points .vtk format is included along with 
the application. 



Visualization features: 

Isosurfaces: 

The tool enables the selection of two isovalues that can be rendered via 
isosurfaces. For each isovalue, a positive and negative surface is 
drawn, assuming both positive and negative values exist for that 
isovalue. If not, only the positive or negative isosurface is drawn. 
Positive isosurfaces are drawn in red, and negative isosurfaces are 
drawn in blue. For each isovalue, visibility and translucency of the 
surfaces can be toggled. 

Per-Pixel Lighting and Translucency: 

On computers with graphics cards that support shaders, per-pixel 
lighting of surfaces is enabled, along with a translucency effect that 
modulates the opacity of the surface based on the angle between the 
surface and the viewpoint, enabling clearer visibility of any surfaces 
behind the translucent surface, and better shape-perception of the 
translucent surface itself. On graphics cards without shader support, 
standard per-vertex lighting and constant-opacity blending are used. 

Slices: 

Orthogonal slices through the center of the volume are displayed on the 
outer "walls" of the volume. These slices are clipped by the value of 
the smallest selected isovalue. 

Color Map: 

A double-ended color map is used for the slices. Positive values are 
mapped from gray to red to yellow to white, similar to a 
black-body-radiation color map. Negative values are mapped from gray to 
blue to cyan to white, a "cold" version of the black-body-radiation map. 
Each color map is scaled from 0 to the highest absolute value in the 
data, and if the most negative and most positive values differ, the 
postive or negative color map is truncated accordingly. 

Axes: 

Axes are drawn on the outside of the volume, labeled by axis and 
including the spatial extents of the data. 

Data label: 

A label is shown in the bottom-right of the view, showing the file 
information read from the VTK file, if available, or the file name 
without extension otherwise. 

Color/Grayscale: 

The application includes a grayscale mode that changes the isosurface 
and color map colors such that they are more suitable for printing in 
grayscale. 



Controls: 

Isovalue Controls: 

The value and visibility of the isosurfaces are controlled with the 
slider and check boxes in the Isocontour 1 and Isocontour 2 group boxes. 
The Isovalue Exponent slider controls the exponent of a power function 
used to map slider position to slider value, such that greater 
resolution can be obtained at lower values. Setting this value to 1 
results in a standard linear slider. 

Settings: 

The background color, color/grayscale rendering, and other visualization 
elements can be changed with the appropriate controls. The Interactive 
Data Resolution slider controls the amount of downsampling applied to 
the volume when moving the isovalue sliders. A lower value generates 
isosurfaces on a lower-resolution volume, and is thus faster but less 
accurate. A higher value generates isosurfaces on a higher-resolution 
volume (up to full resolution), and is thus slower but more accurate. 
Full-resolution surfaces are always generated when the slider is 
released. 



Examples: 

Two examples from the quantum chemistry (QC) community are provided. For 
both, all files required to reproduce the QC results are provided. To 
demonstrate Voluminous, however, only the pregenerated .vtk files are 
required. A tool for data conversion (mergePSOCIdensityFiles.C) is 
provided. This converts the raw QC data on a grid to the proper VTK 
format. It can be used as a template for customization.