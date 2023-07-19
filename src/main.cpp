#include <gtkmm.h>
#include <iostream>
#include <main.hpp>
#include <Toolbar.hpp>
#include <bridge.hpp>
#include <ctime>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

MainWindow::MainWindow() : m_MainBox(Gtk::ORIENTATION_VERTICAL),
                           m_Toolbar(this, CurrentBrowser),
                           m_PathentryBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Returnbtn(),
                           m_Pathentry(),
                           m_SplitBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Browser_1(this, "/", CurrentBrowser, &m_Pathentry),
                           m_Browser_2(this, "/", CurrentBrowser, &m_Pathentry),
                           m_Scrollable_1(),
                           m_Scrollable_2(),
                           CurrentBrowser(&m_Browser_1) {

   set_title("Megakuul Commander");
   override_background_color(Gdk::RGBA("rgba(107, 82, 82, 0.212)"));
   set_border_width(0);
   set_default_size(1000, 800);

   // PathentryBox //
   m_PathentryBox.set_margin_top(10);
   m_PathentryBox.set_margin_bottom(10);
   m_PathentryBox.set_margin_right(10);
   m_PathentryBox.set_margin_left(10);
   m_PathentryBox.pack_start(m_Returnbtn, Gtk::PACK_SHRINK);
   m_PathentryBox.pack_start(m_Pathentry, Gtk::PACK_EXPAND_WIDGET);
   // PathentryBox //

   // Returnbtn //
   m_Returnbtn.set_stock_id(Gtk::Stock::GO_UP);
   m_Returnbtn.set_tooltip_text("CTRL + Z");
   m_Returnbtn.set_margin_right(5);
   m_Returnbtn.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::on_returnbtn_clicked)
   );
   // Returnbtn //

   // Pathentry //
   m_Pathentry.set_tooltip_text("CTRL + T");
   m_Pathentry.set_margin_left(5);
   m_Pathentry.signal_activate().connect(
      sigc::mem_fun(*this, &MainWindow::on_pathentry_activate)
   );
   // Pathentry //

   // Splitbrowser //
   m_Scrollable_1.add(m_Browser_1);
   m_Scrollable_1.set_border_width(5);
   m_Scrollable_2.add(m_Browser_2);
   m_Scrollable_2.set_border_width(5);
   m_SplitBox.pack_start(m_Scrollable_1);
   m_SplitBox.pack_start(m_Scrollable_2);
   // Splitbrowser //

   // MainBox //
   add(m_MainBox);
   m_MainBox.pack_start(m_Toolbar, Gtk::PACK_SHRINK);
   m_MainBox.pack_start(m_PathentryBox, Gtk::PACK_SHRINK);
   m_MainBox.pack_start(m_SplitBox);
   // MainBox //

   // Initial Navigate to the first Browser
   bridge::wChangeBrowser(this, &m_Browser_1);

   signal_key_press_event().connect(sigc::mem_fun(
      *this,
      &MainWindow::on_key_press
   ), false);

   show_all_children();
}

bool MainWindow::on_key_press(GdkEventKey* event)
{  
   if (event->state & GDK_CONTROL_MASK) {
      switch (event->keyval) {
         case GDK_KEY_1:
            bridge::wAddFile(this, CurrentBrowser->CurrentPath);
         break;
         case GDK_KEY_2:
            bridge::wAddDir(this, CurrentBrowser->CurrentPath);
         break;
         case GDK_KEY_3:
            bridge::wDeleteObjects(this, CurrentBrowser->CurrentPath, CurrentBrowser->GetSelectedNames());
         break;
         case GDK_KEY_t:
            set_focus(m_Pathentry);
         break;
         case GDK_KEY_q:
            bridge::wChangeBrowser(this, &m_Browser_1);
         break;
         case GDK_KEY_e:
            bridge::wChangeBrowser(this, &m_Browser_2);
         break;
         case GDK_KEY_z:
            bridge::wChangeDir(this, CurrentBrowser, &m_Pathentry, CurrentBrowser->CurrentPath.parent_path());
         break;
         case GDK_KEY_v:
            bridge::wHandlePaste(CurrentBrowser);
         break;
      }
   }
   return false;
}

void MainWindow::on_returnbtn_clicked() {
   bridge::wChangeDir(this, CurrentBrowser, &m_Pathentry, CurrentBrowser->CurrentPath.parent_path());
}

void MainWindow::on_pathentry_activate() {
   bridge::wChangeDir(this, CurrentBrowser, &m_Pathentry, fs::path(m_Pathentry.get_text()));
}

int main(int argc, char *argv[])
{
   auto app = Gtk::Application::create(argc, argv, "mkc.megakuul.ch");

   MainWindow window;

   GError *error = NULL;
   GdkPixbuf *favicon = gdk_pixbuf_new_from_file("/etc/mkc/favicon.png", &error);
   if (!favicon) {
      cerr << "Error loading icon: " << error->message << endl;
      g_error_free(error);
   } else {
      gtk_window_set_default_icon(favicon);
   }

   return app->run(window);
}
