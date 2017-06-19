#pragma once

#include "Alignment.h"

#include <deque>
#include <map>
#include <set>

/*
 * Solving the two-dimensional chain problem
 * Adapted from 'Algorithms on Strings, Trees, and Sequences', Gusfield 1997
 * O(n log n)
 */
class FindOptimalChain
{
  class Rect {
  public:
    size_t x1, x2;
    size_t y1, y2;
    int score;
    Rect *prev;

    Rect( size_t x1, size_t x2, size_t y1, size_t y2, size_t score )
      : x1( x1 ), x2( x2 ), y1( y1 ), y2( y2 ), score( score ), prev( NULL )
    {
    }
  };

public:
  FindOptimalChain( const SegmentList &seeds ) {
    std::deque< std::shared_ptr< Rect > > rects;
    std::multimap< size_t, std::shared_ptr< Rect > > points;
    std::multimap< size_t, std::shared_ptr< Rect > > solutions;

    auto greaterOrEqualThan = []( const std::multimap< size_t, std::shared_ptr< Rect > >& set, size_t value ) {
      return set.lower_bound( value );
    };
    auto lessOrEqualThan = []( const std::multimap< size_t, std::shared_ptr< Rect > >& set, size_t value ) {
      if( set.size() == 0 )
        return set.end();

      auto it = set.upper_bound( value );
      while( (*it).first > value && it != set.begin() ) {
        it--;
      }

      if( (*it).first > value )
        return set.end();

      return it;
    };

    for( auto &seed : seeds ) {
      /* std::cout << "Seed " << seed.s1 << " " << seed.s2 << " len " << seed.length << std::endl; */
      std::shared_ptr< Rect > rect( new Rect( seed.s1, seed.s1 + seed.length, seed.s2, seed.s2 + seed.length, seed.length ) );
      rects.push_back( rect );
    }

    // For the same x-value, x2 (rectangle end) needs to come before x1 (rectangle start)
    // (since we need the previous rect in the x1 case)
    // Insertion order is guaranteed in C++11
    for( auto &rect : rects ) {
      points.insert( std::pair< size_t, std::shared_ptr< Rect > >( rect->x2, rect ) );
    }
    for( auto &rect : rects ) {
      points.insert( std::pair< size_t, std::shared_ptr< Rect > >( rect->x1, rect ) );
    }

    // Go through each point, from left to right
    for( auto &p : points ) {

      size_t px = p.first;
      std::shared_ptr< Rect > rect = p.second;

#ifndef NDEBUG
      std::cout << "(px " << px << ") Rect " << rect->x1 << " "
        << rect->y1 << " "
        << rect->x2 << " "
        << rect->y2 << " "
        << " score: " << rect->score;
#endif

      if( px == rect->x1 ) {
        // Left end of rectangle
#ifndef NDEBUG
        std::cout << " (left end)" << std::endl;
#endif

        // Find the closest solution which is <= y1
        auto closest = lessOrEqualThan( solutions, rect->y1 );
        if( closest != solutions.end() ) {
          std::shared_ptr< Rect > closestRect = (*closest).second;
          rect->prev = closestRect.get();
          rect->score += closestRect->score;
#ifndef NDEBUG
          std::cout << " Closest entry found " <<
            (*closest).second->x1 << " " <<
            (*closest).second->y1 << " " <<
            (*closest).second->x2 << " " <<
            (*closest).second->y2 << " " << std::endl;
          std::cout << " New score " << rect->score << std::endl;
#endif
        }
      } else {
        // Right end of rectangle
#ifndef NDEBUG
        std::cout << " (right end)" << std::endl;
#endif

        // Find competing solutions
        auto competitor = greaterOrEqualThan( solutions, rect->y2 );
        solutions.insert( competitor, std::pair< size_t, std::shared_ptr< Rect > >( rect->y2, rect ) );

        // Delete competing solutions this one has beat (higher up but lower score)
        auto it = competitor;
        while( it != solutions.end() ) {
          if( (*it).second->score < rect->score ) {
            it = solutions.erase( it );
          } else {
            it++;
          }
        }
      }
    }

    Rect *rect = (*solutions.rbegin()).second.get();
    mOptimalChain.clear();
    while( rect ) {
      mOptimalChain.push_back( Segment( rect->x1, rect->y1, rect->x2 - rect->x1 ) );
      rect = rect->prev;
    }

    std::reverse( mOptimalChain.begin(), mOptimalChain.end() );

#ifndef NDEBUG
    std::cout << "Optimal Chain " << std::endl;;
    for( auto& s : mOptimalChain ) {
      std::cout << " " << s.s1 << " " << s.s2 << " " << std::endl;
    }
#endif

  };

  SegmentList OptimalChain() const {
    return mOptimalChain;
  };

private:
  SegmentList mOptimalChain;
};
