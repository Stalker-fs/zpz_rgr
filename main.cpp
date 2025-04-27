#include <gtkmm.h>
#include <glibmm/timer.h>
#include <iostream>
#include <boost/math/distributions/students_t.hpp>

#include "include/math_func.h"
#include "include/config_file.h"

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

            if (!(window && apply && reset && input && phrase && counter)) {
                Gtk::MessageDialog dialog(*window, "Bad gui.glade file.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
                dialog.run();
                return;
            }

            inp_buf = input->get_buffer();
            inp_buf->signal_changed().connect(sigc::mem_fun(*this, &SingUpWindow::on_txt_changed));

            reset->signal_clicked().connect(sigc::mem_fun(*this, &SingUpWindow::on_reset));
            apply->signal_clicked().connect(sigc::mem_fun(*this, &SingUpWindow::on_apply));
            // _exit->signal_clicked().connect([]() {exit(0);});
            _exit->signal_clicked().connect([this]() { window->hide(); });
            
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

        // Config conf;
        Config* conf = nullptr;

        void on_apply() {
            std::map<double, double> S2;
            for (std::vector<unsigned int>* x : delays) {
                double M = AVG(x);
                S2[M] = sv(x, M);
            }

            auto nm = name1->get_text();

            if (nm.empty()) {
                name1->grab_focus();
                return;
            }

            if (conf->is_user_exist(nm)) {
                Gtk::MessageDialog dialog(*window, "Username exist. Change it.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
                dialog.run();
                name1->select_region(0, name1->get_text_length());
                return;
            }

            conf->set_user(nm, phrase_text, S2);
            conf->save();
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

                            if (phrase_text.length() >= 4) {
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

                        if (check_delay_outliers2(delay_set)) {
                            delays.push_back(delay_set);
                            for (const unsigned int x : *delay_set) {
                                std::cout << x << " ";
                            }
                            std::cout << std::endl;
                            
                            counter->set_text(std::to_string(delays.size()));

                            if (delays.size() == 3) {
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

class MainWindow {
    public:

    MainWindow(const Glib::RefPtr<Gtk::Builder>& builder) : builder(builder) {
        builder->get_widget("main", window);
        builder->get_widget("enter1", enter);
        builder->get_widget("singup1", singup);
        builder->get_widget("check1", check);
        builder->get_widget("user1", user);

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
    std::unique_ptr<SingUpWindow> child_window;
    Glib::RefPtr<Gtk::Builder> builder;
    Config conf;

    void on_sign_up() {
        child_window = std::make_unique<SingUpWindow>(builder, *window, &conf);
        child_window->get_window()->show();
    }

    bool is_first_char = true;
    Glib::Timer timer;
    std::vector<unsigned int> a_delays;

    void on_txt_changed() {
        if (is_first_char) {
            is_first_char = false;
        } else {
            double time = timer.elapsed();
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

        int delay_count = enter->get_text_length() - 1;

        //std::cout << "Len: " << delay_count << std::endl;
        boost::math::students_t dist(delay_count - 1);
        double t_tb = boost::math::quantile(boost::math::complement(dist, (1 - 0.30) / 2));

        // for (unsigned int x : a_delays) {
        //     std::cout << x << " ";
        // }

        // std::cout << std::endl;

        double M = AVG(&a_delays);
        double S2 = sv(&a_delays, M);
        //std::cout << S2 << std::endl;

        std::vector<std::string> u_list;
        conf.get_user_list(u_list);

        std::pair<std::string, double> winner("Unknown", 0);

        for (std::string &u : u_list) {
            std::map<double, double> param;
            int r = 0;

            conf.get_M_S2(u, param);

            for (const auto& [M_e, S2_e] : param) {
                //std::cout << S2 << " " << S2_e << std::endl;
                long double S_general = S(S2, S2_e, delay_count);
                //std::cout << M_e << " " << M << " " << S_general << std::endl;
                long double t_p_ = t_value2(M_e, M, S_general, delay_count);

                std::cout << "T_p: " << t_p_ << " " << t_tb << std::endl;
                if (t_p_ <= t_tb) {
                    r++;
                }
            }

            std::cout << "r: " << r << std::endl;
            double P = r / (double)param.size();

            if (P > winner.second) {
                winner = {u, P};
            }
        }
        
        std::cout << "User: " << winner.first << "\nProbability: " << winner.second * 100 << std::endl;
        user->set_text(winner.first + " " + std::to_string((int)(winner.second * 100)) + "%");

        is_first_char = true;
        enter->set_text("");
        is_first_char = true;

        a_delays.clear();
        enter->set_sensitive(true);
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


// 1. n1 n2
// 2. phrase check