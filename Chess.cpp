// Lotfi Achiri 
// Master 2 techmed 
//2021/2022
//
#include<iostream>
#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <ctime> //  l'heure de sys

#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleJoystickCamera.h>

#include <vtkWindowToImageFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkPNGWriter.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkInteractorStyle3D.h>
#include <vtkContextInteractorStyle.h>
#include <vtkDoubleArray.h>
#include <vtkExecutive.h>

#include <vtkInteractorStyleSwitch.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPlanes.h>

#include <vtkRectilinearGrid.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkAxesActor.h>
#include<conio.h>
#include <vtkOBJReader.h>// Read external .obj

#include "Chess.h"

namespace
{
	// Make a rectilinear grid that shows image-like checkerboard data
	vtkSmartPointer<vtkRectilinearGrid> makeImage(int n);
} 



void Observer::Execute(vtkObject *caller, unsigned long eventID, void* )
{
	vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
	
	char Key;
	Key = interactor->GetKeyCode();
	if (Key == 'z')
	{
		if (Y < YMax)
		{
			cout << "up key pressed" << endl;
			Y = Y + 6;
		Actor->SetOrigin(X, Y, 0.5);
		}

		else
		{
			cout << "YMAx reached" << endl;
		}
	}

	else if (Key == 's')
	{
		if (Y > YMin)
		{
			cout << "down key pressed" << endl;
			Y = Y - 6;
			Actor->SetOrigin(X, Y, 0.5);
		}
		else
		{
			cout << "YMin reached" << endl;
		}
	}

	else if (Key == 'd')
	
	{
		if (X < XMax) 
		{
			X = X + 6;
			Actor->SetOrigin(X, Y, 0.5);
		}
		else
		{
			cout << "XMAx reached" << endl;
		}
	}
	else
	
	{
		if (X > XMin)
		{
			X = X - 6;
			Actor->SetOrigin(X, Y, 0.5);
		}
		else
		{
			cout << "XMin reached" << endl; 
		}
	}
	
}

namespace
{
	vtkSmartPointer<vtkRectilinearGrid> makeImage(int n)
	{
		constexpr double cubeSize = 24.0; // physical linear dimension of entire system

		// This is a simplification of a program that uses actual image data
		// as a source for the rectilinear grid.  In order to recreate the
		// same vtk calls, create a dummy image here.
		vtkNew<vtkImageData> image0;
		image0->SetDimensions(n, n, n);
		image0->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		image0->SetSpacing(cubeSize / n, cubeSize / n, 0);
		int checkerSize = n / 8;
		for (int z = 0; z < n; z++)
		{
			for (int y = 0; y < n; y++)
			{
				for (int x = 0; x < n; x++)
				{
					unsigned char* ptr = (unsigned char*)image0->GetScalarPointer(x, y, z);
					*ptr = (x / checkerSize + y / checkerSize + z / checkerSize) %
						2; // checkerboard
				}
			}
		}

		vtkNew<vtkNamedColors> colors;

		// Color look up table
		vtkNew<vtkLookupTable> lut;
		lut->SetNumberOfTableValues(2);
		lut->SetTableRange(0, 1);
		lut->SetTableValue(0, colors->GetColor4d("White").GetData());
		lut->SetTableValue(1, colors->GetColor4d("Black").GetData());

		vtkNew<vtkImageMapToColors> map;
		map->SetLookupTable(lut);
		map->SetOutputFormatToRGBA();
		map->SetInputData(image0);
		map->GetExecutive()->Update();
		vtkImageData* image = map->GetOutput();

		// Convert the image to a rectilinear grid.  Each point in the image
		// becomes a  cell in the grid.
		vtkNew<vtkRectilinearGrid> rectgrid;

		int extent[6];
		image->GetExtent(extent);
		extent[1] += 1;
		extent[3] += 1;
		extent[5] += 1;
		rectgrid->SetExtent(extent);

		vtkNew<vtkDoubleArray> xcoords;
		vtkNew<vtkDoubleArray> ycoords;
		vtkNew<vtkDoubleArray> zcoords;
		xcoords->SetNumberOfValues(n + 1.0);
		ycoords->SetNumberOfValues(n + 1.0);
		zcoords->SetNumberOfValues(n + 1.0);
		double spacing[3];
		image->GetSpacing(spacing);


		for (vtkIdType i = 0; i <= n; i++)
		{
			xcoords->InsertValue(i, i * spacing[0]);
			ycoords->InsertValue(i, i * spacing[1]);
			zcoords->InsertValue(i, i * spacing[2]);
		}
		rectgrid->SetXCoordinates(xcoords);
		rectgrid->SetYCoordinates(ycoords);
		rectgrid->SetZCoordinates(zcoords);

		vtkPointData* pointData = image->GetPointData();
		vtkCellData* cellData = rectgrid->GetCellData();
		cellData->ShallowCopy(pointData);

		return rectgrid;

	}


} // namespace


