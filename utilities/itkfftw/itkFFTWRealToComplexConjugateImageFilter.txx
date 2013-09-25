/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFFTWRealToComplexConjugateImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2010-02-26 23:50:55 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFFTWRealToComplexConjugateImageFilter_txx
#define __itkFFTWRealToComplexConjugateImageFilter_txx

#include "itkFFTWRealToComplexConjugateImageFilter.h"
#if ITK_VERSION_MAJOR <= 3
#  include <itkFFTRealToComplexConjugateImageFilter.txx>
#else
#  include <itkForwardFFTImageFilter.hxx>
#endif
#include <iostream>
#include <itkIndent.h>
#include <itkMetaDataObject.h>
#include <itkProgressReporter.h>

namespace itk
{
/** TODO:  There should be compile time type checks so that
           if only USE_FFTWF is defined, then only floats are valid.
           and if USE_FFTWD is defined, then only doubles are valid.
*/

#if ITK_VERSION_MAJOR >= 4
  template <typename TInputImage, typename TOutputImage>
  void
  FFTWRealToComplexConjugateImageFilter<TInputImage,TOutputImage>::
#else
  template <typename TPixel, unsigned int VDimension>
  void
  FFTWRealToComplexConjugateImageFilter<TPixel,VDimension>::
#endif
  GenerateData()
{
  // get pointers to the input and output
  typename InputImageType::ConstPointer  inputPtr  = this->GetInput();
  typename OutputImageType::Pointer      outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // we don't have a nice progress to report, but at least this simple line
  // reports the begining and the end of the process
  itk::ProgressReporter progress(this, 0, 1);

  // allocate output buffer memory
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  const typename InputImageType::SizeType&   inputSize
    = inputPtr->GetLargestPossibleRegion().GetSize();
  const typename OutputImageType::SizeType&   outputSize
    = outputPtr->GetLargestPossibleRegion().GetSize();

  // figure out sizes
  // size of input and output aren't the same which is handled in the
  // superclass, sort of.
  // the input size and output size only differ in the fastest moving dimension
  unsigned int total_inputSize = 1;
  unsigned int total_outputSize = 1;

  unsigned int ImageDimension = InputImageType::ImageDimension;
  for(unsigned i = 0; i <ImageDimension; i++)
    {
    total_inputSize *= inputSize[i];
    total_outputSize *= outputSize[i];
    }

  typename FFTWProxyType::PlanType plan;
  InputPixelType * in = const_cast<InputPixelType*>(inputPtr->GetBufferPointer() );
  typename FFTWProxyType::ComplexType * out = (typename FFTWProxyType::ComplexType*)outputPtr->GetBufferPointer();
  int flags = FFTW_ESTIMATE;
  if( !m_CanUseDestructiveAlgorithm )
    {
    // if the input is about to be destroyed, there is no need to force fftw
    // to use an non destructive algorithm. If it is not released however,
    // we must be careful to not destroy it.
    flags = flags | FFTW_PRESERVE_INPUT;
    }
  switch(ImageDimension)
    {
    case 1:
      plan = FFTWProxyType::Plan_dft_r2c_1d(inputSize[0],
                                            in,
                                            out,
                                            flags,
                                            this->GetNumberOfThreads() );
      break;
    case 2:
      plan = FFTWProxyType::Plan_dft_r2c_2d(inputSize[1],
                                            inputSize[0],
                                            in,
                                            out,
                                            flags,
                                            this->GetNumberOfThreads() );
      break;
    case 3:
      plan = FFTWProxyType::Plan_dft_r2c_3d(inputSize[2],
                                            inputSize[1],
                                            inputSize[0],
                                            in,
                                            out,
                                            flags,
                                            this->GetNumberOfThreads() );
      break;
    default:
      int *sizes = new int[ImageDimension];
      for(unsigned int i = 0; i < ImageDimension; i++)
        {
        sizes[(ImageDimension - 1) - i] = inputSize[i];
        }

      plan = FFTWProxyType::Plan_dft_r2c(ImageDimension,
                                         sizes,
                                         in,
                                         out,
                                         flags,
                                         this->GetNumberOfThreads() );
      delete [] sizes;
    }
  FFTWProxyType::Execute(plan);
  FFTWProxyType::DestroyPlan(plan);
}

#if ITK_VERSION_MAJOR >= 4
  template <typename TInputImage, typename TOutputImage>
  bool
  FFTWRealToComplexConjugateImageFilter<TInputImage,TOutputImage>::
#else
  template <typename TPixel, unsigned int VDimension>
  bool
  FFTWRealToComplexConjugateImageFilter<TPixel,VDimension>::
#endif
  FullMatrix()
{
  return false;
}

#if ITK_VERSION_MAJOR >= 4
  template <typename TInputImage, typename TOutputImage>
  void
  FFTWRealToComplexConjugateImageFilter<TInputImage,TOutputImage>::
#else
  template <typename TPixel, unsigned int VDimension>
  void
  FFTWRealToComplexConjugateImageFilter<TPixel,VDimension>::
#endif
  UpdateOutputData(itk::DataObject * output)
{
  // we need to catch that information now, because it is changed later
  // during the pipeline execution, and thus can't be grabbed in
  // GenerateData().
  m_CanUseDestructiveAlgorithm = this->GetInput()->GetReleaseDataFlag();
  Superclass::UpdateOutputData( output );
}

} // namespace itk

#endif //_itkFFTWRealToComplexConjugateImageFilter_txx