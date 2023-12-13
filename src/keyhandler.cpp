#include <gtkmm.h>

#include "RunShell.hpp"
#include "bridge.hpp"
#include "keyhandler.hpp"
#include "fsutil.hpp"
#include "Toolbar.hpp"

bool HandleKeyPress(
    GdkEventKey* event,
	Gtk::Window* Parent,
	Browser* CurrentBrowser,
	Browser* Browser_1,
	Browser* Browser_2,
	Toolbar* Toolbar,
	Gtk::Entry* Pathentry,
    RunShell* Runentry) {
  
  if (event->state & GDK_CONTROL_MASK) {
	switch (event->keyval) {
	case ADD_FILE_KEY:
	  bridge::wAddFile(Parent, CurrentBrowser);
	  break;
	case ADD_DIR_KEY:
	  bridge::wAddDir(Parent, CurrentBrowser);
	  break;
	case RENAME_KEY:
	  bridge::wRenameObjects(Parent, CurrentBrowser);
	  break;
	case DELETE_KEY:
	  bridge::wDeleteObjects(Parent, Toolbar, CurrentBrowser);
	  break;
	case RECOVER_KEY: {
	  std::filesystem::path trash_path(getenv("HOME"));
      trash_path.append(TRASH_PATH_REL);
	  
	  if (CurrentBrowser->CurrentPath != trash_path)
		bridge::wNavigate(Parent, CurrentBrowser, Pathentry, trash_path);
	  else
		bridge::wRestoreObject(Parent, Toolbar, CurrentBrowser);
	  break;
	}
	case DIRECT_COPY_KEY:
	  bridge::wDirectCopyObjects(Parent, Toolbar, CurrentBrowser, false);
	  break;
	case DIRECT_MOVE_KEY:
	  bridge::wDirectCopyObjects(Parent, Toolbar, CurrentBrowser, true);
	  break;
	case MODIFY_KEY:
	  bridge::wModifyObjects(Parent, CurrentBrowser);
	  break;
	case COPY_KEY:
	  bridge::wHandleGnomeCopy(Parent, CurrentBrowser, false);
	  break;
	case CUT_KEY:
	  bridge::wHandleGnomeCopy(Parent, CurrentBrowser, true);
	  break;
	case PASTE_KEY:
	  bridge::wHandleGnomePaste(Parent, Toolbar, CurrentBrowser);
      break;
	case PATHENTRY_KEY:
	  Parent->set_focus(*Pathentry);
	  Pathentry->set_position(-1);
	  break;
	case BROWSER_1_KEY:
	  bridge::wChangeBrowser(Parent, Browser_1);
	  break;
	case BROWSER_2_KEY:
	  bridge::wChangeBrowser(Parent, Browser_2);
	  break;
	case RETURN_KEY:
	  bridge::wNavigate(Parent, CurrentBrowser, Pathentry, CurrentBrowser->CurrentPath.parent_path());
	  break;
	case RUNSHELL_KEY:
	  Parent->set_focus(*Runentry);
	  break;
	}	
  } else if (event->keyval == REFRESH_KEY)
	bridge::wNavigate(Parent, CurrentBrowser, Pathentry, CurrentBrowser->CurrentPath);
  return false;
}
