# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2015 Adrian Perez <aperez@igalia.com>
#
# Distributed under terms of the MIT license.
use strict;
use warnings;

sub on_document_loaded {
    my($webpage) = @_;
    # As en example, use the DOM to print the document title
    print("document-loaded: uri = ", $webpage->get_uri(), "\n");
    my $document = $webpage->get_dom_document();
    print("document-loaded: title = ", $document->get_title());
}

sub on_page_created {
    my($extension, $webpage) = @_;
    print("page-created: extension = ", $extension, "\n");
    print("page-created: page = ", $webpage, "\n");
    print("page-created: uri = ", $webpage->get_uri(), "\n");
    $webpage->signal_connect("document-loaded", \&on_document_loaded);
}

sub initialize {
    my($extension, $arguments) = @_;
    print("initialize: extension = ", $extension, "\n");
    print("initialize: arguments = ", $arguments, "\n");
    $extension->signal_connect("page-created", \&on_page_created);
}

initialize(@_);
