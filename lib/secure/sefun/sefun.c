/*    /secure/obj/sefun.c
 *    from Nightmare IV
 *    the mud sefun object
 *    created by Descartes of Borg 940213
 */

#pragma save_binary

#include <lib.h>
#include <daemons.h>
#include <commands.h>
#include <objects.h>
#include <localtime.h>
#include "sefun.h"

#include "/secure/sefun/absolute_value.c"
#include "/secure/sefun/base_name.c"
#include "/secure/sefun/communications.c"
#include "/secure/sefun/convert_name.c"
#include "/secure/sefun/copy.c"
#include "/secure/sefun/distinct_array.c"
#include "/secure/sefun/domains.c"
#include "/secure/sefun/economy.c"
#include "/secure/sefun/english.c"
#include "/secure/sefun/events.c"
#include "/secure/sefun/expand_keys.c"
#include "/secure/sefun/files.c"
#include "/secure/sefun/format_page.c"
#include "/secure/sefun/get_object.c"
#include "/secure/sefun/identify.c"
#include "/secure/sefun/interface.c"
#include "/secure/sefun/light.c"
#include "/secure/sefun/load_object.c"
#include "/secure/sefun/log_file.c"
#include "/secure/sefun/messaging.c"
#include "/secure/sefun/morality.c"
#include "/secure/sefun/mud_info.c"
#include "/secure/sefun/ordinal.c"
#include "/secure/sefun/parse_objects.c"
#include "/secure/sefun/path_file.c"
#include "/secure/sefun/percent.c"
#include "/secure/sefun/persist.c"
#include "/secure/sefun/pointers.c"
#include "/secure/sefun/query_time_of_day.c"
#include "/secure/sefun/absolute_path.c"
#include "/secure/sefun/security.c"
#include "/secure/sefun/strings.c"
#include "/secure/sefun/this_agent.c"
#include "/secure/sefun/time.c"
#include "/secure/sefun/to_object.c"
#include "/secure/sefun/translate.c"
#include "/secure/sefun/user_exists.c"
#include "/secure/sefun/user_path.c"
#include "/secure/sefun/visible.c"

int tail(string file) {
    string str;
    int diff;

    diff = file_size(file);
    if (diff < 0) return 0;

    diff -= 1024;
    if (diff < 0) diff = 0;
    str = read_bytes(file, diff, 1024);
    if (!str) return 0;
    if (diff) str = str[strsrch(str, "\n")+1..];

    write(str);

    return 1;
}

string timestamp(){
    string rawtz;
    int *t, gmtoff, offset;

    gmtoff = localtime(time())[LT_GMTOFF];

    t = localtime(time());
    return sprintf("%04d.%02d.%02d-%02d.%02d", t[LT_YEAR], (t[LT_MON])+1, t[LT_MDAY], t[LT_HOUR], t[LT_MIN]);
}

varargs string get_stack( int x) {
    int i, s;
    string list = "";
    string *stack0 = call_stack(0);
    string *stack1 = call_stack(1);
    string *stack2 = call_stack(2);
    for(i = 0, s = sizeof(stack1); i < s; i++){
        list +="\n"+i+":"+identify(stack2[i])+"."+identify(stack1[i])+"."+identify(stack2[i]);
    }

    if(x){
        list += "\n"+ identify(previous_object(-1));
    }

    return list;
}

void tc(string mess){
    object crat = find_player("god");
    string sauce = base_name((previous_object() || this_object()));
    if(crat) tell_object(crat, sauce +": "+mess+"\n");
    //if(crat) crat->receive_private_msg(sauce+": "+mess, PRIVATE_MSG);
    debug_message(sauce +": "+mess);
    flush_messages();
}

string dump_socket_status() {
    string ret;
    string *finalsocks, *sock_array = ({});
    int i, quant = sizeof(socket_status());
    for(i = 0; i < quant; i++){
        sock_array += ({ socket_status(i) });
    }
    finalsocks = sock_array;

    ret = @END
Fd    State      Mode       Local Address          Remote Address
--  ---------  --------  ---------------------  ---------------------
END;

    foreach (mixed *item in finalsocks) {
        int memb = member_array(item, finalsocks);
        ret += sprintf("%2d  %|9s  %|8s  %-21s  %-21s\n", memb, item[1], item[2], item[3], item[4]);
    }

    return ret;
}

int destruct(object ob) {
    string *privs;
    string tmp;

    if(previous_object(0) == ob) return efun::destruct(ob);
    if(!(tmp = query_privs(previous_object(0)))) return 0;
    if(member_array(PRIV_SECURE, explode(tmp, ":")) != -1)
      return efun::destruct(ob);
    privs = ({ file_privs(file_name(ob)) });
    if((int)master()->valid_apply(({ "ASSIST" }) + privs))
      return efun::destruct(ob);
    else return 0;
}

varargs void shutdown(int code) {
    if(!((int)master()->valid_apply(({})))) return;
    if(this_player())
      log_file("shutdowns", (string)this_player()->GetCapName()+
        " shutdown "+mud_name()+" at "+ctime(time())+"\n");
    else log_file("shutdowns", "Game shutdown by "+
      file_name(previous_object(0))+" at "+ctime(time())+"\n");
    efun::shutdown(code);
}

varargs object snoop(object who, object target) {
    if(!target) return efun::snoop(who);
    if(!creatorp(who)) return 0;
    if(!((int)master()->valid_apply(({ "ASSIST" })))) {
        if(!((int)target->query_snoopable())) return 0;
        else return efun::snoop(who, target);
    }
    else if(member_group(target, PRIV_SECURE)) {
        message("system", (string)who->GetCapName()+" is now snooping "
          "you.", target);
        return efun::snoop(who, target);
    }
    else return efun::snoop(who, target);
}

object query_snoop(object ob) {
    if(!userp(previous_object(0))) return 0;
    return efun::query_snoop(ob);
}

object query_snooping(object ob) {
    if(!((int)master()->valid_apply(({})))) return 0;
    else return efun::query_snooping(ob);
}

int exec(object target, object src) {
    string tmp;

    tmp = base_name(previous_object());
    if(tmp != LIB_CONNECT && tmp != CMD_ENCRE && tmp != CMD_DECRE) return 0;
    return efun::exec(target, src);
}

void write(string str) {
    if(this_player()) message("my_action", str, this_player());
    else efun::write(str);
}

object *livings() {
    return efun::livings() - (efun::livings() - objects());
}

void set_privs(object ob, string str) { return; }

void set_eval_limit(int x) {
    if(previous_object() != master()) return;
    efun::set_eval_limit(x);
}

void notify_fail(string str) {
    if( !this_player() ) return;
    if( str[<1..] == "\n" ) str = str[0..<2];
    this_player()->SetCommandFail(str);
}

/* want to handle colours, but do it efficiently as possible */
string capitalize(string str) {
    string *words, *tmp;
    int i;
    
    /* error condition, let it look like an efun */
    if( !str || str == "" ) return efun::capitalize(str);
    /* most strings are not colour strings */
    if( strlen(str) < 2 || str[0..1] != "%^" ) return efun::capitalize(str);
    /* god help us */
    words = explode(str, " ");
    /* ok, this is strange, but remember, colours are all caps :) */
    tmp = explode(words[0], "%^");
    for(i=0; i<sizeof(tmp); i++) tmp[i] = efun::capitalize(tmp[i]);
    words[0] = "%^" + implode(tmp, "%^") + "%^";
    return implode(words, " ");
}

