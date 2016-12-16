/** @file
 *
 * @par History
 */


#include "testenv.h"

#include "../../clib/logfacility.h"
#include "../../plib/mapserver.h"
#include "../globals/uvars.h"
#include "../item/item.h"
#include "../los.h"
#include "../mobile/npc.h"
#include "../realms/realm.h"
#include "../uconst.h"
#include "../uobject.h"

namespace Pol
{
namespace Testing
{
namespace
{
void test_los( Core::UObject* src, Core::UObject* target, bool should_have_los )
{
  INFO_PRINT << "LOS test: " << src->name() << " to " << target->name() << "(" << should_have_los
             << "):";
  bool res = Core::gamestate.main_realm->has_los( *src, *target );
  INFO_PRINT << res << " ";
  if ( should_have_los == res )
  {
    INFO_PRINT << "Ok!\n";
    inc_successes();
  }
  else
  {
    INFO_PRINT << "Failure!\n";
    inc_failures();
  }
}

void test_los( u16 x1, u16 y1, s8 z1, u16 x2, u16 y2, s8 z2, bool should_have_los )
{
  INFO_PRINT << "LOS test: (" << x1 << "," << y1 << "," << int( z1 ) << ")"
             << " - (" << x2 << "," << y2 << "," << int( z2 ) << ")"
             << " (" << should_have_los << "):";
  auto main_realm = Core::gamestate.main_realm;
  bool res = main_realm->has_los( Core::LosObj( x1, y1, z1, main_realm ),
                                    Core::LosObj( x2, y2, z2, main_realm ) );
  INFO_PRINT << res << " ";
  if ( should_have_los == res )
  {
    INFO_PRINT << "Ok!\n";
    inc_successes();
  }
  else
  {
    INFO_PRINT << "Failure!\n";
    inc_failures();
  }
}

void test_los( u16 x1, u16 y1, s8 z1, u8 h1, u16 x2, u16 y2, s8 z2, u8 h2,
                      bool should_have_los )
{
  INFO_PRINT << "LOS test: (" << x1 << "," << y1 << "," << int( z1 ) << ",ht=" << int( h1 ) << ")"
             << " - (" << x2 << "," << y2 << "," << int( z2 ) << ",ht=" << int( h2 ) << ")"
             << " (" << should_have_los << "):";
  auto main_realm = Core::gamestate.main_realm;
  bool res = main_realm->has_los( Core::LosObj( x1, y1, z1, main_realm ),
                                    Core::LosObj( x2, y2, z2, main_realm ) );
  INFO_PRINT << res << " ";
  if ( should_have_los == res )
  {
    INFO_PRINT << "Ok!\n";
    inc_successes();
  }
  else
  {
    INFO_PRINT << "Failure!\n";
    inc_failures();
  }
}
} // namespace

void los_test()
{
  INFO_PRINT << "POL datafile LOS tests:\n";

  test_los( 5421, 78, 10, 5422, 89, 20, false );
  test_los( 1403, 1624, 28, 1402, 1625, 28, true );

  test_los( 1374, 1625, 59, 1379, 1625, 30, true );
  test_los( 1373, 1625, 59, 1379, 1625, 30, true );
  test_los( 1372, 1625, 59, 1379, 1625, 30, false );
  test_los( 1372, 1625, 59, 1381, 1625, 30, true );

  // from you standing in the house in GA to a backpack outside
  test_los( 5987, 1999, 7 + 9, 5991, 2001, 1, false );
  test_los( 5987, 1999, 7 + 9, 5993, 1997, 1, false );

  // test looking through "black space" in a cave
  test_los( 5583, 226, 0, 5590, 223, 0, false );

  test_los( test_banker2, test_chest1, true );
  test_los( test_banker2, test_chest2, true );

  // standing aboveground in buc's den, and below

  test_los( test_orclord, test_banker3, false );

  test_los( 579, 2106, 0, 1, 563, 2107, 0, 1, false );
  test_los( 862, 1691, 0, 1, 843, 1689, 0, 1, true );
  test_los( 1618, 3351, 3, 1, 1618, 3340, 4, 1, true );
}
} // namespace Testing
} // namespace Pol
