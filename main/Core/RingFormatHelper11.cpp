/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
/**
 * @file RingFormatHelper11.cpp
 * @brief Implement ring format helper for nscldaq-11.x+
 */
#include "RingFormatHelper11.h"
#include "DataFormat.h"
#include <BufferTranslator.h>


/**
 * hasBodyHeader
 *
 * Determines if a ring item has a body header.  A body header is present
 * if its size is nonzero.  See DataFormat.h for more information about
 * BodyHeader and the structure of data used here.
 *
 * @param pItem - Actually a pRingItem pointing to the item to decode.
 *
 * @return bool
 * @retval true - the item has a nonempty body header.
 * @retval false - the item has no body header data.
 */
 bool
 CRingFormatHelper11::hasBodyHeader(void* pItem) const
 {
    pRingItem p = reinterpret_cast<pRingItem>(pItem);
    
    // This also works in 12.
    
    return (p->s_body.u_noBodyHeader.s_mbz > sizeof(uint32_t));
 }
 /**
  * getBodyPointer
  *
  * Returns a pointer to the payload of the ring item.  The payload
  * does not include any body header.
  *
  * @param pItem - Pointer to the item, really a  pRingItem.
  *
  * @return void* Pointer to the body.
  */
 void*
 CRingFormatHelper11::getBodyPointer(void* pItem)
 {
    pRingItem p = reinterpret_cast<pRingItem>(pItem);
    
    if (hasBodyHeader(pItem)) {
        // If there's a body header, and this is a physics event item,
        // save the source id so we know which sid to use for
        // event counting.
        
        if (p->s_header.s_type == PHYSICS_EVENT) m_glomSourceId =
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId;

           // The code below will work if the body header has been extended
           // (e.g. by the software trigger framework (NSCLDAQ-11.4 and later):
          
           uint32_t bheaderSize =  p->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
           uint8_t* b           =
             reinterpret_cast<uint8_t*>(&(p->s_body.u_hasBodyHeader.s_bodyHeader));
           b                   += bheaderSize;
           return reinterpret_cast<void*>(b);
				 
    } else {
          if (p->s_header.s_type == PHYSICS_EVENT) m_glomSourceId = 0;  // don't know.
          return reinterpret_cast<void*>(p->s_body.u_noBodyHeader.s_body);
    }
 }
 /**
  * getBodyHeaderPointer
  *
  * Return a pointer to the body header.  Note that if hasBodyHeader would
  * return false, a null pointer is returned.
  *
  * @param pItem - Pointer to the item (actually a pRingitem)
  *
  * @return void* Pointer to the body header.
  * @return null - No body header.
  */
 void*
 CRingFormatHelper11::getBodyHeaderPointer(void* pItem)
 {
    pRingItem  p = reinterpret_cast<pRingItem>(pItem);
    
    if (hasBodyHeader(pItem)) {
        return reinterpret_cast<void*>(&(p->s_body.u_hasBodyHeader.s_bodyHeader));
    } else {
        return reinterpret_cast<void*>(0);
    }
 }
/**
 *  getSourceId
 *    @param pItem - pointer to the full item.
 *    @param pTranslator - buffer translator.
 *    @return uint32_t - Return the source Id from the body header.
 *    @retval 0 - if there is no body header.
 */
uint32_t
CRingFormatHelper11::getSourceId(void* pItem, BufferTranslator* pTranslator)
{
  uint32_t result  = 0;
  if (hasBodyHeader(pItem)) {
    BodyHeader *pHeader = reinterpret_cast<BodyHeader*>(getBodyHeaderPointer(pItem));
    result =  pTranslator->GetLongword(pHeader->s_sourceId);
  }
  return result;
}
 /*------------------------------------------------------------------------
  * Private utility methods.
  */

// methods specific to state transition items.
  
/**
 * getTitle
 *   Return the title of a state transition item.
 *
 *  @param pItem - Pointer to the ring item.
 *  
 * @return std::string - run title.
 * @throw  std::string if the item is not a run state transition.
 */
std::string
CRingFormatHelper11::getTitle(void* pItem)
{
    if (isStateTransition(pItem)) {
        pStateChangeItemBody p =
            reinterpret_cast<pStateChangeItemBody>(getBodyPointer(pItem));
        return std::string(p->s_title);
    } else {
        throw std::string("CRingFormatHelper11::getTitle - not state transition");
    }
}
/**
 * getRunNumber
 *
 * Return the run number from a state transition item.
 *
 * @param pItem       - pointer to the item.
 * @param pTranslator - pointer to the byte order converter appropriate to the sourc.
 *
 * @return unsigned - Run number.
 * @throw std::string if the item is not a state transition item.
 */
unsigned
CRingFormatHelper11::getRunNumber(void* pItem, BufferTranslator* pTranslator)
{
    if (isStateTransition(pItem)) {
      pStateChangeItemBody p = reinterpret_cast<pStateChangeItemBody>(getBodyPointer(pItem));
        return pTranslator->TranslateLong(p->s_runNumber);
    } else {
        throw std::string("CRingFormatHelper11::getRunNumber - not state Transition");
    }
}

// Methods specific to string lists:

/**
 * getStringCount
 *    Get the number of string items in a TextItem.
 * @param pItem           - Pointer to the item.
 * @param pTranslator     - Pointer to the byte order converter.
 *
 * @return unsigned - Number of scalers.
 * @throw std::string - If the item is not a text item.
 */
