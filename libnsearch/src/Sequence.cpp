#include "nsearch/Sequence.h"
#include "nsearch/Utils.h"

#include <cassert>
#include <ctype.h>
#include <iostream>

Sequence::Sequence() : Sequence( "", "", "" ) {}

Sequence::Sequence( const Sequence& sequence )
    : sequence( sequence.sequence ), identifier( sequence.identifier ),
      quality( sequence.quality ) {}

Sequence::Sequence( Sequence&& sequence )
    : sequence( std::move( sequence.sequence ) ),
      identifier( std::move( sequence.identifier ) ),
      quality( std::move( sequence.quality ) ) {}

Sequence& Sequence::operator=( const Sequence& other ) {
  sequence   = other.sequence;
  identifier = other.identifier;
  quality    = other.quality;
  return *this;
}

Sequence::Sequence( const std::string& sequence )
    : Sequence( "", sequence, "" ) {}

Sequence::Sequence( const char* sequence ) : Sequence( "", sequence, "" ) {}

Sequence::Sequence( const std::string& identifier, const std::string& sequence )
    : Sequence( identifier, sequence, "" ) {}

Sequence::Sequence( const std::string& identifier, const std::string& sequence,
                    const std::string& quality )
    : identifier( identifier ), sequence( sequence ), quality( quality ) {}

size_t Sequence::Length() const {
  return sequence.length();
}

Sequence Sequence::Subsequence( const size_t pos, const size_t len_ ) const {
  size_t len = len_;
  if( len == std::string::npos ) {
    len = Length() - pos;
  }

  return Sequence( identifier,
                   pos < sequence.length() ? sequence.substr( pos, len ) : "",
                   pos < quality.length() ? quality.substr( pos, len ) : "" );
}

Sequence Sequence::operator+( const Sequence& other ) const {
  return Sequence( identifier, sequence + other.sequence,
                   quality + other.quality );
}

char& Sequence::operator[]( const size_t index ) {
  assert( index >= 0 && index < sequence.size() );
  return sequence[ index ];
}

char Sequence::operator[]( const size_t index ) const {
  assert( index >= 0 && index < sequence.size() );
  return sequence[ index ];
}

bool Sequence::operator==( const Sequence& other ) const {
  return !( *this != other );
}

bool Sequence::operator!=( const Sequence& other ) const {
  if( Length() != other.Length() )
    return true;

  auto tit = ( *this ).sequence.begin();
  auto oit = other.sequence.begin();
  while( tit != ( *this ).sequence.end() && oit != other.sequence.end() ) {
    if( !DoNucleotidesMatch( *tit, *oit ) )
      return true;

    ++tit;
    ++oit;
  }

  return false;
}

Sequence Sequence::Complement() const {
  Sequence complement = *this;

  for( char& ch : complement.sequence ) {
    ch = NucleotideComplement( ch );
  }

  return complement;
}

Sequence Sequence::Reverse() const {
  Sequence rev = *this;
  // Reverse sequence and quality
  std::reverse( rev.sequence.begin(), rev.sequence.end() );
  std::reverse( rev.quality.begin(), rev.quality.end() );
  return rev;
}

Sequence Sequence::ReverseComplement() const {
  return Reverse().Complement();
}
