#include <gtkmm.h>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <mutex>

#include "RunShell.hpp"

using namespace std;

RunShell::RunShell(Gtk::Entry *Pathentry) {
  pathentry = Pathentry;
  dispatcher.connect(sigc::mem_fun(*this, &RunShell::on_shell_update));
  signal_activate().connect(sigc::mem_fun(*this, &RunShell::on_shell_activate));
}

void RunShell::on_shell_activate() {
  {
	lock_guard<mutex> guard(mut);
	if (process!=0) {
	  kill(process, SIGKILL);
	  process = 0;
	  dispatcher.emit();
	  return;
	}
  }
  
  pid_t pid = fork();
  if (pid == -1) return;
  if (pid == 0) {
	chdir(pathentry->get_text().c_str());
	execl("/bin/sh", "sh", "-c", get_text().c_str(), (char*)NULL);

	exit(EXIT_FAILURE);
  } else {
	{
	  lock_guard<mutex> guard(mut);
	  process = pid;
	}
	thread([this, pid]() {
	  int status;
	  waitpid(pid, &status, 0);
	  dispatcher.emit();
	}).detach();
  }
}

void RunShell::on_shell_update() {
  set_text("");
  lock_guard<mutex> guard(mut);
  if (process!=0) {
	cout << "Updated Up" << endl;
	set_margin_top(10);
  } else {
	cout << "Updated Down" << endl;
	set_margin_top(0);
  }
}
