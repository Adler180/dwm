/* See LICENSE file for copyright and license details. */
#include"movestack.c"
/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel size of windows */
static const unsigned int gappx     = 5;        /* gaps size between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=16", "fontawesome:size=16" };
static const char dmenufont[]       = "monospace:size=16";
//background color
static const char col_gray1[]       = "#222222";
//inactive window border color
static const char col_gray2[]       = "#444444";
//font color
static const char col_gray3[]       = "#bbbbbb";
//current tag and current window font color
static const char col_gray4[]       = "#eeeeee";
//Top bar second color (blue) and active window border color
static const char col_cyan[]        = "#9900cc";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
//tag names (upper left)
static const char *tags[] = { "", "", "", "", "",  "", "", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            0,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "zoom",     NULL,       NULL,       0,       	    1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
//static const char *filemanager[] = { "
//launches htop
static const char *monitor[] = { "/usr/bin/htop", NULL };
//sets st as the default terminal
//static const char *termcmd[]  = { "st", NULL };
//sets urxvt as the default terminal
static const char *termcmd[]  = { "kitty", NULL };
static const char *lockcmd[]  = { "slock", NULL };
static const char *suspendcmd[] = { "slock", "systemctl", "suspend", "-i", NULL };
//volume controls
//static const char *upvol[]   = { "amixer", "-q", "set", "Master", "5%+", "unmute", NULL };
//static const char *downvol[] = { "amixer", "-q", "set", "Master", "5%-", "unmute", NULL };
//static const char *mutevol[] = { "amixer", "-q", "set", "Master", "toggle", NULL };
//Volume Controls Wireplumber
static const char *upvol[] = {"wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%+", NULL};
static const char *downvol[] = {"wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%-", NULL};
static const char *mutevol[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };

//Control cmus Music player
//You'll first need to add the selectet Playlist to the remote player with -cmus-remote PATH_TO_PLAYLIST-
static const char *pausecmus[] = {"cmus-remote", "-u", NULL};
static const char *nextcmus[] = {"cmus-remote", "-n", NULL};
static const char *prevcmus[] = {"cmus-remote", "-r", NULL};

//Take screenshot with shotgun into ~/Pictures/Screenshots
//static const char *sshot[] = {"shotgun", "~/Pictures/Screenshots$(date", "'+%Y%m%H%M')", NULL};
static const char *sshot[] = {"shotgun", "~/Pictures/Screenshots/", NULL};
//static const char *sshot[] = {"shotgun", "/tmp/Screenshots/$(date '+%Y')", NULL};

#include "shiftview.c"
static const int mainmon = 0; /* xsetroot will only change the bar on this monitor */
static char *endx[] = { "/bin/sh", "-c", "endx", "externalpipe", NULL };
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_space,      spawn,          {.v = dmenucmd } },
	{ MODKEY,	                    XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY, 			XK_u, 		spawn, 		{.v = lockcmd } },
	{ MODKEY|ShiftMask,		XK_u, 		spawn, 		{.v = suspendcmd } },
	{ MODKEY,                       XK_t,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_m,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_z,	   zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,	                XK_BackSpace,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },
	//{ MODKEY|ShiftMask,             XK_space,  setlayout,      {0} },
 	{ MODKEY|ControlMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ControlMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_j,      focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ControlMask|ShiftMask,             XK_j,  tagmon,         {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask,             XK_k, tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,		XK_l,      shiftview,  	   { .i = +1 } },
	{ MODKEY|ShiftMask, 		XK_h,      shiftview,      { .i = -1 } },
    { MODKEY,                       XK_F12,     spawn,          {.v = upvol   } },
    //{ MODKEY,                       KC_VOLU,     spawn,          {.v = upvol   } },
    { MODKEY,                       XK_F11,     spawn,          {.v = downvol } },
    { MODKEY,                       XK_F10,     spawn,          {.v = mutevol } },
    { MODKEY,                       XK_F8,     spawn,          {.v = pausecmus   } },
    { MODKEY,                       XK_F7,     spawn,          {.v = prevcmus   } },
    { MODKEY,                       XK_F9,     spawn,          {.v = nextcmus   } },
    { MODKEY,                       XK_Print,     spawn,          {.v = sshot } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_BackSpace,      quit,           {0} },
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
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

