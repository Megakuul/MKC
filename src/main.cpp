#include <gtkmm.h>
#include <iostream>
#include <ctime>
#include <filesystem>

#include "main.hpp"
#include "Toolbar.hpp"
#include "bridge.hpp"
#include "keyhandler.hpp"

#define BACKGROUND_COLOR "rgba(107, 82, 82, 0.212)"

using namespace std;
namespace fs = filesystem;

MainWindow::MainWindow(vector<Glib::ustring> RunCompletions) : m_MainBox(Gtk::ORIENTATION_VERTICAL),
                           m_Toolbar(this, CurrentBrowser),
                           m_PathentryBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Returnbtn(),
                           m_Pathentry(),
						   m_Runentry(CurrentBrowser, RunCompletions),
                           m_SplitBox(Gtk::ORIENTATION_HORIZONTAL),
                           m_Browser_1(this, "/", CurrentBrowser, &m_Browser_2, &m_Pathentry),
                           m_Browser_2(this, "/", CurrentBrowser, &m_Browser_1, &m_Pathentry),
                           m_Scrollable_1(),
                           m_Scrollable_2(),
                           CurrentBrowser(&m_Browser_1) {

  set_title("Megakuul Commander");
  override_background_color(Gdk::RGBA(BACKGROUND_COLOR));
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
  m_Returnbtn.set_tooltip_text(RETURN_KEY_LB);
  m_Returnbtn.set_margin_right(5);
  m_Returnbtn.signal_clicked().connect(
    sigc::mem_fun(*this, &MainWindow::on_returnbtn_clicked)
  );
  // Returnbtn //

  // Pathentry //
  m_Pathentry.set_tooltip_text(PATHENTRY_KEY_LB);
  m_Pathentry.set_margin_left(5);
  m_Pathentry.signal_activate().connect(
    sigc::mem_fun(*this, &MainWindow::on_pathentry_activate)
  );
  m_Pathentry.signal_key_press_event().connect(
    sigc::mem_fun(*this, &MainWindow::on_pathentry_key_press)
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
  m_MainBox.pack_end(m_Runentry, Gtk::PACK_SHRINK);
  // MainBox //

  // Initial Navigate to the first Browser
  bridge::wChangeBrowser(this, &m_Browser_1);
  // Initial set directories
  bridge::wChangeDir(this, &m_Browser_1, &m_Pathentry, "/");
  bridge::wChangeDir(this, &m_Browser_2, &m_Pathentry, fs::path(getenv("HOME")));

  signal_key_press_event().connect(sigc::mem_fun(
    *this,
	&MainWindow::on_key_press
  ), false);

  show_all_children();
}

bool MainWindow::on_key_press(GdkEventKey* event)
{  
  return HandleKeyPress(event, this, CurrentBrowser, &m_Browser_1, &m_Browser_2, &m_Pathentry, &m_Runentry);
}

void MainWindow::on_returnbtn_clicked() {
  bridge::wChangeDir(this, CurrentBrowser, &m_Pathentry, CurrentBrowser->CurrentPath.parent_path());
}

void MainWindow::on_pathentry_activate() {
  bridge::wChangeDir(this, CurrentBrowser, &m_Pathentry, fs::path(m_Pathentry.get_text()));
}

bool MainWindow::on_pathentry_key_press(GdkEventKey* event) {
  switch (event->keyval) {
  case GDK_KEY_Tab:
	bridge::wAutoComplete(&m_Pathentry, CurrentBrowser);
	return true;
  default:
	return false;
  }
}

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "mkc.megakuul.ch");

  // TODO: Read additional completions from e.g. /etc/mkc/completions.conf
  vector<Glib::ustring> run_completions = {
	"zip ",
	"unzip ",
	"gzip ",
	"gunzip ",
	"tar -czvf ",
	"tar -xzvf ",
  };

  MainWindow window(run_completions);

  GError *error = NULL;
  GdkPixbuf *favicon = gdk_pixbuf_new_from_file("/usr/share/mkc/favicon.png", &error);
  if (!favicon) {
	cerr << "Error loading icon: " << error->message << endl;
	g_error_free(error);
  } else {
	gtk_window_set_default_icon(favicon);
  }

  return app->run(window);
}
