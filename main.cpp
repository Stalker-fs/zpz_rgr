#include <gtkmm.h>
#include <glibmm/timer.h>

#include "include/config.h"
#include "include/processing.h"

class SingUpWindow : public Gtk::Window {
    public:
        SingUpWindow(const Glib::RefPtr<Gtk::Builder>& builder, Gtk::Window& parent, Config* conf_link) : conf(conf_link) {
            set_transient_for(parent);
            set_modal(true);

            builder->get_widget("learning1", window);
            builder->get_widget("apply1", apply);
            builder->get_widget("reset1", reset);
            builder->get_widget("exit1", _exit);
            builder->get_widget("input1", input);
            builder->get_widget("name1", name1);
            builder->get_widget("phrase1", phrase);
            builder->get_widget("counter1", counter);

            inp_buf = input->get_buffer();
            inp_buf->signal_changed().connect(sigc::mem_fun(*this, &SingUpWindow::on_txt_changed));

            reset->signal_clicked().connect(sigc::mem_fun(*this, &SingUpWindow::on_reset));
            apply->signal_clicked().connect(sigc::mem_fun(*this, &SingUpWindow::on_apply));
            _exit->signal_clicked().connect([this]() { window->hide();});
            
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
        Gtk::Entry* name1 = nullptr;
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

        Config* conf = nullptr;

        void on_apply() {
            std::map<float, float> S2;
            calc_variance(delays, S2);

            auto nm = name1->get_text();

            if (nm.empty()) {
                name1->grab_focus();
                return;
            }

            if (conf->is_user_exist(nm)) {
                name1->select_region(0, name1->get_text_length());
                Gtk::MessageDialog dialog(*window, "Username exist. Change it.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
                dialog.run();
                return;
            }

            conf->set_user(nm, phrase_text, S2);
            on_reset();
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
            name1->set_text("");
            counter->set_text("0");
            reset->set_sensitive(false);
            apply->set_sensitive(false);
        }

        void on_txt_changed() {
            float time = timer.elapsed();
            timer.reset();

            auto text = inp_buf->get_text();
            if (!text.empty()) {
                char last_char = text[text.length() - 1];

                if (last_char == '\n') {
                    is_first_char = true;
                    phrase_pos = 0;

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

                            if (phrase_text.length() >= 4) {
                                is_phrase_set = true;
                                phrase->set_text(phrase_text);
                                reset->set_sensitive(true);                              
                            } else {
                                Gtk::MessageDialog dialog(*window, "Bad input. Short phrase. Min phrase len is: 4.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                                dialog.run();

                                phrase_text.clear();
                                delay_set = nullptr;
                                inp_buf->set_text("");
                                return;
                            }
                
                        }

                        if (check_delay_outliers2(delay_set)) {
                            delays.push_back(delay_set);
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
                    } else {
                        is_first_char = false;
                        delay_set = new std::vector<unsigned int>();
                    }
                }
            }
        }
};

class MainWindow {
    public:

    MainWindow(const Glib::RefPtr<Gtk::Builder>& builder) : builder(builder) {
        builder->get_widget("main", window);
        builder->get_widget("enter1", enter);
        builder->get_widget("singup1", singup);
        builder->get_widget("check1", check);
        builder->get_widget("user1", user);
        builder->get_widget("probability1", prob);
        builder->get_widget("phrase2", phrase);
        builder->get_widget("status1", status);

        singup->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_sign_up));
        check->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_check));
        enter->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_check));
        enter->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_txt_changed)); 
    }

    Gtk::Window* get_window() {
        return window;
    }

    private:

    Gtk::Window* window = nullptr;
    Gtk::Entry* enter = nullptr;
    Gtk::Button* singup = nullptr;
    Gtk::Button* check = nullptr;
    Gtk::Label* user = nullptr;
    Gtk::Label* prob = nullptr;
    Gtk::Label* phrase = nullptr;
    Gtk::Label* status = nullptr;
    std::unique_ptr<SingUpWindow> child_window;
    Glib::RefPtr<Gtk::Builder> builder;
    Config conf;

    void on_sign_up() {
        if (!child_window) {
            child_window = std::make_unique<SingUpWindow>(builder, *window, &conf);
        }
        child_window->get_window()->show();
    }

    bool is_first_char = true;
    Glib::Timer timer;
    std::vector<unsigned int> a_delays;

    void on_txt_changed() {
        if (is_first_char) {
            is_first_char = false;
        } else {
            float time = timer.elapsed();
            a_delays.push_back((int)(time * 1000000));
        }

        timer.reset();
    }

    void on_check() {
        enter->set_sensitive(false);

        if (enter->get_text_length() <= 3) {
            enter->set_text("");
            is_first_char = true;
            a_delays.clear();

            Gtk::MessageDialog dialog(*window, "Short phrase.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dialog.run();
            
            enter->set_sensitive(true);
            enter->grab_focus();
            return;
        }

        std::pair<std::string, float> winner("Unknown", 0);
        search_user(a_delays, conf, winner);

        user->set_text(winner.first);
        prob->set_text(std::to_string((int)(winner.second * 100)) + "%");
        
        if (winner.second > 0) {
            if (conf.check_credentials(winner.first, enter->get_text())) {
                phrase->set_markup("<span foreground=\"green\">Correct</span>");
                if ((int)(winner.second * 100) >= 70) {
                    status->set_markup("<span foreground=\"green\">Authorized</span>");
                } else {
                    status->set_markup("<span foreground=\"red\">Unauthorized</span>");
                }
            } else {
                phrase->set_markup("<span foreground=\"red\">Fail</span>");
                status->set_markup("<span foreground=\"red\">Unauthorized</span>");
            }
        } else {
            phrase->set_text("-");
            status->set_text("-");
        }

        is_first_char = true;
        enter->set_text("");
        is_first_char = true;

        a_delays.clear();
        enter->set_sensitive(true);
        enter->grab_focus();
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

