/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#define _WIN32_IE 0x301

#include <winsock2.h>
#include <windows.h>
#include <gl\glew.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <process.h>
#include <fcntl.h>
#include <io.h>

#include <jpeglib.h>
#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>
#include <freetype\ftoutln.h>
#include <freetype\fttrigon.h>

#include <vfw.h>

#include "gamecore\hud\hud.h"
#include "gui\gui.h"
#include "gui\xmmenu\xmmenu.h"

#include "window\textures.h"
#include "window\tex3D.h"
#include "window\window.h"
#include "window\shaders.h"
#include "window\timer.h"
#include "window\font.h"
#include "window\resource.h"
#include "window\updatergui.h"

#include "math\vector.h"
#include "math\mat4.h"

#include "gui\cryptfuncs.h"
#include "gui\networking.h"

#include "gamecore\xmanimation\xmanimation.h"
#include "gamecore\xmanimation\xmvertexanimation.h"
#include "gamecore\characters\characters.h"
#include "gamecore\cartridges\cartridges.h"
#include "gamecore\collisions\collision.h"
#include "gamecore\xmlevel\postprocess.h"
#include "gamecore\xmdmodel\xmdmodel.h"
#include "gamecore\xmlevel\xmlevel.h"
#include "gamecore\xmlevel\shaders.h"
#include "gamecore\frustum\frustum.h"
#include "gamecore\weapons\weapons.h"
#include "gamecore\bullets\bullet.h"
#include "gamecore\skybox\skybox.h"
#include "gamecore\camera\camera.h"


#include "network\client.h"
#include "network\fileUpdater.h"

using namespace std;

