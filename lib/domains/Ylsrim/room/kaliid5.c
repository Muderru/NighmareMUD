/*    /domains/Ylsrim/room/kaliid5.c
 *    From the Nightmare V Object Library
 *    A simple room 
 *    Created by Descartes of Borg 960512
 */

#include <lib.h>

inherit LIB_ROOM;

static void create() {
    room::create();
    SetTown("Ylsrim");
    SetClimate("arid");
    SetNightLight(18);
    SetShort("Kaliid Road near the Fighters' Hall");
    SetLong("Kaliid Road stretches as far as you can see east and west "
	    "across Ylsrim.  Along the north side of this stretch of road "
	    "sits the local Fighters' Hall.  Immediately inside the entrance "
	    "you notice a recruitment area where novice seekers of fortune "
	    "decide to make their ways as fighters.");
    AddItem("hall", "Fighters passing through Ylsrim stop in to yap about "
	    "their adventures, and novices sign up to become fighters.",
	    ({ "fighter", "fighters" }));
    AddItem("road", "The main street for Ylsrim.", ({ "kaliid" }));
    SetObviousExits("e, w, enter hall");
    SetExits( ([ "east" : __DIR__ "kaliid4",
	       "west" : __DIR__ "kaliid6" ]));
    SetEnters( ([ "hall" : __DIR__ "fighter_hall" ]));
}

void init(){
    ::init();
}

