/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

// Implement the longword multi incremented 2d spectrum.

#include <config.h>
#include "CSpectrum2DmW.h"
#include <Event.h>
#include <RangeError.h>
#include <TH2S.h>
#include <TDirectory.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//////////////////////////////////////////////////////////////////
///////////////////// Canonicals //////////////////////////////////
//////////////////////////////////////////////////////////////////

/*!
   Constructor:  The main work is done by the base class.. we just
   need to set the storage type so that we get the appropriate
   amount of storage...and get the storage in the first place.
  Construct the spectrum.
  \param name   : std::string
     Name of the spectrum.
  \param id     : UInt_T
     unique id of the spect5rum.
  \param parameters : std::vector<CParameter>
     Set of parameters that make up the spectrums parameters...
     the caller must ensure that there are an even number of these.
  \param xscale, yscale : UInt_t
     Number of channels on the X axis and y axis respectively.
*/
CSpectrum2DmW::CSpectrum2DmW(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xscale,
			     UInt_t                   yscale) :
  CSpectrum2Dm(name, id, parameters, xscale, yscale)
{
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2S* pRootSpectrum = new TH2S(
    name.c_str(), name.c_str(),
    xscale, 0.0, static_cast<Double_t>(xscale),
    yscale, 0.0, static_cast<Double_t>(yscale)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  CreateChannels();
  setStorageType(keWord);
  gDirectory->Cd(olddir.c_str());
  
}

/*!
   Overloaded constructor:
    \param name : std::string
       Name of the spectrum.
    \param id   : UInt_t
       Unique spectrum id.
    \param parameters :std::vector<CParameter>& parameters
       Parameter definitions for the histogram.
    \param xchans : UInt_t
      Channels on x axis.
    \param ychans : UInt_t ychans
      Channels on y axis.
    \param xlow,xhigh : Float_t
      Range covered by x axis.
    \param ylow,yhigh : Float_t
      Range covered by y axis.

*/
CSpectrum2DmW::CSpectrum2DmW(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xchans,
			     UInt_t                   ychans,
			     Float_t  xlow, Float_t   xhigh,
			     Float_t  ylow, Float_t   yhigh) :
  CSpectrum2Dm(name, id, parameters, xchans, ychans,
	       xlow, xhigh, ylow, yhigh)
{
  std::string olddir = gDirectory->GetPath();
  gDirectory->Cd("/");
  TH2S* pRootSpectrum = new TH2S(
    name.c_str(), name.c_str(),
    xchans, static_cast<Double_t>(xlow),  static_cast<Double_t>(xhigh),
    ychans, static_cast<Double_t>(ylow),  static_cast<Double_t>(yhigh)
  );
  pRootSpectrum->Adopt(0, nullptr);
  setRootSpectrum(pRootSpectrum);
  CreateChannels();
  setStorageType(keWord);
  gDirectory->Cd(olddir.c_str());
  
}


CSpectrum2DmW::~CSpectrum2DmW()
{
  TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(getRootSpectrum());
  pRootSpectrum->fArray = nullptr;

}


///////////////////////////////////////////////////////////////////
///////////////// Virtual function overrides //////////////////////
///////////////////////////////////////////////////////////////////

/*!
   Fetches a value from the spectrum.  The parameters
   are a bit awkward in order to be able to accomodate
   all spectrum types and shapes.

   \param pIndices : const UInt_t*
      Pointer to an array of indices.  It is the caller's responsibility
      to ensure that there are 2 indices.

   \return ULong_t
   \retval  the channel value at the x/y coordinates passed in.
 
   \throws CRangeError - in the event one or more of the indices is out of
           range.
*/
ULong_t
CSpectrum2DmW::operator[](const UInt_t* pIndices) const
{
  
  Int_t  x      = pIndices[0];
  Int_t  y      = pIndices[1];
  
  const TH1* pRootSpectrum = getRootSpectrum();
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(
    pRootSpectrum->GetBin(x + 1,y + 1)
  ));
}

/*!
   Set a channel value.
   \param pIndices : const UInt_t* 
     Array of indices. Caller must ensure there are two indices, x,y in that order.
   \param nValue : ULong_t
     Value to set in the spectrum channel.
*/
void
CSpectrum2DmW::set(const UInt_t* pIndices, ULong_t nValue)
{
  
  Int_t x     = pIndices[0];
  Int_t y     = pIndices[1];

  if (x >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      string("Indexing 2m spectrum (x)"));
  }
  if (y >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      string("Indexing 2m Spectrum (y)"));
  }
  TH1* pRootSpectrum = getRootSpectrum();
  
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(x+1,y+1), static_cast<Double_t>(nValue)
  );
}

/*!
   Increment the spectrum.  By now the spectrum gate  has been checked
   and we will unconditionally increment the spectrum.
   For each consecutive pair in the parameter array we'll increment if both
   indices scale to in range.
   
   \param rEvent : const CEvent&

      Reference to the event 'array'.
*/
void 
CSpectrum2DmW::Increment(const CEvent& rEvent)
{
  for (int i =0; i < m_parameterList.size(); i += 2) {
    IncPair(rEvent, m_parameterList[i], m_parameterList[i+1], i);
  }
}

/////////////////////////////////////////////////////////////////////////
//////////////////////// Private utilities //////////////////////////////
/////////////////////////////////////////////////////////////////////////

// Create and set channel storage (also data type).

void
CSpectrum2DmW::CreateChannels()
{
  setStorageType(keWord);
  UShort_t*  pStorage = new UShort_t[m_xChannels*m_yChannels];
  ReplaceStorage(pStorage);
  Clear();

}

// Increment for one of the parameter pairs:

void
CSpectrum2DmW::IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny, int i)
{
  // The parameters must both be in the range of the rEvent vector:

  UInt_t nParams = rEvent.size();
  if (!((nx < nParams) && (ny < nParams))) {
    return;			// One or both out of range of the vector.
  }
  // The parameters must both be defined:

  if (!(const_cast<CEvent&>(rEvent)[nx].isValid() && 
	const_cast<CEvent&>(rEvent)[ny].isValid())) {
    return;			// One or both invalid for this event.
  }
  // The parameters must both be in range for the spectrum after scaling:

  Double_t x = const_cast<CEvent&>(rEvent)[nx];
  Double_t y = const_cast<CEvent&>(rEvent)[ny];
  getRootSpectrum()->Fill(x,y);
    
}
/**
 * setStorage
 *    Tell the root histogram to use a different block of storage for its
 *    channels.
 * @param pStorage - pointer to the new storage to use.
 */
void
CSpectrum2DmW::setStorage(Address_t pStorage)
{
  TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(getRootSpectrum());
  pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
  pRootSpectrum->fN     = Dimension(0) * Dimension(1);
}
/**
 * StorageNeeded
 *
 * @return Size_t - number of bytes of spectrum storage needed for this spectrum.
 */
Size_t
CSpectrum2DmW::StorageNeeded() const
{
  return static_cast<Size_t>(Dimension(0) * Dimension(1) * sizeof(Short_t));
}
