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

#ifndef __rtkVarianObiXMLFileReader_h
#define __rtkVarianObiXMLFileReader_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <itkXMLFile.h>
#include <itkMetaDataDictionary.h>

#include <map>

namespace rtk
{

/** \class VarianObiXMLFileReader
 *
 * Reads the XML-format file written by a Varian OBI
 * machine for every acquisition
 */
class VarianObiXMLFileReader : public itk::XMLReader<itk::MetaDataDictionary>
{
public:
  /** Standard typedefs */
  typedef VarianObiXMLFileReader                  Self;
  typedef itk::XMLReader<itk::MetaDataDictionary> Superclass;
  typedef itk::SmartPointer<Self>                 Pointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VarianObiXMLFileReader, itk::XMLReader);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Determine if a file can be read */
  int CanReadFile(const char* name);

protected:
  VarianObiXMLFileReader(){m_OutputObject = &m_Dictionary;};
  virtual ~VarianObiXMLFileReader() {};

  virtual void StartElement(const char * name,const char **atts);

  virtual void EndElement(const char *name);

  void CharacterDataHandler(const char *inData, int inLength);

private:
  VarianObiXMLFileReader(const Self&); //purposely not implemented
  void operator=(const Self&);         //purposely not implemented

  itk::MetaDataDictionary m_Dictionary;
  std::string             m_CurCharacterData;
};

}
#endif