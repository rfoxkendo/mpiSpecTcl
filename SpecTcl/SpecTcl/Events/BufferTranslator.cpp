/******************************************************************************
  Implementation of class BufferTranslator
******************************************************************************/

#include <buffer.h>
#include <histotypes.h>
#include "BufferTranslator.h"

/*-----------------------------------------------------------------------------
  Name:  TranslatorFactory::CreateTranslator

  Purpose:  Compares endianess of the buffer with endianess of the running
            system. Returns a SwappingBufferTranslator if different, and a 
	    NonSwappingBufferTranslator if they are the same.
-----------------------------------------------------------------------------*/

BufferTranslator* TranslatorFactory::CreateTranslator
( Address_t pBuffer, Endian eBufferOrdering) 
{
  if( MyEndianess() != eBufferOrdering ) {
    return new SwappingBufferTranslator( pBuffer );
  }
  else {
    return new NonSwappingBufferTranslator( pBuffer );
  }
}

/*-----------------------------------------------------------------------------
  Name:  MyEndianess

  Purpose:  Determine the running system endianess
-----------------------------------------------------------------------------*/

TranslatorFactory::Endian MyEndianess()
{
  Int_t x = 1;
  if ( *(Char_t *)&x == x ) {
    return TranslatorFactory::little;
  }
  
  else {
    return TranslatorFactory::big;
  }
}

/*-----------------------------------------------------------------------------
  Name:  BufferFactory::CreateBuffer

  Purpose:  Examine the lsignature of a buffer header and return a reference
            to the appropriate BufferTranslator.
-----------------------------------------------------------------------------*/

BufferTranslator* BufferFactory::CreateBuffer( Address_t pBuffer,
					       Int_t Signature32 )
{
  BufferTranslator* pTranslator;

  if( Signature32 == 0x01020304 ) {
    pTranslator = TranslatorFactory::CreateTranslator
      ( pBuffer, TranslatorFactory::little );
  }
  else {
    pTranslator = TranslatorFactory::CreateTranslator
      ( pBuffer, TranslatorFactory::big );
  }
  return pTranslator;
}

/*-----------------------------------------------------------------------------
  Name:  SwappingBufferTranslator::GetBlock

  Purpose:  Copy memory from buffer and return swapped
-----------------------------------------------------------------------------*/

void SwappingBufferTranslator::GetBlock( const Address_t pItem, 
					 Int_t size, Int_t iOffset )
{
  UChar_t *pBuff = (UChar_t *) m_pBuffer;
  UChar_t *Item = (UChar_t *) pItem;

  for( Int_t I = 0; I < size; I++ ) {
    Item[I] = pBuff[iOffset+I];
  }

  UChar_t *a = (UChar_t *) pItem, b;
  for( Int_t I = 0; I < size/2; I++ ) {
    b = a[I];
    a[I] = a[(size-1) - I];
    a[(size-1) - I] = b;
  }
}

/*-----------------------------------------------------------------------------
  Name:  NonSwappingBufferTranslator::GetBlock

  Purpose:  Copy memory from buffer and return
-----------------------------------------------------------------------------*/

void NonSwappingBufferTranslator::GetBlock( const Address_t pItem, 
					    Int_t size, Int_t iOffset )
{
  UChar_t *pBuff = (UChar_t *) m_pBuffer;
  UChar_t *Item = (UChar_t *) pItem;
  
  for( Int_t I = 0; I < size; I++ ) {
    Item[I] = pBuff[iOffset+I];
  }
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetByte

  Purpose:  Get the byte at offset iOffset from the buffer
-----------------------------------------------------------------------------*/

UChar_t BufferTranslator::GetByte( UInt_t iOffset )
{
  UChar_t Byte = Get<UChar_t>( iOffset );
  return Byte;  
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetWord

  Purpose:  Get the word at offset iOffset from the buffer and byteswap it
            if necessary
-----------------------------------------------------------------------------*/

Short_t BufferTranslator::GetWord( UInt_t iOffset )
{
  Short_t Word = Get<Short_t>( iOffset );
  return Word;
}

/*-----------------------------------------------------------------------------
  Name:  BufferTranslator::GetLongword

  Purpose:  Get the longword at offset iOffset from the buffer and byteswap it
            if necessary
-----------------------------------------------------------------------------*/

Long_t BufferTranslator::GetLongword( UInt_t iOffset )
{
  Long_t Longword = Get<Long_t>( iOffset );
  return Longword;
}
