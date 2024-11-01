/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <switch.h>
#include <unistd.h>

static int nxlink_sock = -1;

void userAppInit()
{
    romfsInit();
    socketInitializeDefault();
    nxlink_sock = nxlinkStdio();
    plInitialize(PlServiceType_User);
    setsysInitialize();
}

void userAppExit()
{
    if (nxlink_sock != -1)
        close(nxlink_sock);
    socketExit();
    romfsExit();
    plExit();
    setsysExit();
}
