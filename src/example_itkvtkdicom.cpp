#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

typedef signed short PixelType;
typedef itk::Image< PixelType, 3 > ImageType;

ImageType::Pointer readDataArrayFromDICOM(std::string dirName)
{
	using NamesGeneratorType = itk::GDCMSeriesFileNames;
	NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

	nameGenerator->SetUseSeriesDetails(true);
	nameGenerator->AddSeriesRestriction("0008|0021");
	nameGenerator->SetGlobalWarningDisplay(false);
	nameGenerator->SetDirectory(dirName);

	using SeriesIdContainer = std::vector< std::string >;
	const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
	auto seriesItr = seriesUID.begin();
	auto seriesEnd = seriesUID.end();

	if (seriesItr != seriesEnd)
	{
		std::cout << "The directory: ";
		std::cout << dirName << std::endl;
		std::cout << "Contains the following DICOM Series: ";
		std::cout << std::endl;
	}
	else
	{
		std::cout << "No DICOMs in: " << dirName << std::endl;
	}

	while (seriesItr != seriesEnd)
	{
		std::cout << seriesItr->c_str() << std::endl;
		++seriesItr;
	}

	seriesItr = seriesUID.begin();
	std::string seriesIdentifier;
	seriesIdentifier = seriesItr->c_str();
	seriesItr++;

	std::cout << "\nReading: ";
	std::cout << seriesIdentifier << std::endl;
	using FileNamesContainer = std::vector< std::string >;
	FileNamesContainer fileNames =
		nameGenerator->GetFileNames(seriesIdentifier);

	using ReaderType = itk::ImageSeriesReader< ImageType >;
	ReaderType::Pointer reader = ReaderType::New();
	using ImageIOType = itk::GDCMImageIO;
	ImageIOType::Pointer dicomIO = ImageIOType::New();
	reader->SetImageIO(dicomIO);
	reader->SetFileNames(fileNames);
	reader->Update();

	return reader->GetOutput();
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << std::endl;
		std::cerr << argv[0] << " [DicomDirectory  [outputFileName  [seriesName]]]";
		std::cerr << "\nIf DicomDirectory is not specified, current directory is used\n";
	}
	std::string dirName = "."; //current directory by default
	if (argc > 1)
		dirName = argv[1];

	// Чтение папки томограмм 
	ImageType::Pointer data = readDataArrayFromDICOM(dirName);


	// Применение фильтра 
	typedef itk::BinaryThresholdImageFilter <ImageType, ImageType>
		BinaryThresholdImageFilterType;

	int lowerThreshold = 300;
	int upperThreshold = 30000;

	BinaryThresholdImageFilterType::Pointer thresholdFilter
		= BinaryThresholdImageFilterType::New();
	thresholdFilter->SetInput(data);
	thresholdFilter->SetLowerThreshold(lowerThreshold);
	thresholdFilter->SetUpperThreshold(upperThreshold);
	thresholdFilter->SetInsideValue(255);
	thresholdFilter->SetOutsideValue(0);

	typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
	ConnectorType::Pointer connector = ConnectorType::New();
	connector->SetInput(thresholdFilter->GetOutput());
	connector->Update();


	vtkSmartPointer<vtkMarchingCubes> surface =
		vtkSmartPointer<vtkMarchingCubes>::New();

	surface->SetInputData(connector->GetOutput());
	surface->ComputeNormalsOn();
	surface->SetValue(0, 100);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(surface->GetOutputPort());
	mapper->ScalarVisibilityOff();

	auto actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	auto renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);
	renderWindow->Render();
	interactor->Start();

	return EXIT_SUCCESS;
}