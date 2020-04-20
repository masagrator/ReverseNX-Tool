#include <switch.h>
#include "Utils.h"
#include "menu.h"

#include "States/StateMachine.h"
#include "States/States.h"

using namespace std;

int main(int argc, char **argv)
{
    StateMachine stateMachine;

    stateMachine.states.push_back(new ReverseNXMenu());

    if (R_FAILED(ErrorMenu::error))
        stateMachine.PushState("error");
    else
    {
        Utils::titles = Utils::getAllTitles();
        if (R_FAILED(ErrorMenu::error))
            stateMachine.PushState("error");
        else
            stateMachine.PushState("title");
    }

    //Initialize console. Using NULL as the second argument tells the console library to use the internal console structure as current one.
    //this is C++ we should use nullptr instead
    consoleInit(nullptr);

    while (appletMainLoop())
    {
        u64 kDown = Utils::GetControllerInputs();
        if (kDown & KEY_PLUS || (kDown & KEY_B && stateMachine.currentState->Name() == "title"))
            break;

        stateMachine.Update(kDown);
        consoleUpdate(nullptr);
    }

    consoleExit(nullptr);
    return 0;
}

extern "C"
{
    void userAppInit(void)
    {
        Result rc;
        rc = nsInitialize();

        ErrorMenu::error = rc;
    }

    void userAppExit(void)
    {
        nsExit();
    }
}
