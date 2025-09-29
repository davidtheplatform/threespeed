#include <string>

namespace ts {
    extern std::string current_input;
    extern bool show_question;

    void set_question(std::string question);

    void confirm_answer();
    std::string get_response();

    int init_ask();
    void render_ask();
}