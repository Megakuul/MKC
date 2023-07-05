#include <gtkmm.h>
#include <iostream>
#include <main.hpp>
#include <Toolbar.hpp>

using namespace std;

MainWindow::MainWindow() : m_Box(Gtk::ORIENTATION_VERTICAL), m_Toolbar() {
   // The 'this' is not required here, but I feel like it is more readable with it
   this->set_title("XMDR");
   this->set_border_width(0);
   this->set_default_size(600, 400);

   this->add(m_Box);
   this->m_Box.pack_start(m_Toolbar, Gtk::PACK_SHRINK);

   show_all_children();
}

MainWindow::~MainWindow()
{

}

int main(int argc, char *argv[])
{
   auto app = Gtk::Application::create(argc, argv, "xmdr.megakuul.ch");

   MainWindow window;

   //Shows the window and returns when it is closed.
   return app->run(window);
}
