/*=========================================================================
 *
 *  Copyright RTK Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef __rtkBinningImageFilter_cxx
#define __rtkBinningImageFilter_cxx

#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

typedef itk::Image<unsigned int, 2> TImage;
namespace rtk
{

//template<class TInputImage, class TOutputImage>
BinningImageFilter::BinningImageFilter()
{
  m_BinningFactors[0]=2;
  m_BinningFactors[1]=2;
}

//template<class TInputImage, class TOutputImage>
void BinningImageFilter::GenerateInputRequestedRegion()
{
  TImage::Pointer inputPtr = const_cast<TImage *>(this->GetInput());

  TImage::SpacingType inputSpacing;
  TImage::SizeType    inputSize;
  TImage::IndexType   inputStartIndex;
  TImage::PointType   inputOrigin;

  for (unsigned int i = 0; i < TImage::ImageDimension; i++)
  {
    inputSpacing[i] = inputPtr->GetSpacing()[i];
    inputSize[i] = inputPtr->GetLargestPossibleRegion().GetSize()[i];
    inputStartIndex[i] = inputPtr->GetLargestPossibleRegion().GetIndex()[i];
    inputOrigin[i] = inputPtr->GetOrigin()[i];
  }
  //Set Spacing
  inputPtr->SetSpacing( inputSpacing );
  //Set Origin
  inputPtr->SetOrigin( inputOrigin );
  //Set Region
  TImage::RegionType inputLargestPossibleRegion;
  inputLargestPossibleRegion.SetSize( inputSize );
  inputLargestPossibleRegion.SetIndex( inputStartIndex );
  inputPtr->SetLargestPossibleRegion( inputLargestPossibleRegion );
}

//template<class TImage, class TOutputImage>
void BinningImageFilter::GenerateOutputInformation()
{
  const TImage::SpacingType& inputSpacing    = this->GetInput()->GetSpacing();
  const TImage::SizeType&    inputSize       = this->GetInput()->GetLargestPossibleRegion().GetSize();
  const TImage::IndexType&   inputStartIndex = this->GetInput()->GetLargestPossibleRegion().GetIndex();
  const TImage::PointType&   inputOrigin     = this->GetInput()->GetOrigin();

  TImage::SpacingType  outputSpacing;
  TImage::SizeType     outputSize;
  TImage::IndexType    outputStartIndex;
  TImage::PointType    outputOrigin;

  for (unsigned int i = 0; i < TImage::ImageDimension; i++)
  {
    outputSpacing[i] = inputSpacing[i] * (double) m_BinningFactors[i];
    outputOrigin[i]  = inputOrigin[i];
    // Round down so that all output pixels fit input region
    outputSize[i] = (unsigned long)vcl_floor(double(inputSize[i]) / double(m_BinningFactors[i]) );

    if( outputSize[i] < 1 )
    {
      outputSize[i] = 1;
    }

    // Because of the later origin shift this starting index is not
    // critical
    outputStartIndex[i] = (long)vcl_ceil((double) inputStartIndex[i] / (double) m_BinningFactors[i] );
  }

  this->GetOutput()->SetSpacing( outputSpacing );
  //FIXME: origini needs to be shifted 0.5 pixels after binning
  this->GetOutput()->SetOrigin( outputOrigin );

  // Set region
  TImage::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( outputSize );
  outputLargestPossibleRegion.SetIndex( outputStartIndex );
  this->GetOutput()->SetLargestPossibleRegion( outputLargestPossibleRegion );
}

//template <class TInputImage, class TOutputImage>
void BinningImageFilter
::ThreadedGenerateData(const OutputImageRegionType& itkNotUsed(outputRegionForThread), ThreadIdType itkNotUsed(threadId) )
{

  int inputSize[2], binSize[2];
  inputSize[0] = this->GetInput()->GetLargestPossibleRegion().GetSize()[0];
  inputSize[1] = this->GetInput()->GetLargestPossibleRegion().GetSize()[1];
  binSize[0] = (inputSize[0]>>1);
  binSize[1] = (inputSize[1]>>1);

  typedef   TImage::PixelType inputPixel;
  typedef   TImage::PixelType outputPixel;

  //this->GenerateOutputInformation();
  inputPixel  *bufferIn  = const_cast<inputPixel*>( this->GetInput()->GetBufferPointer() );
  outputPixel *bufferOut = const_cast<outputPixel*>(this->GetOutput()->GetBufferPointer() );

  // Binning 2x2
  if(m_BinningFactors[0]==2 && m_BinningFactors[1]==2)
  {
    int i, j, idx, pos;
    for (i=0, idx=0;i<binSize[0]*(inputSize[1]);i++,idx+=2) // Only works if number of pixels per line multiple of 2
      bufferIn[i] = (bufferIn[idx])+(bufferIn[idx+1]);

    for (j=0; j<binSize[1];j++)
    {
      pos = j*binSize[0];
      for (i=0; i<binSize[0];i++, pos++)
        bufferOut[pos] = ( (bufferIn[2*j*binSize[0]+i])+(bufferIn[(2*j+1)*binSize[0]+i]) )>>2;
    }
  }

  // Binning 2x1
  else if(m_BinningFactors[0]==2 && m_BinningFactors[1]==1)
  {
    int i, idx;
    for (i=0, idx=0;i<binSize[0]*(inputSize[1]);i++,idx+=2) // Only works if number of pixels per line multiple of 2
      bufferOut[i] = ( (bufferIn[idx])+(bufferIn[idx+1]) )>>1;
  }

  // Binning 1x2
  else if(m_BinningFactors[0]==1 && m_BinningFactors[1]==2)
  {
    int i, j, pos;
    for (j=0; j<binSize[1];j++)
    {
      pos = j*inputSize[0];
      for (i=0; i<inputSize[0];i++, pos++)
        bufferOut[pos] = ( (bufferIn[2*j*inputSize[0]+i])+(bufferIn[(2*j+1)*inputSize[0]+i]) )>>1;
    }
  }
  else
  {
    itkExceptionMacro(<< "Binning factors mismatch! Current factors: "
                      << m_BinningFactors[0] << "x"
                      << m_BinningFactors[1] << " "
                      << "accepted modes [2x2] [2x1] and [1x2]")
  }
}

} // end namespace rtk

#endif // __rtkBinningImageFilter_cxx
