#include <gtkmm.h>
#include <glibmm/timer.h>
#include <iostream>

class MainWindow {
    public:
        MainWindow(const Glib::RefPtr<Gtk::Builder>& builder) {           
            builder->get_widget("learning1", window);
            builder->get_widget("apply1", apply);
            builder->get_widget("input1", input);
            builder->get_widget("phrase1", phrase);
            builder->get_widget("counter1", counter);

            if (!(window && apply && input && phrase && counter)) {
                Gtk::MessageDialog dialog(*window, "Bad gui.glade file.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
                dialog.run();
                return;
            }

            inp_buf = input->get_buffer();
            inp_buf->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_txt_changed));

        }

        Gtk::Window* get_window() {
            return window;
        }

    private:
        Gtk::Window* window = nullptr;
        Gtk::Button* apply = nullptr;
        Gtk::TextView* input = nullptr;
        Gtk::Label* phrase = nullptr;
        Gtk::Label* counter = nullptr;

        bool is_phrase_set = false;
        bool is_first_char = true;

        Glib::RefPtr<Gtk::TextBuffer> inp_buf;
        Glib::Timer timer;

        std::string phrase_text;

        void on_txt_changed() {
            double time = timer.elapsed();
            timer.reset(); // <-------------

            auto text = inp_buf->get_text();
            if (!text.empty()) {
                char last_char = text[text.length() - 1];

                if (last_char == '\n') {
                    is_first_char = true;

                    if (!is_phrase_set) {
                        is_phrase_set = true;
                        phrase_text = text.substr(0, text.length() - 1);
                        phrase->set_text(phrase_text);

                        std::cout << std::endl;
                        std::cout << "Phrase: " << text;
                        std::cout << "-------------------------------"  << std::endl;
                        // save in var                    
                    } else {
                        std::cout << std::endl;
                        //save dalays
                    }

                } else {
                    if (!is_first_char) {
                        std::cout << time * 1000 << " ";
                    } else {
                        is_first_char = false;
                    }
                }
            }
        }
};

int main(int argc, char* argv[]) {
    setenv("GTK_THEME", "Adwaita", 1);
    auto gtk_app = Gtk::Application::create(argc, argv, "org.example.glade");

    Glib::RefPtr<Gtk::Builder> builder;
    builder = Gtk::Builder::create_from_file("gui.glade");

    MainWindow app_main(builder);
    
    return gtk_app->run(*app_main.get_window());

    // if (start) {
    //     start->signal_clicked().connect([stop, start, &timer_f] {
    //         stop->set_sensitive(true);
    //         start->set_sensitive(false);
    //         timer_f = true;
    //         std::cout << "Timer created!" << std::endl;
    //     });
    // }

    // if (stop) {
    //     stop->signal_clicked().connect([stop, start, &timer_f, &timer] {
    //         stop->set_sensitive(false);
    //         start->set_sensitive(true);
    //         timer_f = false;
    //         //timer = nullptr;
    //         std::cout << "Timer stoped!" << std::endl;
    //     });
    // }

    // auto buffer = input->get_buffer();

    // buffer->signal_changed().connect([buffer, &timer_f, &timer, &last_tm, &get_word] {
    //     double seconds = timer.elapsed();

    //     auto text = buffer->get_text();
    //     if (!text.empty()) {
    //         char last_char = text[text.length() - 1];
    //         if (last_char == '\n') {
    //             if (get_word == false) {
    //                 get_word = true;
    //                 //std::cout << buffer->get_text() << std::endl;
    //             }
    //             std::cout << "\n-" << std::endl;
    //             timer_f = false;
    //         } else {
    //             if (timer_f == false) {
    //                 timer_f = true;
    //                 last_tm = seconds;
    //             } else {
    //                 std::cout << (seconds - last_tm) * 1000 << " ";
    //                 last_tm = seconds;
    //                 //std::cout << last_char << std::endl;                    
    //             }
    //         }
    //     }
    // });
    // if (window)
    //     return app->run(*window);

    //return 0;
}
