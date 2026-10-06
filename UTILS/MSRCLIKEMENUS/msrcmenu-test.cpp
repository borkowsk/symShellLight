/// @file
/// @brief Prototypowanie MS RC like MENUS
/// @date 2026-10-06 (created)

class RUNREG
{

};

RUNREG MENU DISCARDABLE
    BEGIN
        MENUITEM "Start/Stop",                      SSH_STARTSTOP
        MENUITEM "One step",                        SSH_ONESTEP
        POPUP "&Job"
            BEGIN
            MENUITEM "&End now",                    IDM_EXIT
            MENUITEM "&Don't continue",             IDM_NCONTINUE
            END
        POPUP "&Help"
            BEGIN
            MENUITEM "&About",                      IDM_ABOUT
            END
    END
    ;

int main()
{
    return 0;
}

