#include "TestDisplay.h"
#include "GateContainer.h"
#include "Spectrum.h"
#include "CSpectrumFit.h"

#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;


CTestDisplay* CTestDisplayCreator::create()
{
    return new CTestDisplay;
}



int CTestDisplay::m_nextFitlineId = 0;


CTestDisplay::CTestDisplay()
    : m_alive(false), m_DisplayBindings()
{
}

CTestDisplay::~CTestDisplay()
{
}

CTestDisplay* CTestDisplay::clone() const
{
    return new CTestDisplay(*this);
}

// All Test displays are identical by definition.
int CTestDisplay::operator==(const CDisplay& disp)
{ return 1; }

void CTestDisplay::start() { m_alive = true;}
void CTestDisplay::stop() { m_alive = false; }

// By definition, Test displays are alive. They never turn off.
bool CTestDisplay::isAlive() { return m_alive;}
void CTestDisplay::restart() { m_alive = true; }

UInt_t CTestDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer&)
{
    m_DisplayBindings.push_back(rSpectrum.getName());
    m_boundSpectra.push_back(&rSpectrum);

    return m_DisplayBindings.size()-1;
}

void CTestDisplay::removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum)
{
    m_DisplayBindings.at(nSpec) = "";
    m_boundSpectra.at(nSpec) = static_cast<CSpectrum*>(kpNULL);
}

void CTestDisplay::removeSpectrum(CSpectrum &rSpectrum)
{
    SpectrumContainer::iterator it = std::find(m_boundSpectra.begin(),
                                               m_boundSpectra.end(),
                                               &rSpectrum);
    if (it != m_boundSpectra.end()) {
        *it = NULL;
        size_t index = std::distance(m_boundSpectra.begin(), it);
        m_DisplayBindings.at(index) = "";
    }
}

void CTestDisplay::addFit(CSpectrumFit &fit) {
    // get the fit name and spectrum name... both of which we'll need to
    //   ensure we can add/bind the fit.

    string fitName      = fit.fitName();
    string spectrumName = fit.getName();

    Int_t  xSpectrumId  = FindDisplayBinding(spectrumName);
    if (xSpectrumId < 0) {
      // Display is not bound to Xamine.

      return;
    }
    // The display is bound... ensure that our fitlines binding array is large
    // enough.

    while (m_FitlineBindings.size() <=  xSpectrumId) {
      FitlineList empty;
      m_FitlineBindings.push_back(empty);
    }

    // Now we must:
    //  1. Allocate a fitline id.
    //  2. Enter the fit line in Xamine.
    //  3. Add the fitline name/id to our m_FitlineBindings

    int fitId = m_nextFitlineId++;
    pair <int, string> fitInfo(fitId, fitName);
    m_FitlineBindings[xSpectrumId].push_back(fitInfo);

}
void CTestDisplay::deleteFit(CSpectrumFit &fit) {
    string spectrumName =  fit.getName();
    string fitName      = fit.fitName();
    int    xSpectrumId  = FindDisplayBinding(spectrumName);
    if (xSpectrumId >= 0 && ( xSpectrumId <  m_FitlineBindings.size())) {

        // xSpectrumId < 0 means spectrum not bound.
        // xSpectrumId >= size of the bindings vector means no fitlines on spectrum.

        FitlineList::iterator i = m_FitlineBindings[xSpectrumId].begin();
        FitlineList::iterator e = m_FitlineBindings[xSpectrumId].end();
        while (i != e) {
            if (fitName == i->second) {
                // found it.. delete this one and return... don't delete all
                // occurences as
                // a. there's only supposed to be one occurence.
                // b. Depending on the underlying representation of a FitlineList,
                //    deletion may invalidate i.

                m_FitlineBindings[xSpectrumId].erase(i);
                return;
            }
            i++;
        }
        // Falling through here means no matching fit lines...which is a no-op.

    }
}

void CTestDisplay::updateStatistics() {}


