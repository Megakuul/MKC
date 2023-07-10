#include <gtkmm.h>
#include <iostream>
#include <main.hpp>
#include <Toolbar.hpp>
#include <bridge.hpp>
#include <ctime>

using namespace std;

MainWindow::MainWindow() : m_MainBox(Gtk::ORIENTATION_VERTICAL),
                           m_Toolbar(this),
                           m_Pathentry(),
                           m_SplitBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Browser_1("/"),
                           m_Browser_2("/"),
                           m_Scrollable_1(),
                           m_Scrollable_2(),
                           CurrentBrowser(&m_Browser_1) {

   set_title("XMDR");
   set_name("mainwindow");
   set_border_width(0);
   set_default_size(1000, 800);

   add(m_MainBox);
   m_MainBox.pack_start(m_Toolbar, Gtk::PACK_SHRINK);
   m_Pathentry.set_tooltip_text("CTRL + T");
   m_Pathentry.signal_activate().connect(
      sigc::mem_fun(*this, &MainWindow::on_pathentry_activate
   ));
   m_MainBox.pack_start(m_Pathentry, Gtk::PACK_SHRINK);

   m_Scrollable_1.add(m_Browser_1);
   m_Scrollable_1.set_border_width(5);
   m_Scrollable_2.add(m_Browser_2);
   m_Scrollable_2.set_border_width(5);
   m_SplitBox.pack_start(m_Scrollable_1);
   m_SplitBox.pack_start(m_Scrollable_2);
   m_MainBox.pack_start(m_SplitBox);

   bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_1);

   bridge::wChangeDir(&m_Browser_1, "/usr/include");
   bridge::wChangeDir(&m_Browser_2, "/");

   signal_key_press_event().connect(sigc::mem_fun(
      *this,
      &MainWindow::on_key_press
   ), false);

   show_all_children();
}

bool MainWindow::on_key_press(GdkEventKey* event)
{  
   // This should be a switch statement, but weirdly it did not work consistently
   // TODO: rebuild it as switch
   if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_1) {
      bridge::wAddFile(this, CurrentBrowser->CurrentPath);
   }
   else if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_2) {
      bridge::wAddDir(this, CurrentBrowser->CurrentPath);
   }
   else if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_t) {
      set_focus(m_Pathentry);
   }
   else if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_q) {
      bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_1);
   }
   else if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_e) {
      bridge::wChangeBrowser(this, &m_Pathentry, CurrentBrowser, &m_Browser_2);
   }
   return false;
}

void MainWindow::on_pathentry_activate() {
   bridge::wChangeDir(CurrentBrowser, m_Pathentry.get_text());
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
