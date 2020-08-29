/** @file
 *
 * @par History
 */

#include "uofile.h"

#include <cstdio>
#include <fstream>
#include <string>

#include "../clib/fileutil.h"
#include "../clib/logfacility.h"
#include "../clib/strutil.h"
#include "pol/objtype.h"
#include "systemstate.h"

namespace Pol
{
namespace Plib
{
FILE* mapfile = nullptr;
FILE* sidxfile = nullptr;
FILE* statfile = nullptr;
FILE* verfile = nullptr;
FILE* tilefile = nullptr;
FILE* stadifl_file = nullptr;
FILE* stadifi_file = nullptr;
FILE* stadif_file = nullptr;
FILE* mapdifl_file = nullptr;
FILE* mapdif_file = nullptr;

std::ifstream uopmapfile;

bool open_uopmap_file( const int mapid, int* out_file_size = nullptr )
{
  std::string filepart = "map" + std::to_string( mapid ) + "LegacyMUL.uop";
  std::string filename = systemstate.config.uo_datafile_root + filepart;
  if ( !Clib::FileExists( filename ) )
  {
    INFO_PRINT << filepart << " not found in " << systemstate.config.uo_datafile_root
               << ". Searching for old map[N].mul files.\n";
    return false;
  }

  uopmapfile.open( filename, std::ios::binary );

  if ( (bool)uopmapfile )
  {
    if ( out_file_size != nullptr )
    {
      *out_file_size = Clib::filesize( filename.c_str() );
    }
    return true;
  }

  return false;
}

FILE* open_uo_file( const std::string& filename_part, int* out_file_size = nullptr )
{
  std::string filename = systemstate.config.uo_datafile_root + filename_part;
  FILE* fp = fopen( filename.c_str(), "rb" );
  if ( !fp )
  {
    ERROR_PRINT << "Unable to open UO datafile: " << filename << "\n"
                << "POL.CFG specifies UODataFileRoot as '" << systemstate.config.uo_datafile_root
                << "'.  Is this correct?\n"
                << "  The following files must be present in that directory:\n"
                << "      map0.mul OR map0LegacyMUL.uop\n"
                << "      multi.idx\n"
                << "      multi.mul\n"
                << "      staidx0.mul\n"
                << "      statics0.mul\n"
                << "      tiledata.mul\n"
                << "      verdata.mul    (optional - only if present on client install)\n";

    throw std::runtime_error( "Error opening UO datafile." );
  }
  if ( out_file_size != nullptr )
  {
    *out_file_size = Clib::filesize( filename.c_str() );
  }
  return fp;
}

FILE* open_map_file( std::string name, int map_id, int* out_file_size = nullptr )
{
  std::string filename;

  filename = name + Clib::tostring( map_id ) + ".mul";
  if ( uo_mapid == 1 && !Clib::FileExists( systemstate.config.uo_datafile_root + filename ) )
  {
    ERROR_PRINT << "Unable to find UO file: " << filename
                << ", reading " + name + "0.mul instead.\n";
    filename = name + "0.mul";
  }

  return open_uo_file( filename, out_file_size );
}

int uo_mapid = 0;
int uo_usedif = 1;
bool uo_readuop = true;

unsigned short uo_map_width = 0;
unsigned short uo_map_height = 0;

void open_tiledata( void )
{
  int tiledata_size;
  size_t nblocks;

  tilefile = open_uo_file( "tiledata.mul", &tiledata_size );

  if ( ( tiledata_size - 428032 ) % 1188 != 0 && ( tiledata_size - 493568 ) % 1316 == 0 )
  {
    cfg_use_new_hsa_format = true;
    nblocks = ( tiledata_size - 493568 ) / 1316;
  }
  else
  {
    cfg_use_new_hsa_format = false;
    nblocks = ( tiledata_size - 428032 ) / 1188;
  }

  // Save the parameters into this ugly global state we have
  Plib::systemstate.config.max_tile_id = 32 * nblocks - 1;

  INFO_PRINT << "Converting with parameters: UseNewHSAFormat = "
             << ( cfg_use_new_hsa_format ? "True" : "False" )
             << ", MaxTileId = " << Clib::hexint( Plib::systemstate.config.max_tile_id ) << "\n";
}

void open_map( void )
{
  int map_size;
  // First tries to load the new UOP files. Otherwise fall back to map[N].mul files.
  // map1 uses map0 + 'dif' files, unless there is a map1.mul (newer clients)
  // same for staidx and statics

  if ( !uo_readuop || !open_uopmap_file( uo_mapid, &map_size ) )
    mapfile = open_map_file( "map", uo_mapid, &map_size );

  if ( ( uo_mapid == 0 || uo_mapid == 1 ) && ( uo_map_width == 0 || uo_map_height == 0 ) )
  {
    bool use_legacy_dimensions = ( map_size / 196 ) == 393216;
    if ( use_legacy_dimensions )
    {
      uo_map_width = 6144;
      uo_map_height = 4096;
    }
    else
    {
      uo_map_width = 7168;
      uo_map_height = 4096;
    }
    INFO_PRINT << "Using calculated map dimensions: " << uo_map_width << "x" << uo_map_height
               << "\n";
  }
}

void open_uo_data_files( void )
{
  std::string filename;

  open_map();

  sidxfile = open_map_file( "staidx", uo_mapid );
  statfile = open_map_file( "statics", uo_mapid );

  if ( Clib::FileExists( ( systemstate.config.uo_datafile_root + "verdata.mul" ).c_str() ) )
  {
    verfile = open_uo_file( "verdata.mul" );
  }
  else
  {
    verfile = nullptr;
  }
  open_tiledata();

  if ( uo_usedif )
  {
    filename = "stadifl" + Clib::tostring( uo_mapid ) + ".mul";
    if ( Clib::FileExists( systemstate.config.uo_datafile_root + filename ) )
    {
      stadifl_file = open_uo_file( filename );
      filename = "stadifi" + Clib::tostring( uo_mapid ) + ".mul";
      stadifi_file = open_uo_file( filename );
      filename = "stadif" + Clib::tostring( uo_mapid ) + ".mul";
      stadif_file = open_uo_file( filename );
    }
    filename = "mapdifl" + Clib::tostring( uo_mapid ) + ".mul";
    if ( Clib::FileExists( systemstate.config.uo_datafile_root + filename ) )
    {
      mapdifl_file = open_uo_file( filename );
      filename = "mapdif" + Clib::tostring( uo_mapid ) + ".mul";
      mapdif_file = open_uo_file( filename );
    }
  }
}
}  // namespace Plib
}  // namespace Pol