int main(int, char*[])
{

	// Read chess Piece
	std::string filename = "chess2.obj";
	vtkNew<vtkOBJReader> reader;
	reader->SetFileName(filename.c_str());
	reader->Update();

	// Visualize
	vtkNew<vtkNamedColors> colors;
	vtkColor3d backgroundColor = colors->GetColor3d("SpringGreen");
	vtkColor3d actorColor = colors->GetColor3d("HoneyDew");

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(reader->GetOutputPort());
	
	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	actor->GetProperty()->SetDiffuseColor(actorColor.GetData());
	
	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(actor);
	actor->GetProperty()->SetDiffuseColor(
	colors->GetColor3d("Wheat").GetData());
	actor->GetProperty()->SetDiffuse(.7);
	actor->GetProperty()->SetSpecularPower(20);
	actor->GetProperty()->SetSpecular(.5);

	// Set the origin of the chess piece and rescale the .obj to fit on one square  
	// put the chess piece on the buttom left of the board 
	actor->SetOrigin(0.8, -13, 0); 
	actor->SetScale(0.5, 0.5,0.5);

	vtkNew<vtkAxesActor> axes;
	// create a cube source 
	vtkSmartPointer<vtkCubeSource> cubeSource =
	vtkSmartPointer<vtkCubeSource>::New();
	cubeSource->SetXLength(3);
	cubeSource->SetYLength(3);
	//cubeSource->SetCenter(4.6+ (offset*r), 4.6+ (offset*p), 0.5);
	//3 On X and Y to move ranks and philes
	vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
	vtkSmartPointer<vtkActor> cubeActor = vtkSmartPointer<vtkActor>::New();
	cubeActor->SetMapper(cubeMapper);
	cubeActor->GetProperty()->SetColor(1.0, 0.5, 1.0);

	// number of checkerboard squares on a side is 8 by 8 which means 64 squares
	// Offsets for clipping planes with normals in the X and Y directions
	constexpr int imageSize = 64; 
	constexpr int xOffset = 8;
	constexpr int yOffset = 8;

	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

	//vtkNew<vtkNamedColors> colors;
	/*vtkNew<vtkRenderer> renderer;*/
	renderer->SetBackground(colors->GetColor3d("Wheat").GetData());
	renderer->UseHiddenLineRemovalOn();

	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(480, 480);
	renderWindow->SetWindowName("ChessBoard");

	vtkNew<vtkRenderWindowInteractor> interactor;
	vtkNew<vtkInteractorStyleSwitch> style;
	interactor->SetInteractorStyle(style);
	interactor->SetRenderWindow(renderWindow);
	interactor->Initialize();

	// Creating the board
	vtkSmartPointer<vtkRectilinearGrid> image = makeImage(imageSize);

	// Clipping planes in the X and Y direction.
	auto normals = vtkSmartPointer<vtkDoubleArray>::New();
	vtkNew<vtkPoints> clipPts;
	normals->SetNumberOfComponents(3);
	double xnorm[3] = {0., 0., 0. };
	double ynorm[3] = { 0, 0., 0. };

	double xpt[3] = { xOffset, 0., 0. };
	double ypt[3] = { 0., yOffset, 0. };

	normals->InsertNextTuple(xnorm);
	normals->InsertNextTuple(ynorm);

	clipPts->InsertNextPoint(xpt);
	clipPts->InsertNextPoint(ypt);

	vtkNew<vtkPlanes> clipPlanes;
	clipPlanes->SetNormals(normals);
	clipPlanes->SetPoints(clipPts);

	vtkNew<vtkTableBasedClipDataSet> clipper;
	clipper->SetClipFunction(clipPlanes);
	clipper->SetInputData(image);

	vtkNew<vtkDataSetMapper> imageMapper;
	vtkNew<vtkActor> imageActor;
	imageActor->SetMapper(imageMapper);
	renderer->AddViewProp(imageActor);
	imageMapper->SetInputConnection(clipper->GetOutputPort());



	//  Camera, top down view of the board, change zoom, to zoom in and out 
	renderer->ResetCamera();
	renderer->GetActiveCamera()->Azimuth(0);
	renderer->GetActiveCamera()->Elevation(0);
	renderer->GetActiveCamera()->Zoom(1.502);
	renderer->ResetCameraClippingRange();
	renderWindow->Render();
	renderWindow->Render();
	// // // // // // // // // // // // // // // 


	//Observer eent Click sur le point r
	vtkSmartPointer<Observer> observer = vtkSmartPointer<Observer>::New();
	observer->setActorSource(actor);
	observer->SetYMax(24.5);
	observer->SetYMin(-1.5);
	observer->SetXMax(24.5);
	observer->SetXMin(-1.5);
	interactor->AddObserver(vtkCommand::KeyPressEvent, observer);
	renderWindow->Render();
	interactor->Start();
	return EXIT_SUCCESS;
}

