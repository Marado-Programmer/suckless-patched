/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  		= 2;		/* border pixel of windows */
static const int startwithgaps[]	= { 1 };	/* 1 means gaps are used by default, this can be customized for each tag */
static const unsigned int gappx[]	= { 4 };	/* default gap between windows in pixels, this can be customized for each tag */
static unsigned int snap      		= 32;		/* snap pixel */
static int showbar            		= 1;		/* 0 means no bar */
static const int topbar			= 1;		/* 0 means bottom bar */
static const int showextrabar		= 1;		/* 0 means no extra bar */
static const int extrabarright		= 1;		/* 1 means extra bar text on right */
static const char statussep		= '@';		/* separator between status bars */
static const int startontag         	= 1;		/* 0 means no tag active on start */
static const int barheight      	= 12;		// 0 means that dwm will calculate bar height, >= 1 means dwm will barheight as bar height
static const int vertpad            	= 2;		// vertical padding of bar
static const int sidepad            	= 2;		// horizontal padding of bar
static char font[]            		= "Fira Code:style=Regular:size=9:antialias=true:autohint=true";
static char dmenufont[]       		= "Fira Code:style=Regular:size=9:antialias=true:autohint=true";
static const char *fonts[]		= { 
	"Fira Code:style=Regular:pixelsize=9:antialias=true:autohint=true",
	"Siji:style=Regular:pixelsize=9:antialias=true:autohint=true",
	"OpenMoji=Regular:pixelsize=9:antialias=true:autohint=true",
	"Noto Color Emoji=Regular:pixelsize=9:antialias=true:autohint=true",
	"Standard Symbols PS=Regular:pixelsize=9:antialias=true:autohint=true",
	"M+ 1c:style=Regular:pixelsize=9:antialias=true:autohint=true",
	"M+ 1p:style=Regular:pixelsize=9:antialias=true:autohint=true",
	"M+ 1m:style=Regular:pixelsize=9:antialias=true:autohint=true",
	"M+ 1mn:style=Regular:pixelsize=9:antialias=true:autohint=true",
};
static char normbgcolor[]           	= "#222222";
static char normbordercolor[]       	= "#444444";
static char normfgcolor[]           	= "#bbbbbb";
static char selfgcolor[]            	= "#eeeeee";
static char selbordercolor[]        	= "#005577";
static char selbgcolor[]            	= "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = 		{ "home", "code", "sys", "docs", "mail", "www",     "music", "test" };
static const char *tagsalt[] = 		{ "1", 	  "2", 	  "3",   "4",    "5",    "6", 	    "7",     "8" };
static const char *defaulttagapps[] = 	{ NULL,   "code",   "st",  NULL,   NULL,   "firefox", NULL,    NULL };
static const int momentaryalttags = 1; /* 1 means alttags will show only when key is held */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	// class		instance    title       tags mask	centerIfFirst	iscentered	isfloating   monitor */
	{ "St",			NULL,       NULL,       0,			1,				0,			0,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */
static const int attachdirection = 4;    // 0 default, 1 above, 2 aside, 3 below, 4 bottom, 5 top
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "|||",      col },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTMOD Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      combotag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }, \
	{ ALTMOD,			KEY,	  focusnthmon,	  {.i = TAG } }, \
	{ ALTMOD|ShiftMask,		KEY,	  tagnthmon,	  {.i = TAG } },


/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, topbar ? NULL : "-b", NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &font },
		{ "font",          	STRING,  &dmenufont },
		{ "color0",        	STRING,  &normbgcolor },
		{ "color8",    		STRING,  &normbordercolor },
		{ "color7",       	STRING,  &normfgcolor },
		{ "color6",         	STRING,  &selbgcolor },
		{ "color14",     	STRING,  &selbordercolor },
		{ "color15",         	STRING,  &selfgcolor },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_s,      spawndefault,   {0} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_b,      toggleextrabar, {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    comboview,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,		XK_t,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ControlMask,		XK_t,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY|ShiftMask,             XK_r,      setlayout,      {.v = &layouts[6]} },
	{ MODKEY|ShiftMask,		XK_m,      setlayout,      {.v = &layouts[7]} },
	{ MODKEY|ControlMask,		XK_m,      setlayout,      {.v = &layouts[8]} },
	{ MODKEY,                       XK_c,      setlayout,      {.v = &layouts[9]} },
	{ MODKEY,                       XK_space,  setlayout,  	   {0} },
	{ MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ControlMask,           XK_space,  togglealwaysontop, {0} },
	{ MODKEY,                       XK_0,      comboview,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      combotag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_Left,   viewtoleft,     {0} },
	{ MODKEY,                       XK_Right,  viewtoright,    {0} },
	{ MODKEY|ShiftMask,             XK_Left,   tagtoleft,      {0} },
	{ MODKEY|ShiftMask,             XK_Right,  tagtoright,     {0} },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_n,      togglealttag,   {0} },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -4 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +4 } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = GAP_RESET } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = GAP_TOGGLE} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY|ShiftMask, Button1,      movemouse,      {.i = 1} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,         MODKEY|ShiftMask, Button3,      resizemouse,    {.i = 1} },
	{ ClkTagBar,            0,              Button1,        comboview,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        combotag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

