WebKit2GTK+ WebExtension loader
======================================

This is some exploratory code towards finding a good solution which can be
shipped built-in to allow
[loading of Perl extensions in WebKit2GTK+](https://bugs.webkit.org/show_bug.cgi?id=140745)


How does it work?
-----------------

The [loader.c](Perl.c) file contains a small WebExtension
written in C, which is a thin shim that will:

1. Initialize an embedded Perl interpreter.
2. Import `Glib::Object::Introspection`, to ensure that the
   types used to implement WebExtensions are registered into Glib.
3. Import the `extension` Perl module ([extension.pm](extension.pm)).
4. Invoke the `extension.initialize()` function, passing as arguments a
   [WebKitWebExtension](http://webkitgtk.org/reference/webkit2gtk/stable/WebKitWebExtension.html)
   instance, and a
   [GVariant](https://developer.gnome.org/glib/stable/glib-GVariant.html)
   which contains the additional the value previously set with
   [webkit_web_context_set_web_extensions_initialization_user_data()](http://webkitgtk.org/reference/webkit2gtk/stable/WebKitWebContext.html#webkit-web-context-set-web-extensions-initialization-user-data).

The Perl extension can use all the functionality exposed via
GObject-Introspection, except for the `WebKit`, and `WebKit2` modules (web
extensions run in a process separate from the normal WebKit library, and using
them is unsupported — and will most likely crash your program). In particular,
the following modules included with WebKit2GTK+ can be used:

* [Web Extensions](http://webkitgtk.org/reference/webkit2gtk/stable/ch02.html)
* [DOM bindings](http://webkitgtk.org/reference/webkitdomgtk/stable/index.html)

Any other module exposed by GObject-Introspection can be used, as long as they
do not use the `WebKit`, or `WebKit2` modules.


Trying it out
-------------

You will need the following components installed, including their development
headers and libraries:

* WebKit2GTK+, version 2.4, or newer.
* Perl 5.10, or newer.
* A working Glib::Object::Introspection installation.
* GNU Make.
* `pkg-config`

