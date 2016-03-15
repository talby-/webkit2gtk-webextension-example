/*
 * loader.c
 * Copyright (C) 2016 Robert Stone <talby@trap.mtview.ca.us>
 * Adapted from:
 *
 * pythonloader.c
 * Copyright (C) 2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdlib.h>

#include <webkit2/webkit-web-extension.h>
#define PERL_NO_GET_CONTEXT
#include "gperl.h"

extern void boot_DynaLoader (pTHX_ CV *cv);

static void xs_init(pTHX) {
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
}

#define NAME        "plWebKitWebExtension"
#define PERL_INIT   "use Glib::Object::Introspection;" \
                    "Glib::Object::Introspection->setup(" \
                    "    basename => 'WebKit2WebExtension'," \
                    "    version  => '4.0', "\
                    "    package  => 'WebKit2WebExtension'," \
                    ");"
#define ENV_KEY     "PLWKEXT"

G_MODULE_EXPORT void
webkit_web_extension_initialize_with_user_data (WebKitWebExtension *extension,
                                                const GVariant     *user_data)
{
    int argc = 3;
    char *argv[] = { "", "-e", PERL_INIT, NULL };
    char *plrun = getenv(ENV_KEY);
    PerlInterpreter *my_perl;

    if(plrun == NULL) {
        g_printerr("[%s] environment \"%s\" missing\n", NAME, ENV_KEY);
        return;
    }

    my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    perl_parse(my_perl, xs_init, argc, argv, NULL);
    perl_run(my_perl);

    {
        dSP;
        SV *load;

        ENTER;
        SAVETMPS;

        eval_sv(sv_2mortal(newSVpvf(
            "sub { require q%c%s%c }", 0, plrun, 0)), G_SCALAR);
        SPAGAIN;
        load = POPs;
        PUTBACK;

        PUSHMARK(SP);
        mXPUSHs(gperl_new_object(G_OBJECT(extension), FALSE));
        mXPUSHs(gperl_new_boxed((gpointer)user_data, G_TYPE_VARIANT, 0));
        PUTBACK;
        call_sv(load, G_VOID);
        if(SvTRUE(ERRSV))
            g_printerr("[%s] %s", NAME, SvPV_nolen(ERRSV));

        FREETMPS;
        LEAVE;
    }
}
