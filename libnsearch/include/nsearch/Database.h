#pragma once

#include <unordered_map>
#include <set>

#include "Sequence.h"
#include "Utils.h"
#include "Aligner.h"
#include "Kmer.h"

#include "Alignment/Seed.h"
#include "Alignment/Ranges.h"
#include "Alignment/HitTracker.h"
#include "Alignment/OptimalChainFinder.h"
#include "Alignment/DP.h"

class Database {
  typedef std::pair< size_t, const Sequence* > SequenceInfo;

public:
  Database( int wordSize )
    : mWordSize( wordSize )
  {
    // Sequences have to fit in the calculated hash
    assert( wordSize * 2 <= sizeof( size_t ) * 8 );
  }

  void AddSequence( const Sequence &seq ) {
    // Save
    mSequences.push_back( seq );
    const Sequence *ref = &mSequences.back();

    // Map words
    Kmers kmers( *ref, mWordSize );
    kmers.ForEach( [&]( const Sequence &kmer, size_t pos ) {
      this->mWords[ kmer ].push_back( SequenceInfo( pos, ref ) );
    });
  }

  SequenceList Query( const Sequence &query, int maxHits = 10 ) const {
    std::unordered_map< const Sequence*, HitTracker > candidates;

    // Go through each kmer, find candidates
    Kmers kmers( query, mWordSize );
    kmers.ForEach( [&]( const Sequence &kmer, size_t pos ) {
      for( auto &seqInfo : mWords.at( kmer ) ) {
        size_t candidatePos = seqInfo.first;
        const Sequence *candidateSeq = seqInfo.second;

        candidates[ candidateSeq ].AddHit( pos, candidatePos, mWordSize );
      }
    });

    // Compute the optimal chain for each candidate
    std::multimap< OptimalChainFinder, const Sequence* > highscore;

    // Sort candidates based on the score of the optimal chain
    for( auto &c : candidates ) {
      const Sequence *seq = c.first;
      const HitTracker &hitTracker = c.second;

      // Compute optimal chain
      OptimalChainFinder ocf( hitTracker.Seeds() );
      highscore.insert( std::pair< OptimalChainFinder, const Sequence* >( ocf, seq ) );
    }

    // Now align the candidates
    AlignmentParams ap;
    ap.matchScore = 2;
    ap.mismatchScore = -3;
    ap.interiorGapOpenPenalty = ap.terminalGapOpenPenalty = 5;
    ap.interiorGapExtensionPenalty = ap.terminalGapExtensionPenalty = 2;

    static GuidedBandedGlobalAlign *dp = NULL;
    SequenceList list;
    for( auto it = highscore.rbegin(); it != highscore.rend(); ++it ) {
      const OptimalChainFinder &ocf = it->first;
      const Sequence &reference = *it->second;

      if( dp == NULL ) {
        dp = new GuidedBandedGlobalAlign( query, reference, ap, 16, ocf.OptimalChain() );
      }
      dp->ComputeMatrix();
      list.push_back( *(*it).second );

      if( list.size() >= maxHits )
        break;
    }

    return list;
  }

  size_t NumWords() const {
    return mWords.size();
  }

private:
  int mWordSize;
  SequenceList mSequences;
  std::unordered_map< Sequence, std::deque< SequenceInfo > > mWords;
};
