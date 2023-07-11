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
                           m_Toolbar(this),
                           m_PathentryBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Returnbtn(),
                           m_Pathentry(),
                           m_SplitBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Browser_1("/"),
                           m_Browser_2("/"),
                           m_Scrollable_1(),
                           m_Scrollable_2(),
                           CurrentBrowser(&m_Browser_1) {

   set_title("Megakuul Commander");
   set_name("mainwindow");
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
   bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_1);

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
         case GDK_KEY_t:
            set_focus(m_Pathentry);
         break;
         case GDK_KEY_q:
            bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_1);
         break;
         case GDK_KEY_e:
            bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_2);
         break;
         case GDK_KEY_z:
            bridge::wChangeDir(CurrentBrowser, &m_Pathentry, CurrentBrowser->CurrentPath.parent_path());
         break;
      }
   }
   return false;
}

void MainWindow::on_returnbtn_clicked() {
   bridge::wChangeDir(CurrentBrowser, &m_Pathentry, CurrentBrowser->CurrentPath.parent_path());
}

void MainWindow::on_pathentry_activate() {
   bridge::wChangeDir(CurrentBrowser, &m_Pathentry, fs::path(m_Pathentry.get_text()));
}

int main(int argc, char *argv[])
{
   auto app = Gtk::Application::create(argc, argv, "xmdr.megakuul.ch");

   MainWindow window;
   
   auto css = Gtk::CssProvider::create();
   css->load_from_path("../style/style.css");
   Gtk::StyleContext::add_provider_for_screen(
      Gdk::Screen::get_default(),
      css,
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
   );


   //Shows the window and returns when it is closed.
   return app->run(window);
}
