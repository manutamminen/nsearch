#pragma once

#include "Sequence.h"
#include "Utils.h"

#define BIT_INDEX(pos) ( (pos) * 2 ) % ( sizeof( size_t ) * 8 )
#define BASE_VALUE(base) \
    ( (base) == 'A' ? 0b00 : \
      ( (base) == 'C' ? 0b01 : \
        ( (base) == 'T' || (base) == 'U' ) ? 0b10 : \
          ( (base) == 'G' ? 0b11 : (-1) ) \
      ) \
    )

class HashWords {
public:
  using Callback = const std::function< void( size_t, uint32_t ) >;

public:
  HashWords( const Sequence &ref, size_t wordSize )
    : mRef( ref )
  {
    mWordSize = std::min( wordSize, mRef.Length() );
  }

  size_t Length() const {
    return mWordSize;
  }

  void ForEach( const Callback &block ) const {
    const char *data = mRef.sequence.data();
    const char *ptr = data;

    // First word
    size_t lastAmbigIndex = ( size_t )-1;
    size_t word = 0;
    for( size_t k = 0; k < mWordSize; k++ ) {
      int8_t val = BASE_VALUE( *ptr );
      if( val < 0 ) {
        lastAmbigIndex = k;
      } else {
        word |= ( val << BIT_INDEX( k ) );
      }
      ptr++;
    }

    if( lastAmbigIndex == ( size_t )-1 )
      block( 0, word );

    // For each, shift window by one
    size_t cols = mRef.Length() - mWordSize;
    for( size_t i = 1; i <= cols; i++ ) {
      word >>= 2;
      int8_t val = BASE_VALUE( *ptr );
      if( val < 0 ) {
        lastAmbigIndex = i;
      } else {
        word |= ( val << BIT_INDEX( mWordSize - 1 ) );
      }

      if( lastAmbigIndex == (size_t)-1 || i - lastAmbigIndex >= mWordSize ) {
        block( i, word );
      }

      ptr++;
    }
  }

private:
  size_t mWordSize;
  const Sequence &mRef;
};
