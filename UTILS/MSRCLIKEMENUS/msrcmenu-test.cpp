/// @file
/// @brief Prototypowanie MS RC like MENUS
/// @date 2026-10-26 (last modifications)
#include <vector>
#include <string>
#include <iostream>

namespace MSWinMenu
{
    class MenuItem;

    void intend(std::ostream& o,int level=0);

    struct MenuItemHandle
    {
       const MenuItem* item;
       bool  allocated:1;

       explicit MenuItemHandle(const MenuItem& it);
    };

    class Menu
    {
        std::string title;
        std::vector<MenuItemHandle> items;
    public:
        Menu(const char* title, std::initializer_list<MenuItemHandle> initialItems);
        void display(std::ostream& o,int level=0) const;
    };

    class MenuItem
    {
        friend class MenuItemHandle;
        std::string name;
        unsigned int val;
    public:
        virtual ~MenuItem();
        MenuItem():name{"#H.It"},val(-1){}
        MenuItem(const char* str, unsigned int i):name{str},val{i}
        {}
        virtual void print(std::ostream& o,int level) const;
        virtual bool isPopup() const { return false; }
    };

    class Popup:public MenuItem, private Menu
    {
    public:
        Popup(const char* title, std::initializer_list<MenuItemHandle> initialItems):MenuItem(title,0),Menu(title,initialItems)
        {}
        void print(std::ostream& o,int level) const override;
        bool isPopup() const override { return true; }
    };

}/* _EndOfNamespace MSWinMenu */

#define RUNREG /*constexpr - niestety nie może być */
#define MENU MSWinMenu::Menu MainMenu
#define DISCARDABLE ={"MAIN MENU"
#define BEGIN ,{ MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem{
#define END }}}
#define MENUITEM }}, MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem {
#define POPUP }}, MSWinMenu::MenuItemHandle{MSWinMenu::Popup {
#define END_OF_MENU  };/* _EndOfMenu */

#define SSH_STARTSTOP 'p'
#define SSH_ONESTEP   's'
#define IDM_EXIT      'e'
#define IDM_NCONTINUE 's'
#define IDM_ABOUT     'a'

// /*NOTHING*/ MSWinMenu::Menu HelpMenu1 = {"EMPTY MENU"
//                                        ,{
//                                          }
//};
//
// /*NOTHING*/ MSWinMenu::Menu HelpMenu2= {"FIRST MENU"
//                                        , { MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem{
//                                        }}, MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem { "Start/Stop", SSH_STARTSTOP
//                                        }}}
//};
//
// /*NOTHING*/ MSWinMenu::Menu HelpMenu3= {"SECOND MENU"
//        , { MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem{
//        }}, MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem{"Start/Stop", SSH_STARTSTOP
//        }}, MSWinMenu::MenuItemHandle{MSWinMenu::MenuItem {  "One step",  SSH_ONESTEP
//        }}}
//};
//
//RUNREG MSWinMenu::Menu HelpMenu4= {"THIRD MENU"
//                                   BEGIN
//                                   MENUITEM "Start/Stop", SSH_STARTSTOP
//                                   MENUITEM "One step", SSH_ONESTEP
//                                   END
//END_OF_MENU

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
END_OF_MENU

int main()
{
    //HelpMenu4.display(std::cout,0);
    std::cout<<std::endl;
    MainMenu.display(std::cerr,0);
    return 0;
}

// IMPLEMENTATIONS
namespace MSWinMenu
{
    MenuItemHandle::MenuItemHandle(const MenuItem &it) :item(&it),allocated(false)
    {
      std::cerr<<__FUNCTION__<<' '<<item<<' ';
      if(item!= nullptr) std::cerr<<item->name<<(item->isPopup()?"(popup)":"(item)")<<std::endl;
    }

    MenuItem::~MenuItem()
    {
        std::cerr<<__FUNCTION__<<' '<<this<<' '<<this->name<<'\n';
    }

    Menu::Menu(const char *title, std::initializer_list<MenuItemHandle> initialItems)
            : title(title),
              items{initialItems} //TODO: TO JEST BŁĄD! OBIEKTY UŻYTE W INICJALIZACJI HIERARCHII SĄ TYMCZASOWE, NAWET CONST...
    {
        display(std::cerr,1);
    }

    void intend(std::ostream& o,int level)
    {
        for(int i=0;i<level;i++)
            o<<"____";
    }

    void MenuItem::print(std::ostream &o, int level) const
    {
        intend(o,level);
        o<<this->name<<"\t\t"<<this->val;
    }

    void Menu::display(std::ostream &o, int level) const
    {
        intend(o,level);
        o<<"<<"<<title<<">>"<<std::endl;
        for(int i=0;i<items.size();i++)
        {
            o<<i<<"> "<<items[i].item<<" ";
            items[i].item->print(o, level);
            o << std::endl;
        }
    }

    void Popup::print(std::ostream &o, int level) const
    {
        MenuItem::print(o,level);
        std::cout<<std::endl;
        Menu::display(o,level+1);
    }
}
