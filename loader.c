/*
 * loader.c
 * Copyright (C) 2016-2018 Robert Stone <talby@trap.mtview.ca.us>
 * Adapted from:
 *
 * pythonloader.c
 * Copyright (C) 2015-2016 Adrian Perez <aperez@igalia.com>
 * Copyright (C) 2016 Nathan Hoad <nathan@getoffmalawn.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <webkit2/webkit-web-extension.h>
#include <stdarg.h>
#include <stdlib.h>
#define PERL_NO_GET_CONTEXT
#include "gperl.h"

extern void boot_DynaLoader (pTHX_ CV *cv);

static void xs_init(pTHX) {
    newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
}


static const char *get_module(GVariant *user_data) {
    /* The scheme here is that if you pass a dictionary to
     * webkit_web_context_set_web_extensions_initialization_user_data()
     * we'll look for a "pl_ext" key to find the perl module path. */

    if(user_data) {
        if(g_variant_type_is_subtype_of(
                g_variant_get_type(user_data),
                G_VARIANT_TYPE("a{s*}"))) {
            GVariant *val = g_variant_lookup_value(
                    user_data, "pl_ext", G_VARIANT_TYPE("s"));
            if(val)
                return g_variant_get_string(val, NULL);
        }
    }
    return NULL;

}

G_MODULE_EXPORT void
webkit_web_extension_initialize_with_user_data (WebKitWebExtension *extension,
                                                GVariant           *user_data)
{
    int argc = 3;
    char *argv[] = { "", "-e",
        "use Glib::Object::Introspection;"
        "Glib::Object::Introspection->setup("
        "    basename => 'WebKit2WebExtension',"
        "    version  => '4.0',"
        "    package  => 'WebKit2WebExtension',"
        ");", NULL };
    PerlInterpreter *my_perl;
    extern char **environ;
    const char *module = get_module(user_data);
    
    g_message("[%s:%d] module: %s", __FILE__, __LINE__, module);
    if(!module) {
        g_message("[%s:%d] user_data: %s", __FILE__, __LINE__,
            user_data ? g_variant_print(user_data, 1) : NULL);
        return;
    }

    my_perl = perl_alloc();
    perl_construct(my_perl);
    PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
    perl_parse(my_perl, xs_init, argc, argv, environ);
    perl_run(my_perl);

    {
        dSP;
        SV *load;

        ENTER;
        SAVETMPS;

        eval_sv(sv_2mortal(newSVpvf("sub { require q%c%s%c }",
            0, module, 0)), G_SCALAR);

        SPAGAIN;
        load = POPs;
        PUTBACK;

        PUSHMARK(SP);
        mXPUSHs(gperl_new_object(G_OBJECT(extension), FALSE));
        mXPUSHs(newSVGVariant(user_data));
        PUTBACK;

        call_sv(load, G_VOID | G_EVAL);

        if(SvTRUE(ERRSV)) {
            //GError *err = gperl_error_from_sv(ERRSV);
            /* TODO: where do I send this GError now? */
            g_printerr("[%s] %s", module, SvPV_nolen(ERRSV));
        }

        FREETMPS;
        LEAVE;
    }
}
