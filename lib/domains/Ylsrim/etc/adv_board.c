/*    /domains/Ylsrim/etc/adv_board.c
 *    from the Nightmare V Object Library
 *    created by Rush@Nightmare
 */

#include <lib.h>

inherit "/lib/bboard";

static void create()
{
  ::create();
  SetKeyName("bulletin board");
  SetId( ({"board", "bulletin board"}) );
  SetAdjectives("stupid");
  SetShort("a bulletin board");
  SetLong("Adventurers passing through Ylsrim post their thoughts here.");
  set_board_id("ylsrim");
}


