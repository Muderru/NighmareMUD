/*    /lib/creator.c
 *    from the Nightmare IV LPC Library
 *    a very simple creator object
 *    created by Descartes of Borg 950321
 *    Version: @(#) creator.c 1.14@(#)
 *    Last Modified: 96/12/21
 */

#include <lib.h>
#include <daemons.h>
#include <vision.h>
#include <position.h>
#include <message_class.h>
#include "include/creator.h"

inherit LIB_INTERACTIVE;
inherit LIB_POSITION;
inherit LIB_TALK;

#ifdef __PACKAGE_DATABASE_DB__
inherit LIB_ISQL;
#endif /* __PACKAGE_DATABASE_DB__ */

private int CreatorAge, CreatorBirth;
private string Gender;
private static int LastCreatorAge;

/* *****************  /lib/creator.c driver applies  ***************** */

static void create() {
    interactive::create();
    CreatorAge = 0;
    LastCreatorAge = time();
    CreatorBirth = time();
}

int is_living() { return 1; }

int inventory_accessible() { return 1; }

int inventory_visible() { return 1; }

mixed direct_verb_rule(string verb) {
    return SOUL_D->CanTarget(this_player(), verb, this_object());
}

mixed direct_give_liv_obj() {
    if( this_player() == this_object() ) return "Are you confused?";
    return 1;
}

mixed direct_give_liv_obs() {
    return direct_give_liv_obj();
}

mixed indirect_give_obj_to_liv(object item) {
    if( !item ) return 0;
    if( this_player() == this_object() ) return "Are you confused?";
    if( environment(item) != this_player() ) return "You don't have that!";
    return CanCarry((int)item->GetMass());
}

mixed indirect_give_obs_to_liv(object *item) {
    return 1;
}

mixed direct_look_obj() { return 1; }

mixed direct_look_at_obj() { return 1; }

mixed direct_marry_liv_to_liv() {
    return 1;
}

mixed indirect_marry_liv_to_liv() {
    return 1;
}

static void net_dead() {
    interactive::net_dead();
    CreatorAge += time() - LastCreatorAge;
    LastCreatorAge = time();
}

void eventReconnect() {
    string tmp;

    interactive::eventReconnect();
    LastCreatorAge = time();
    if( file_exists(tmp = user_path(GetKeyName()) + "dead.edit") )
      message("system", "\nYour edit file was saved as: "+tmp, this_object());
}

varargs mixed eventShow(object who, string str, string on_id) {
    who->eventPrint(GetLong(str));
    environment(who)->eventPrint((string)this_player()->GetName() +
				 " looks at " + GetShort() + ".",
				 ({ who, this_object() }));
    if( this_player() != this_object() )
      eventPrint((string)this_player()->GetName() + " looks you over.");
    return 1;
}

/* *****************  /lib/creator.c events  ***************** */

void eventDescribeEnvironment(int verbose) {
    object env;

    if( !(env = environment()) ) {
        message("room_description", "No environment.", this_object());
        return;
    }
    message("system", file_name(env), this_object());
    interactive::eventDescribeEnvironment(verbose);
}

varargs int eventMoveLiving(mixed dest, string omsg, string imsg) {
    object *inv;
    object prev, env;

    if( prev = environment() ) {
        if( stringp(dest) ) {
            if(dest[0] != '/') {
                string *arr;

                arr = explode(file_name(prev), "/");
                dest = "/"+implode(arr[0..sizeof(arr)-2], "/")+"/"+dest;
            }
        }
        if( !eventMove(dest) ) {
	    eventPrint("You remain where you are.", MSG_SYSTEM);
            return 0;
        }
        inv = filter(all_inventory(prev), (: (!GetInvis($1) && living($1) &&
                                              ($1 != this_object())) :));
        if( !omsg || omsg == "" ) omsg = GetMessage("telout");
        else omsg = GetMessage("leave", omsg);
	inv->eventPrint(omsg, MSG_ENV);
    }
    else if( !eventMove(dest) ) {
	eventPrint("You remain where you are.", MSG_SYSTEM);
        return 0;
    }
    inv = filter(all_inventory(environment()),
                 (: (!GetInvis($1) && living($1) && ($1 != this_object())) :));
    if( (!imsg || imsg == "") && (!omsg || omsg == "") )
      imsg = GetMessage("telin");
    else if( !imsg || imsg == "" ) imsg = GetMessage("come", imsg);
    else imsg = replace_string(imsg, "$N", GetName());
    inv->eventPrint(imsg, MSG_ENV);
    eventDescribeEnvironment(GetBriefMode());
    return 1;
}

