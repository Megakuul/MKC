#include <gtkmm.h>
#include <iostream>
#include <main.hpp>
#include <Toolbar.hpp>
#include <bridge.hpp>
#include <ctime>

using namespace std;

unique_ptr<string> CurrentContext = make_unique<string>("/");

MainWindow::MainWindow() : m_MainBox(Gtk::ORIENTATION_VERTICAL),
                           m_Toolbar(this),
                           m_Pathentry(),
                           m_SplitBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Browser_1(),
                           m_Browser_2(),
                           m_Scrollable_1(),
                           m_Scrollable_2() {

   set_title("XMDR");
   set_name("mainwindow");
   set_border_width(0);
   set_default_size(1000, 800);

   add(m_MainBox);
   m_MainBox.pack_start(m_Toolbar, Gtk::PACK_SHRINK);
   m_MainBox.pack_start(m_Pathentry, Gtk::PACK_SHRINK);

   m_Scrollable_1.add(m_Browser_1);
   m_Scrollable_1.set_border_width(5);
   m_Scrollable_2.add(m_Browser_2);
   m_Scrollable_2.set_border_width(5);
   m_SplitBox.pack_start(m_Scrollable_1);
   m_SplitBox.pack_start(m_Scrollable_2);
   m_MainBox.pack_start(m_SplitBox);

   bridge::wChangeDir(&m_Browser_1, "/usr/include");
   bridge::wChangeDir(&m_Browser_2, "/");

   signal_key_press_event().connect(sigc::mem_fun(
      *this,
      &MainWindow::onKeyPress
   ), false);

   show_all_children();
}

bool MainWindow::onKeyPress(GdkEventKey* event)
{
   if (GDK_CONTROL_MASK) {
      switch (event->keyval) {
         case GDK_KEY_1:
            bridge::wAddFile(this, "/");
         break;
         case GDK_KEY_2:
            bridge::wAddDir(this, "/");
         break;
      }
   }

   return false;
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
