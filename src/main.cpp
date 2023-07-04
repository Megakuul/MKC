#include <gtkmm.h>
#include <iostream>

class HelloWorldWindow : public Gtk::Window {
public:
    HelloWorldWindow() {
        set_title("Hello, World!");
        m_button.set_label("Click me!");
        m_button.signal_clicked().connect(sigc::mem_fun(*this, &HelloWorldWindow::on_button_clicked));
        add(m_button);
        show_all();
    }

protected:
    void on_button_clicked() {
        std::cout << "Hello, World!" << std::endl;
    }

private:
    Gtk::Button m_button;
};

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv);
    HelloWorldWindow window;
    return app->run(window);
}
