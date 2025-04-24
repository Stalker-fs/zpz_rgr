#include <gtkmm.h>
#include <glibmm/timer.h>
#include <iostream>

#include "include/math_func.h"
#include "include/config_file.h"

class MainWindow {
    public:
        MainWindow(const Glib::RefPtr<Gtk::Builder>& builder) {        
            builder->get_widget("learning1", window);
            builder->get_widget("apply1", apply);
            builder->get_widget("reset1", reset);
            builder->get_widget("exit1", _exit);
            builder->get_widget("input1", input);
            builder->get_widget("phrase1", phrase);
            builder->get_widget("counter1", counter);

            if (!(window && apply && reset && input && phrase && counter)) {
                Gtk::MessageDialog dialog(*window, "Bad gui.glade file.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                dialog.run();
                return;
            }

            inp_buf = input->get_buffer();
            inp_buf->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_txt_changed));

            reset->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_reset));
            apply->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_apply));
            _exit->signal_clicked().connect([]() {exit(0);});
        }

        Gtk::Window* get_window() {
            return window;
        }

    private:
        Gtk::Window* window = nullptr;
        Gtk::Button* apply = nullptr;
        Gtk::Button* reset = nullptr;
        Gtk::Button* _exit = nullptr;
        Gtk::TextView* input = nullptr;
        Gtk::Label* phrase = nullptr;
        Gtk::Label* counter = nullptr;

        bool is_phrase_set = false;
        bool is_first_char = true;

        Glib::RefPtr<Gtk::TextBuffer> inp_buf;
        Glib::Timer timer;

        std::string phrase_text;
        int phrase_pos = 0;

        std::vector<std::vector<unsigned int>*> delays;
        std::vector<unsigned int>* delay_set = nullptr; 

        void on_apply() {
            ConfigData data;
            data.delays = delays;
            data.max_S = 10;

            save_delays(data);
        }

        void on_reset() {
            is_phrase_set = false;
            is_first_char = true;

            phrase_text.clear();
            phrase_pos = 0;

            delays.clear();
            delay_set = nullptr;

            phrase->set_text("-");
            inp_buf->set_text("");
            counter->set_text("0");
            reset->set_sensitive(false);
            apply->set_sensitive(false);
        }

        void on_txt_changed() {
            double time = timer.elapsed();
            timer.reset();

            auto text = inp_buf->get_text();
            if (!text.empty()) {
                char last_char = text[text.length() - 1];

                if (last_char == '\n') {
                    is_first_char = true;
                    phrase_pos = 0;

                    //save delays
                    if (delay_set) {
                        if (is_phrase_set && (delay_set->size() != phrase_text.length()-1)) {
                            Gtk::MessageDialog dialog(*window, "Bad input. Incomplete phrase. Try again.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                            dialog.run();
                            return;
                        }

                        if (!is_phrase_set) {
                            phrase_text = text.substr(0, text.length() - 1);

                            if (phrase_text.find('\n') != std::string::npos) {
                                Gtk::MessageDialog dialog(*window, "Bad input. '\\n' in phase. Don't use it!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                                dialog.run();
                                phrase_text.clear();
                                delay_set = nullptr;
                                inp_buf->set_text("");
                                return;
                            }

                            if (phrase_text.length() >= 3) {
                                is_phrase_set = true;
                                phrase->set_text(phrase_text);
                                reset->set_sensitive(true);

                                //std::cout << std::endl;
                                std::cout << "Phrase: " << text;
                                //std::cout << "-------------------------------"  << std::endl;                                
                            } else {
                                Gtk::MessageDialog dialog(*window, "Bad input. Short phrase. Min phrase len is: 3.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                                dialog.run();
                                phrase_text.clear();
                                delay_set = nullptr;
                                inp_buf->set_text("");
                                return;
                            }
                
                        }

                        if (check_delay_outliers(delay_set)) {
                            delays.push_back(delay_set);
                            for (const unsigned int x : *delay_set) {
                                std::cout << x << " ";
                            }
                            std::cout << std::endl;
                            
                            counter->set_text(std::to_string(delays.size()));

                            if (delays.size() == 10) {
                                apply->set_sensitive(true);
                            }                            
                        } else {
                            Gtk::MessageDialog dialog(*window, "Bad input. check_delay_outliers is false. Try again.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                            dialog.run();
                        }

                        delay_set = nullptr;
                    }

                } else {
                    if (is_phrase_set) {
                        if ((phrase_pos >= phrase_text.length()) || (phrase_text[phrase_pos] != last_char)) {
                            phrase_pos = 0;
                            delay_set = nullptr;
                            Gtk::MessageDialog dialog(*window, "Bad input. Try again.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                            dialog.run();
                            inp_buf->insert_at_cursor("\n");
                            return;
                        } else {
                            phrase_pos++;
                        }                        
                    }

                    if (!is_first_char) {
                        delay_set->push_back((int)(time * 1000000)); // µs
                        //std::cout << delay_set->back() << " ";
                    } else {
                        is_first_char = false;
                        delay_set = new std::vector<unsigned int>();
                    }
                }
            }
        }
};

int main(int argc, char* argv[]) {
    setenv("GTK_THEME", "Adwaita", 1);
    auto gtk_app = Gtk::Application::create(argc, argv, "org.example.glade");

    Glib::RefPtr<Gtk::Builder> builder;
    builder = Gtk::Builder::create_from_file("../gui.glade");

    MainWindow app_main(builder);
    
    return gtk_app->run(*app_main.get_window());
}
