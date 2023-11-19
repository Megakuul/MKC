#include <gtkmm.h>
#include <bridge.hpp>
#include <keyhandler.hpp>
#include <fsutil.hpp>

bool HandleKeyPress(
    GdkEventKey* event,
	Gtk::Window* Parent,
	Browser* CurrentBrowser,
	Browser* Browser_1,
	Browser* Browser_2,
	Gtk::Entry* Pathentry) {
  
  if (event->state & GDK_CONTROL_MASK) {
	switch (event->keyval) {
	case ADD_FILE_KEY:
	  bridge::wAddFile(Parent, CurrentBrowser->CurrentPath);
	  break;
	case ADD_DIR_KEY:
	  bridge::wAddDir(Parent, CurrentBrowser->CurrentPath);
	  break;
	case DELETE_KEY:
		bridge::wDeleteObjects(Parent, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
	  break;
	case RECOVER_KEY:
	  bridge::wRestoreObject(Parent, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
	  break;
	case OPEN_RECOVER_KEY:
	  bridge::wChangeDir(
		    Parent, CurrentBrowser, Pathentry,
			std::filesystem::path(getenv("HOME")) / TRASH_PATH_REL);
	  break;
	case COPY_KEY:
	  bridge::wDirectCopyObjects(
	    Parent,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		false
	  );
	  break;
	case MOVE_KEY:
	  bridge::wDirectCopyObjects(
	    Parent,
		CurrentBrowser->CurrentPath,
		CurrentBrowser->RemoteBrowser->CurrentPath,
		CurrentBrowser->GetSelectedNames(),
		true
	  );
	  break;
	case PATHENTRY_KEY:
	  Parent->set_focus(*Pathentry);
	  break;
	case BROWSER_1_KEY:
	  bridge::wChangeBrowser(Parent, Browser_1);
	  break;
	case BROWSER_2_KEY:
	  bridge::wChangeBrowser(Parent, Browser_2);
	  break;
	case RETURN_KEY:
	  bridge::wChangeDir(Parent, CurrentBrowser, Pathentry, CurrentBrowser->CurrentPath.parent_path());
	  break;
	}
  }
  return false;
}
