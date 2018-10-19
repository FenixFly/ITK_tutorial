#include "itkImage.h"
#include "itkRandomImageSource.h"
#include "QuickView.h"

int main(int argc, char ** argv)
{
	typedef itk::Image< unsigned char, 2 >  ImageType;

	itk::Size<2> size;
	size.Fill(100);

	itk::RandomImageSource<ImageType>::Pointer randomImageSource =
		itk::RandomImageSource<ImageType>::New();
	randomImageSource->SetSize(size);
	randomImageSource->Update();

	QuickView viewer;
	viewer.AddImage(randomImageSource->GetOutput());
	viewer.Visualize();

	return EXIT_SUCCESS;
}