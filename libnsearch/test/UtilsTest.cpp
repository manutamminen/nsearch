#include <catch.hpp>

#include <nsearch/Utils.h>

TEST_CASE( "Utils" ) {
  SECTION( "IsBlank" ) {
    REQUIRE( IsBlank( "" ) == true );
    REQUIRE( IsBlank( " " ) == true );
    REQUIRE( IsBlank( "\r\n" ) == true );
    REQUIRE( IsBlank( "!" ) == false );
    REQUIRE( IsBlank( "\r\nA" ) == false );
  }

  SECTION( "UpcaseString" ) {
    std::string str = "AcGt";
    UpcaseString( str );
    REQUIRE( str == "ACGT" );
  }

  SECTION( "DoNucleotidesMatch" ) {
    REQUIRE( DoNucleotidesMatch( 'A', 'A' ) == true );
    REQUIRE( DoNucleotidesMatch( 'C', 'T' ) == false );
    REQUIRE( DoNucleotidesMatch( 'G', 'R' ) == true );
    REQUIRE( DoNucleotidesMatch( 'A', 'Y' ) == false );
    REQUIRE( DoNucleotidesMatch( 'M', 'Y' ) == true );
  }

  SECTION( "NucleotideComplement" ) {
    REQUIRE( NucleotideComplement( 'A' ) == 'T' );
    REQUIRE( NucleotideComplement( 'U' ) == 'A' );
    REQUIRE( NucleotideComplement( 'R' ) == 'Y' );
    REQUIRE( NucleotideComplement( 'N' ) == 'N' );
  }
}
