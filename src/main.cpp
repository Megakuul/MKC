#include <gtkmm.h>
#include <iostream>
#include <main.hpp>
#include <Toolbar.hpp>
#include <bridge.hpp>
#include <ctime>

using namespace std;

MainWindow::MainWindow() : m_Box(Gtk::ORIENTATION_VERTICAL), m_Toolbar(this), m_Browser() {
   // The 'this' is not required here, but I feel like it is more readable with it
   this->set_title("XMDR");
   this->set_border_width(0);
   this->set_default_size(1000, 800);

   this->add(m_Box);
   this->m_Box.pack_start(m_Toolbar, Gtk::PACK_SHRINK);
   this->m_Box.pack_start(m_Browser);

   m_Browser.AddElement("234", "das isssess", "f", "755", "root", time(nullptr));

   m_Browser.AddElement("53", "das isssess", "f", "755", "root", time(nullptr));

   m_Browser.AddElement("2344", "das isssess dir", "d", "754", "schlumpf", time(nullptr));

   m_Browser.AddElement("10", "Das File der unbegrenzten Machts", "f", "777", "root", time(nullptr));

   this->signal_key_press_event().connect(sigc::mem_fun(
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