std::vector<CGateContainer> CTestDisplay::getAssociatedGates(const std::string &rSpectrum,
                                                         CHistogrammer &rSorter)
{

      // Returns a vector of gates which can be displayed on the spectrum.
      // Gates are considered displayable on a spectrum iff the gate parameter set
      // is the same as the spectrum parameter set. If the spectrum is a gamma
      // spectrum, then the gate is only displayed on one spectrum. Note that
      // displayable gates at present are one of the following types only:
      //   Contour (type = 'c')
      //   Band    (type = 'b')
      //   Cut     (type = 's')
      //   Sum2d   {type = 'm2'}
      //   GammaContour   (type = 'gc')
      //   GammaBand      (type = 'gb')
      //   GammaCut       (type = 'gs')
      // All other gates are not displayable.
      //

      std::vector<CGateContainer> vGates;
    //  CSpectrum *pSpec = m_pSorter->FindSpectrum(rSpectrum);
    //  if(!pSpec) {
    //    throw CDictionaryException(CDictionaryException::knNoSuchKey,
    //                   "No such spectrum CXamine::GatesToDisplay",
    //                   rSpectrum);
    //  }
    //  //
    //  // The mediator tells us whether the spectrum can display the gate:
    //  //
    //  CGateDictionaryIterator pGate = m_pSorter->GateBegin();
    //  while(pGate != m_pSorter->GateEnd()) {
    //    CGateMediator DisplayableGate(((*pGate).second), pSpec);
    //    if(DisplayableGate()) {
    //      vGates.push_back((*pGate).second);
    //    }
    //    pGate++;
    //  }

      return vGates;
}

SpectrumContainer CTestDisplay::getBoundSpectra() const
{
    return m_boundSpectra;
}

CSpectrum* CTestDisplay::getSpectrum(UInt_t xid)
{
    // Returns  a pointer to a spectrum which is bound on a particular
    // xid.
    // Formal Parameters:
    //    UInt_t xid:
    //       The xid.
    // Returns:
    //     POinter to the spectrum or kpNULL Iff:
    //     xid is out of range.
    //     xid does not map to a spectrum.
    //
    if(xid >= DisplayBindingsSize())
      return (CSpectrum*)kpNULL;

    return m_boundSpectra[xid];
}

DisplayBindingsIterator CTestDisplay::DisplayBindingsBegin()
{
    return m_DisplayBindings.begin();
}

DisplayBindingsIterator CTestDisplay::DisplayBindingsEnd()
{
    return m_DisplayBindings.end();
}

UInt_t CTestDisplay::DisplayBindingsSize() const
{
    return m_DisplayBindings.size();
}

/*!
    Test that the spectrum is bound to the display
   \param rSpectrum  : CSpectrum&
       The spectrum
   \return boolean
   \retval false   - Spectrum has no binding.
   \retval true    - Spectrum has binding

*/
bool CTestDisplay::spectrumBound(CSpectrum& rSpectrum) {
    vector<CSpectrum*>::iterator found = std::find(m_boundSpectra.begin(), m_boundSpectra.end(), &rSpectrum);
    return (found != m_boundSpectra.end());
}

Int_t CTestDisplay::FindDisplayBinding(const std::string& spectrumName)
{
    DisplayBindings::iterator found = std::find(m_DisplayBindings.begin(), m_DisplayBindings.end(),
                            spectrumName);
    if (found == m_DisplayBindings.end()) {
        return -1;
    } else {
        return std::distance(m_DisplayBindings.begin(), found);
    }
}

std::string CTestDisplay::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer&)
{
    return rSpectrum.getName();
}

void CTestDisplay::setInfo(CSpectrum &rSpectrum, string name) {}
void CTestDisplay::setTitle(CSpectrum &rSpectrum, string name) {}
UInt_t CTestDisplay::getTitleSize() const { return 0; }

void CTestDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate) {}
void CTestDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer& rGate) {}

void CTestDisplay::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CTestDisplay::setUnderflows(unsigned slot, unsigned x, unsigned y) {}