unsigned
CRingFormatHelper11::getStringCount(void* pItem, BufferTranslator* pTranslator)
{
    if (isTextItem(pItem)) {
      pTextItemBody p = reinterpret_cast<pTextItemBody>(getBodyPointer(pItem));
        return pTranslator->TranslateLong(p->s_stringCount);
    } else {
        throw std::string("CRingFormatHelper11::getStringCount - not a text item.");
    }
}
/**
 * getStrings
*    Marshall the strings in a text item into a vector.
* @param pItem - pointer to the item.
* @param pTranslator - pointer to the byte order translator.
* @return std::vector<std::string> - the strings.
*/
std::vector<std::string>
CRingFormatHelper11::getStrings(void* pItem, BufferTranslator* pTranslator)
{
    // This throws if the item is not a text itme:
    
    unsigned n = getStringCount(pItem, pTranslator);
    pTextItemBody pBody =
        reinterpret_cast<pTextItemBody>(getBodyPointer(pItem));
    std::vector<std::string> result = stringListToVector(n, pBody->s_strings);
    return result;
}
// Methods specific to scaler items.
/**
 * getScalerCount
 *
 * Returns the number of scalers in a scaler item.
 *
 * @param pItem        - Pointer to the item.
 * @param pTranslator  - Pointer to the appropriate byte order translator.
 *
 * @return unsigned - number of scalers.
 * @throw std::string  not a scaler item.
 */
unsigned
CRingFormatHelper11::getScalerCount(void* pItem, BufferTranslator* pTranslator)
{
    if (isScalerItem(pItem)) {
      pScalerItemBody p = reinterpret_cast<pScalerItemBody>(getBodyPointer(pItem));
      return p->s_scalerCount;
    } else {
        throw std::string(
            "CRingFormatHelper11::getScalerCount - not a scaler item."
        );
    }
}
/**
 * getScalers
 *     Marshals the scalers counts from a scaler item into a vector.
 * @param pItem - pointer to the item.
 * @param pTranslator - pointer to the byte order translator.
 * @return std::vector<uint32_t>
 * @note no account is taken of 24 bit scalers that might have trash in the
 *       upper bits.
 */
std::vector<uint32_t> CRingFormatHelper11::getScalers(
     void* pItem, BufferTranslator* pTranslator
)
{
     // This throws if the item is not a scaler item.
    
     unsigned n = getScalerCount(pItem, pTranslator);
     
     pScalerItemBody pBody =
         reinterpret_cast<pScalerItemBody>(getBodyPointer(pItem));
     std::vector<uint32_t> result = marshallScalers(
         n, pBody->s_scalers, pTranslator
     );
     return result;
}
/**
 * getScalerOriginalSourceId
 *   @param pItem - pointer to the item.
 *   @param pTranslator - buffer translator
 *   @return uint32_t - V11 does not have original source ids so we return
 *                      the body header source id:
 */
uint32_t
CRingFormatHelper11::getScalerOriginalSourceId(void* pItem, BufferTranslator* pTranslator)
{
  return getSourceId(pItem, pTranslator);
}

// Methods specific to trigger count items.

/**
 * getTriggerCount
 *
 * Return the number of triggers seen so far.
 *
 * @param pItem       - Pointer to the item.
 * @param pTranslator - Pointer to the appropriate translator.
 *
 * @return uint64_t
 * @throw std::string if this is not a trigger item.
 */
uint64_t
CRingFormatHelper11::getTriggerCount(void* pItem, BufferTranslator* pTranslator)
{
    if (isTriggerCountItem(pItem)) {
        pPhysicsEventCountItemBody p =
            reinterpret_cast<pPhysicsEventCountItemBody>(getBodyPointer(pItem));
            
        // If there's no body header, we just hand back the event count from the
        // body. If there is a body header, we'll hand back the prior value. unless
        // the source id matches m_glomSourceId .
        
        if (hasBodyHeader(pItem)) {
            pBodyHeader phdr =
                reinterpret_cast<pBodyHeader>(getBodyHeaderPointer(pItem));
            if (phdr->s_sourceId == m_glomSourceId) {
                m_nLastEventCount = pTranslator->getQuad(p->s_eventCount);
            }
            return m_nLastEventCount;
        } else {
            return pTranslator->getQuad(p->s_eventCount);
        }
    } else {
        throw std::string("CRingFormatHelper11::getTriggerCount - not trigger count item");
    }
}
/*-----------------------------------------------------------------------------
 * Private utilities
 */

/**
 * isStateTransition
 *
 * @param pItem - pointer to an item.
 * @return bool - true if pItem is a state transition item.
 */
bool
CRingFormatHelper11::isStateTransition(void* pItem)
{
    uint16_t type = itemType(pItem);
    
    return (
        (type ==  BEGIN_RUN)
        || (type == END_RUN)
        || (type == PAUSE_RUN)
        || (type == RESUME_RUN)
    );
}
/**
 * isTextitem
 *
 * @param pItem - pointer to a ring item.
 * @return bool - true if the item is a text item.
 */
bool
CRingFormatHelper11::isTextItem(void* pItem)
{
    uint16_t type = itemType(pItem);
    
    return (
        (type == PACKET_TYPES)
        || (type == MONITORED_VARIABLES)
    );
}
/**
 * isScalerItem
 *
 * @param pItem - pointer to a ring item.
 * @return bool - True if the item type is PERIODIC_SCALERS
 */
bool
CRingFormatHelper11::isScalerItem(void* pItem)
{
    return itemType(pItem) == PERIODIC_SCALERS;
}
/**
 * isTriggerItem
 *
 * @param pItem - Pointer to a ring item.
 * @return bool - true if the item is a PHYSICS_EVENT_COUNT item.
 */
bool
CRingFormatHelper11::isTriggerCountItem(void* pItem)
{
    return itemType(pItem) == PHYSICS_EVENT_COUNT;
}
