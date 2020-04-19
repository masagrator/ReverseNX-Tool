#include "ErrorMenu.h"
#include "../Utils.h"
#include <cstring>

Result ErrorMenu::error = 0;

void ErrorMenu::Update(StateMachine &stateMachine, u64 kDown)
{
    char str[50];
    sprintf(str, "Error: 0x%x", ErrorMenu::error);
    printf(CONSOLE_RED "\x1b[21;%dH%s", center(80, (int)strlen(str)), str);
    printf(CONSOLE_RED "\x1b[22;%dHPress `+` to exit!", center(80, 17));
}