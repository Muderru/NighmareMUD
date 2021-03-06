/*    /domains/Ylsrim/room/pub.c
 *    From the Nightmare V Object Library
 *    An example pub room
 *    Created by Descartes of Borg 960302
 */

#include <lib.h>
#include <message_class.h> // defines MSG_SYSTEM

inherit LIB_ROOM;

// This is a special SetRead() function that allows players to read
// the menu
int readMenu(object who, string str) {
    string array tmp = ({ sprintf("%:-20s %:-7s", "Drink", "Cost") });
    object ob = present("lars");
    
    if( !ob ) { // lars is dead!
	who->eventPrint("The menu is too bloodstained to read.");
	return 1;
    }
    foreach(string item in keys(ob->GetMenuItems())) {
	tmp += ({ sprintf("%:-20s %:-7d electrum", capitalize(item),
			  ob->GetCost(item)) });
    }
    // show the menu a page at a time
    who->eventPage(tmp, MSG_SYSTEM); // MSG_SYSTEM means ignore blocking
    return 1;
}
    
static void create() {
    room::create();
    SetTown("Ylsrim");
    SetClimate("indoors");
    SetAmbientLight(25);
    SetShort("Lars' Pub");
    SetLong("Lars' Pub is one of the most famous gathering places in all "
	    "Ylsrim.  As you can see about you, however, it is not among the "
	    "more homely ones.  Tables and stools are littered all about "
	    "with a hodge-podge of writing in all different languages "
	    "covering the wall.  A menu of drinks is about the only "
	    "readable thing on the wall.  If you read Eltherian.");
    SetListen("Rowdy party sounds make it hard to hear anything else.");
    SetSmell("The place smells like it is soaked in ale.");
    AddItem(({ "tables", "stools" }), "The tables and stools begin "
            "to migrate as the day goes on and customerizes socialize.  "
	    "Of course, nothing moves them better than a good brawl.");
    AddItem(({ "menu", "drinks", "menu of drinks" }), "The menu is "
	    "written in Eltherian.  Can you read it?", ({ "drink" }));
    AddItem(({ "writing", "walls" }), "Scribble in all different "
		"languages, and in many pens and many hands covers much of "
		"each wall.");
    SetObviousExits("e");
    SetExits(([ "east" : __DIR__ "s_bazaar" ]));
    // Bring in the Bar Keep
    SetInventory(([ DIR_STANDARD_DOMAIN "/npc/lars" : 1 ]));
    // make the writing and the menu readable
    SetRead("writing", (: readMenu :));
    SetRead("menu", (: readMenu :));
}