static int Destruct() {
    int x;

    if( !(x = interactive::Destruct()) ) return 0;
#ifdef __PACKAGE_DATABASE_DB__
    isql::Destruct();
#endif /* __PACKAGE_DATABASE_DB__ */
    return x;
}
	  
/* *****************  /lib/creator.c  local functions ***************** */

int Setup() {
    string tmp;
    int bugs, laston;

    laston = GetLoginTime();
    if( !interactive::Setup() ) return 0;
    AddChannel(({"cre", "intercre", "newbie", "avatar", "hm", "gossip",
		     "intergossip", "council" }));
    AddChannel((string array)CLASSES_D->GetClasses());
    if( archp() ) AddChannel( ({ "admin", "error" }) );
    AddSearchPath( ({ DIR_CREATOR_CMDS, DIR_SECURE_CREATOR_CMDS }) );
    if( file_size(tmp = user_path(GetKeyName()) + "cmds") == -2 )
      AddSearchPath( ({ tmp }) );
    if( archp() ) AddSearchPath( ({ DIR_ADMIN_CMDS, DIR_SECURE_ADMIN_CMDS }) );
    if( bugs = (int)BUGS_D->GetAssignedBugs(GetKeyName()) )
      message("system", "\n        >>>  You have " +
              consolidate(bugs, "an incomplete bug") +
              " assigned to you!!!!  <<<\n", this_object());
    NOTIFY_D->eventPrintNotices(this_object(), laston);
    return 1;
}

int eventForce(string cmd) {
    if( !((int)master()->valid_apply( ({ GetKeyName() }) )) ) return 0;
    else return interactive::eventForce(cmd);
}

/*  ***************  /lib/creator.c modal functions  *************** */

int CanCarry(int amount) { return 1; }

/* ************  /lib/creator.c data manipulation functions  ************ */

int GetCreatorAge() {
    int x;

    if( !interactive(this_object()) ) return CreatorAge;
    x = time() - LastCreatorAge;
    CreatorAge += x;
    LastCreatorAge = time();
    return CreatorAge;
}

string SetGender(string gender) {
    if( !((int)master()->valid_apply( ({ GetKeyName() }) )) ) return Gender;
    else return (Gender = gender);
}

string GetGender() { return Gender; }

varargs string GetLong(string str) {
    mapping counts;
    string item;

    str = GetShort() + "\n";
    str += interactive::GetLong() + "\n";
    foreach(item in map(all_inventory(),
                        (: (string)$1->GetAffectLong(this_object()) :)))
      if( item ) str += item + "\n";
    counts = ([]);
    foreach(item in map(
        filter(all_inventory(), (: !((int)$1->GetInvis(this_object())) :)),
        (: (string)$1->GetShort() :)))
      if( item ) counts[item]++;
    if( sizeof(counts) ) str += GetCapName() + " is carrying:\n";
    foreach(item in keys(counts))
      str += consolidate(counts[item], item) + "\n";
    return str;
}

int GetCreatorBirth() { return CreatorBirth; }

int GetEffectiveVision() { return VISION_CLEAR; }

string GetNativeLanguage() { return "English"; }

int GetLanguageLevel(string whocares) { return 100; }

string GetLanguageName(string lang) {
    if( convert_name(lang) != "english" ) return 0;
    else return "English";
}

string GetHealthShort() { return GetShort(); }

string GetName() { 
    if( !GetInvis() ) return ::GetName();
    else return "A shadow";
}

int GetRadiantLight() {
    return 0;
}
