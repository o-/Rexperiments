/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 2000-12     the R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Defn.h> /* for checkArity */
#include <Internal.h>

#ifdef Win32
#include "Startup.h"
extern UImode CharacterMode;
#include "getline/getline.h"     /* for gl_load/savehistory */
#include "getline/wc_history.h"  /* for wgl_load/savehistory */
SEXP savehistory(SEXP call, SEXP op, SEXP args, SEXP env)
{
    SEXP sfile;

    args = CDR(args);
    sfile = CAR(args);
    if (!isString(sfile) || LENGTH(sfile) < 1)
	errorcall(call, _("invalid '%s' argument"), "file");
    if (CharacterMode == RGui) {
	R_setupHistory(); /* re-read the history size */
	wgl_savehistoryW(filenameToWchar(STRING_ELT(sfile, 0), 0), 
			 R_HistorySize);
    } else if (R_Interactive && CharacterMode == RTerm) {
	R_setupHistory(); /* re-read the history size */
	gl_savehistory(translateChar(STRING_ELT(sfile, 0)), R_HistorySize);
    } else
	errorcall(call, _("'savehistory' can only be used in Rgui and Rterm"));
    return R_NilValue;
}

SEXP loadhistory(SEXP call, SEXP op, SEXP args, SEXP env)
{
    SEXP sfile;

    args = CDR(args);
    sfile = CAR(args);
    if (!isString(sfile) || LENGTH(sfile) < 1)
	errorcall(call, _("invalid '%s' argument"), "file");
    if (CharacterMode == RGui)
	wgl_loadhistoryW(filenameToWchar(STRING_ELT(sfile, 0), 0));
    else if (R_Interactive && CharacterMode == RTerm)
	gl_loadhistory(translateChar(STRING_ELT(sfile, 0)));
    else
	errorcall(call, _("'loadhistory' can only be used in Rgui and Rterm"));
    return R_NilValue;
}

SEXP addhistory(SEXP call, SEXP op, SEXP args, SEXP env)
{
    SEXP stamp;

    args = CDR(args);
    stamp = CAR(args);
    if (!isString(stamp))
	errorcall(call, _("invalid timestamp"));
    if (CharacterMode == RGui) {   
	for (int i = 0; i < LENGTH(stamp); i++) 
	    wgl_histadd(wtransChar(STRING_ELT(stamp, i)));
    } else if (R_Interactive && CharacterMode == RTerm) {
    	for (int i = 0; i < LENGTH(stamp); i++)
	    gl_histadd(translateChar(STRING_ELT(stamp, i)));
    }
    return R_NilValue;
}

SEXP Win_dataentry(SEXP args);
SEXP Win_dataviewer(SEXP args);

SEXP dataentry(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    return Win_dataentry(CDR(args));
}

SEXP dataviewer(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    return Win_dataviewer(CDR(args));
}

SEXP Win_selectlist(SEXP args);

SEXP selectlist(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    return Win_selectlist(CDR(args));
}

#else

#define R_INTERFACE_PTRS 1
#include <Rinterface.h>

SEXP loadhistory(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    ptr_R_loadhistory(call, op, CDR(args), rho);
    return R_NilValue;
}

SEXP savehistory(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    ptr_R_savehistory(call, op, CDR(args), rho);
    return R_NilValue;
}

SEXP addhistory(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    if(ptr_R_addhistory) ptr_R_addhistory(call, op, CDR(args), rho);
    return R_NilValue;
}

extern SEXP X11_do_dataentry(SEXP call, SEXP op, SEXP args, SEXP rho);
extern SEXP X11_do_dataviewer(SEXP call, SEXP op, SEXP args, SEXP rho);

SEXP dataentry(SEXP call, SEXP op, SEXP args, SEXP env)
{
    args = CDR(args);
    if(ptr_do_dataentry) return ptr_do_dataentry(call, op, args, env);
    else return X11_do_dataentry(call, op, args, env);
}

SEXP dataviewer(SEXP call, SEXP op, SEXP args, SEXP env)
{
    args = CDR(args);
    if(ptr_do_dataviewer) return ptr_do_dataviewer(call, op, args, env);
    else return X11_do_dataviewer(call, op, args, env);
}

SEXP selectlist(SEXP call, SEXP op, SEXP args, SEXP env)
{
    if(ptr_do_selectlist) return ptr_do_selectlist(call, op, args, env);
    return R_NilValue;
}
#endif

SEXP edit(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    return do_edit(call, op, CDR(args), rho);
}

SEXP flushconsole(void)
{
    R_FlushConsole();
    return R_NilValue;
}


// formerly in src/main/platform.c
SEXP fileedit(SEXP call, SEXP op, SEXP args, SEXP rho)
{
    SEXP fn, ti, ed;
    const char **f, **title, *editor;
    int i, n;

    args = CDR(args);
    fn = CAR(args); args = CDR(args);
    ti = CAR(args); args = CDR(args);
    ed = CAR(args);

    n = length(fn);
    if (!isString(ed) || length(ed) != 1)
	error(_("invalid '%s' specification"), "editor");
    if (n > 0) {
	if (!isString(fn))
	    error(_("invalid '%s' specification"), "filename");
	f = (const char**) R_alloc(n, sizeof(char*));
	title = (const char**) R_alloc(n, sizeof(char*));
	/* FIXME convert to UTF-8 on Windows */
	for (i = 0; i < n; i++) {
	    SEXP el = STRING_ELT(fn, 0);
	    if (!isNull(el))
#ifdef Win32
		f[i] = acopy_string(reEnc(CHAR(el), getCharCE(el), CE_UTF8, 1));
#else
		f[i] = acopy_string(translateChar(el));
#endif
	    else
		f[i] = "";
	    if (!isNull(STRING_ELT(ti, i)))
		title[i] = acopy_string(translateChar(STRING_ELT(ti, i)));
	    else
		title[i] = "";
	}
    }
    else {  /* open a new file for editing */
	n = 1;
	f = (const char**) R_alloc(1, sizeof(char*));
	f[0] = "";
	title = (const char**) R_alloc(1, sizeof(char*));
	title[0] = "";
    }
    SEXP ed0 = STRING_ELT(ed, 0);
#ifdef Win32
    editor = acopy_string(reEnc(CHAR(ed0), getCharCE(ed0), CE_UTF8, 1));
#else
    editor = acopy_string(translateChar(ed0));
#endif
    R_EditFiles(n, f, title, editor);
    return R_NilValue;
}