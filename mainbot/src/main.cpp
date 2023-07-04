#include <templatebot/templatebot.h>
#include <config.h>

using std::cout;
using std::string;
using std::get;
using std::getline;
using std::to_string;
using std::fstream;
using std::ios;
using std::vector;
namespace fs = std::filesystem;


/* for task 3 */
int number_for_guess = 0;

/* 1A2B */
vector<int> abgame_answer;
bool abgame_started = false;
int guess_count;

/* check if a string is a number */
bool is_number(const string& s){
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return (it == s.end());
}

/* check if a string is repeated */
bool is_repeated(const string& s){
    int count[10] = {0};
    for(auto& c: s){
        if(count[c - '0']) return true;
        count[c - '0']++;
    }
    return false;
}

/* TODO List */
struct Date{
    int year, month, day;
    Date(){};
    Date(string date){
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(4, 2));
        day = stoi(date.substr(6, 2));
    };
};

struct ToDo: public Date{
    bool complete;
    string id, raw_date, todo;
    Date date;
    ToDo(bool complete, string id, string raw_date, string todo): 
    complete(complete), id(id), raw_date(raw_date), todo(todo), date(Date(raw_date)){};
};

bool todo_cmp(ToDo& a, ToDo& b){
    if(a.date.year != b.date.year)
        return a.date.year < b.date.year;
    if(a.date.month != b.date.month)
        return a.date.month < b.date.month;
    return a.date.day < b.date.day;
};

int main(int argc, char const *argv[]){
    /* Setup random seed */
    std::srand(std::time(NULL));

    /* todo id initialize */
    fstream todo_id_file("todolist/id.txt", ios::in);
    int todo_id; todo_id_file >> todo_id;
    todo_id_file.close();
    cout << "todo id: " << todo_id << '\n';

    /* Setup the bot */
    dpp::cluster bot(TOKEN);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([&](const dpp::slashcommand_t& event) {
        dpp::interaction interaction = event.command;

        if (interaction.get_command_name() == "ping") {
            event.reply("Pong!");
        }

        /* task 1.1 */
        else if (interaction.get_command_name() == "greeting") {
            /* fetch a parameter value from the command parameters */
            string name = get<string>(event.get_parameter("username"));
            /* reply to the command */
            event.reply(string("Hello ") + name);
        }

        /* task 2.1 */
        else if (interaction.get_command_name() == "add") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Add] The result is ") + to_string(num1) + " + " + to_string(num2) + " = " +to_string(num1+num2));
        }

        /* task 2.2 */
        else if (interaction.get_command_name() == "sub") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Sub] The result is ") + to_string(num1) + " - " + to_string(num2) + " = " +to_string(num1-num2));
        }

        /* task 2.3 */
        else if (interaction.get_command_name() == "mul") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Mul] The result is ") + to_string(num1) + " * " + to_string(num2) + " = " +to_string(num1*num2));
        }

        /* task 3.1 */
        else if (interaction.get_command_name() == "reset") {
            /* set the answer in range [1, 100] */
            number_for_guess = rand() % 100 + 1;
            // event.reply(to_string(number_for_guess));
        }

        /* task 3.2 */
        else if (interaction.get_command_name() == "guess") {
            int num = stoi(get<string>(event.get_parameter("number_guess")));
            string ret;
            if (num == number_for_guess) {
                ret = "Bingo!";
                /* reset the answer if Bingo */
                number_for_guess = rand() % 100 + 1;
            }
            else if (num > number_for_guess) ret = "Guess a smaller number!";
            else ret = "Guess a larger number!";
            event.reply(ret);
        }

        /* task 4.1 */
        else if (interaction.get_command_name() == "write") {
            /* Instantiate an interaction_modal_response object */
            dpp::interaction_modal_response modal("diary", "Please enter your diary");
            /* Add a text component */
            modal.add_component(
                dpp::component().
                set_label("DATE(IN FORMS OF YYYYMMDD)").
                set_id("date").
                set_type(dpp::cot_text).
                set_placeholder("YYYYMMDD").
                set_min_length(8).
                set_max_length(8).
                set_text_style(dpp::text_short)
            );
            /* add another text component in the next row, as required by discord*/
            modal.add_row();
            modal.add_component(
                dpp::component().
                set_label("TITLE").
                set_id("title").
                set_type(dpp::cot_text).
                set_placeholder("title here").
                set_min_length(1).
                set_max_length(100).
                set_text_style(dpp::text_short)
            );
            modal.add_row();
            modal.add_component(
                dpp::component().
                set_label("YOUR DIARY").
                set_id("diary_content").
                set_type(dpp::cot_text).
                set_placeholder("content here").
                set_min_length(1).
                set_max_length(2000).
                set_text_style(dpp::text_paragraph)
            );
            /* trigger the dialog box. all dialog boxes are ephemeral */
            event.dialog(modal);
        }

        /* task 4.2 */
        else if (interaction.get_command_name() == "read") {
            string date = get<string>(event.get_parameter("date"));
            fstream diary_read("diaries/" + date + ".txt", ios::in);
            if(diary_read){
                string title, line, contents = "";
                getline(diary_read, title);
                while(!diary_read.eof()){
                    getline(diary_read, line);
                    cout << "line: " << line << '\n';
                    contents += line;
                    contents += '\n';
                }
                cout << "contents: " << contents << '\n';
                /* create the embed then add it to dpp::message */
                dpp::embed embed = dpp::embed().
                    set_color(dpp::colors::sti_blue).
                    set_title(title).
                    add_field(
                            "Date",
                            date
                    ).
                    add_field(
                            "Content",
                            contents
                    ).
                    set_footer(dpp::embed_footer().set_text("My Diary at " + date)).
                    set_timestamp(time(0));

                diary_read.close();

                dpp::message m;
                m.add_embed(embed);
                event.reply(m);
            }else{
                /* file not opened */
                event.reply("Diary not found!!!!");
            }
        }

        /* task 4.3 */
        else if (interaction.get_command_name() == "remove") {
            string date = get<string>(event.get_parameter("date")), ret;
            try {
                /* use std::filesystem to remove */
                if (fs::remove("diaries/" + date + ".txt")) ret = "Diary deleted successfully :)";
                else ret = "Diary deletion failed :(";
            }catch(...){
                /* catch any */
                ret = "Diary deletion failed :(";
            }
            event.reply(ret);
        }
        
        /* custom things */
        else if (interaction.get_command_name() == "calculator") {
            event.reply("You can use these commands for calculator:\n`/add` `/sub` `/mul`");
        }

        else if (interaction.get_command_name() == "diary") {
            event.reply("You can use these commands for diary:\n`/write` `/read` `/remove`");
        }

        // 1A2B
        else if (interaction.get_command_name() == "1a2b") {
            /* Get the sub command */
            auto subcommand = interaction.get_command_interaction().options[0];
            if (subcommand.name == "start") {
                /* reset abgame_answer */
                abgame_answer.clear();
                for(int i=0; i<10; i++) abgame_answer.push_back(-1);
                bool chose[10] = {};
                cout << "1A2B answer: ";
                int n;
                for(int i=0; i<4; i++){
                    do{
                        /* number in [0, 9] */
                        n = rand() % 10;
                    }while(chose[n]);
                    abgame_answer[n] = i;
                    chose[n] = true;
                    cout << n;
                }
                cout << '\n';
                abgame_started = true;
                guess_count = 0;
                event.reply("New game started!");
            }
            
            else if (subcommand.name == "guess") {
                string ret, guess = get<string>(event.get_parameter("number"));
                bool valid = true;
                /* check if input is valid or is game started */
                if((guess.length() != 4) || (!is_number(guess)) || is_repeated(guess)) valid = false;
                if(!abgame_started) ret = "Please start a new game first.";
                else if(!valid) ret = "Not a valid guess.";
                /* counting the A and B */
                else{
                    int a = 0, b = 0, indx = 0;
                    for(auto& i: guess){
                        int answer_index = abgame_answer[i - '0'];
                        if(answer_index == indx) a++;
                        else if(answer_index != -1) b++;
                        indx++;
                    }
                    if(a != 4){
                        ret = guess + ": " + to_string(a) + "A" + to_string(b) + "B";
                        /* record the number of guesses */
                        guess_count++;
                    }else{
                        ret = "Congrats! `" + guess + "` is the correct answer!\nYou used " + to_string(guess_count+1) + " guesses.";
                        /* reset when Bingo */
                        abgame_started = false;
                    }
                }
                event.reply(ret);
            }
            
            else if (subcommand.name == "quit") {
                abgame_started = false;
                event.reply("Game quitted.");
            }
        }

        // TODO List
        else if (interaction.get_command_name() == "todo") {
            auto subcommand = interaction.get_command_interaction().options[0];
            if (subcommand.name == "add") {
                dpp::interaction_modal_response modal("todo", "Enter your todo!");
                modal.add_component(
                    dpp::component().
                    set_label("DATE(IN FORMS OF YYYYMMDD)").
                    set_id("date").
                    set_type(dpp::cot_text).
                    set_placeholder("YYYYMMDD").
                    set_min_length(8).
                    set_max_length(8).
                    set_text_style(dpp::text_short)
                );
                modal.add_row();
                modal.add_component(
                    dpp::component().
                    set_label("TODO").
                    set_id("todo").
                    set_type(dpp::cot_text).
                    set_placeholder("enter your todo here").
                    set_min_length(1).
                    set_max_length(100).
                    set_text_style(dpp::text_short)
                );
                event.dialog(modal);
            }
            
            else if (subcommand.name == "remove") {
                string id = get<string>(event.get_parameter("id")), ret;
                try {
                    if (fs::remove("todolist/" + id + ".txt")) ret = "todo deleted ( •̀ ω •́ )✧";
                    else ret = "todo does not exist (⊙ˍ⊙)";
                }catch(...){
                    ret = "todo deletion failed （；´д｀）ゞ";
                }
                event.reply(ret);
            }
            
            else if (subcommand.name == "remove_all") {
                /* remove all todos and count (id.txt excluded)*/
                int file_count = -1;
                for (const auto& entry: fs::directory_iterator("todolist/")){
                    fs::remove(entry.path());
                    file_count++;
                }

                /* reset todo id */
                fstream id_file("todolist/id.txt", ios::out);
                id_file << "0";
                id_file.close();
                todo_id = 0;

                string s_or_not = file_count>1?" todos ":" todo ";
                event.reply("Removed " + to_string(file_count) + s_or_not + "( ´▽` )ﾉ");
            }

            else if (subcommand.name == "ls") {
                string ret = "```\n    id   date     todo\n";

                /* read all todos and sort in chronological order */
                vector<ToDo> todos;
                string file_name;
                fstream todo_file;
                bool complete;
                string id, raw_date, todo;
                for (const auto& entry: fs::directory_iterator("todolist/")){
                    file_name = entry.path().string();
                    if(file_name == "todolist/id.txt") continue;
                    todo_file.open(file_name, ios::in);
                    todo_file >> complete >> id >> raw_date;
                    todo_file.ignore();
                    getline(todo_file, todo);
                    todo_file.close();
                    ToDo todo_obj(complete, id, raw_date, todo);
                    todos.push_back(todo_obj);
                }
                std::sort(todos.begin(), todos.end(), todo_cmp);

                /* add them to ret */
                for(auto& todo: todos){
                    ret += todo.complete?"\u2611  ":"\u2610  ";
                    if(todo.id.length() < 2) ret += " ";
                    ret += todo.id + "  " + todo.raw_date + "  " + todo.todo + "\n";
                }
                ret += "```";
                event.reply(ret);
            }

            else if (subcommand.name == "complete") {
                string id = get<string>(event.get_parameter("id"));
                fstream todo_file_in("todolist/" + id + ".txt", ios::in);
                string writeBuffer, lineBuffer;
                getline(todo_file_in, lineBuffer);
                while(getline(todo_file_in, lineBuffer)){
                    writeBuffer += lineBuffer + '\n';
                }
                todo_file_in.close();
                fstream todo_file_out("todolist/" + id + ".txt", ios::out);
                todo_file_out << "1\n" << writeBuffer;
                todo_file_out.close();
                event.reply("Marked `" + id + "` as complete ヾ(≧▽≦*)o");
            }

            else if (subcommand.name == "incomplete") {
                string id = get<string>(event.get_parameter("id"));
                fstream todo_file_in("todolist/" + id + ".txt", ios::in);
                string writeBuffer, lineBuffer;
                getline(todo_file_in, lineBuffer);
                while(getline(todo_file_in, lineBuffer)){
                    writeBuffer += lineBuffer + '\n';
                }
                todo_file_in.close();
                fstream todo_file_out("todolist/" + id + ".txt", ios::out);
                todo_file_out << "0\n" << writeBuffer;
                todo_file_out.close();
                event.reply("Marked `" + id + "` as incomplete ~(>_<。)\\");
            }
        }
    });
 
    /* This event handles form submission for the modal dialog created above */
    bot.on_form_submit([&](const dpp::form_submit_t & event) {
        int components_count = event.components.size();
        if(components_count == 3){
            string date = get<string>(event.components[0].components[0].value);
            string title = get<string>(event.components[1].components[0].value);
            string diary_content = get<string>(event.components[2].components[0].value);

            /* save it */
            fstream diary_write("diaries/" + date + ".txt", ios::out);
            diary_write << title << '\n' << diary_content;
            diary_write.close();

             /* Emit a reply. Form submission is still an interaction and must generate some form of reply! */
            dpp::message m;
            m.set_content("Date: " + date + "\nTitle: " + title + "\nContent:\n" + diary_content).set_flags(dpp::m_ephemeral);
            event.reply(m);
        }else if(components_count == 2){
            string date = get<string>(event.components[0].components[0].value);
            string todo = get<string>(event.components[1].components[0].value);

            /* update todo_id and save it */
            todo_id++;
            fstream id_file("todolist/id.txt", ios::out);
            id_file << todo_id;
            id_file.close();
            fstream todo_add("todolist/" + to_string(todo_id) + ".txt", ios::out);
            todo_add << "0\n" << todo_id << '\n' << date << '\n' << todo << '\n';
            todo_add.close();

            dpp::message m;
            m.set_content("Date: " + date + "\nTODO: " + todo + "\nid:\n" + to_string(todo_id)).set_flags(dpp::m_ephemeral);
            event.reply(m);
        }
        
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));

            dpp::slashcommand greeting("greeting", "Say hello!", bot.me.id);
            greeting.add_option(dpp::command_option(dpp::co_string, "username", "Enter username here", true));
            bot.global_command_create(greeting);

            dpp::slashcommand add("add", "Add two given integers", bot.me.id);
            add.add_option(dpp::command_option(dpp::co_string, "number_1", "Enter an integer", true));
            add.add_option(dpp::command_option(dpp::co_string, "number_2", "Enter an integer", true));
            bot.global_command_create(add);

            dpp::slashcommand sub("sub", "Substract two given integers", bot.me.id);
            sub.add_option(dpp::command_option(dpp::co_string, "number_1", "Enter an integer", true));
            sub.add_option(dpp::command_option(dpp::co_string, "number_2", "Enter an integer", true));
            bot.global_command_create(sub);

            dpp::slashcommand mul("mul", "Multiply two given integers", bot.me.id);
            mul.add_option(dpp::command_option(dpp::co_string, "number_1", "Enter an integer", true));
            mul.add_option(dpp::command_option(dpp::co_string, "number_2", "Enter an integer", true));
            bot.global_command_create(mul);

            bot.global_command_create(dpp::slashcommand("reset", "Ramdomly generate an integer between 1 and 100", bot.me.id));
        
            dpp::slashcommand guess("guess", "Guess an integer between 1 and 100, reset on Bingo", bot.me.id);
            guess.add_option(dpp::command_option(dpp::co_string, "number_guess", "Guess an integer between 1 and 100", true));
            bot.global_command_create(guess);

            bot.global_command_create(dpp::slashcommand("write", "Write a new diary", bot.me.id));

            dpp::slashcommand read("read", "Read the diary of a specific date", bot.me.id);
            read.add_option(dpp::command_option(dpp::co_string, "date", "Please enter a date (YYYYMMDD)", true));
            bot.global_command_create(read);
        
            dpp::slashcommand remove("remove", "Remove the diary of a specific date", bot.me.id);
            remove.add_option(dpp::command_option(dpp::co_string, "date", "Please enter a date (YYYYMMDD)", true));
            bot.global_command_create(remove);

            /* custom things */
            bot.global_command_create(dpp::slashcommand("calculator", "List commands of calculator", bot.me.id));

            bot.global_command_create(dpp::slashcommand("diary", "List command of diary", bot.me.id));

            // 1A2B
            dpp::slashcommand abgame("1a2b", "1A2B game", bot.me.id);
            abgame.add_option(dpp::command_option(dpp::co_sub_command, "start", "Start a new game"));
            abgame.add_option(
                dpp::command_option(dpp::co_sub_command, "guess", "Guess a 4-digit number without any digit repeated").
                add_option(dpp::command_option(dpp::co_string, "number", "Guess a 4-digit number", true))
            );
            abgame.add_option(dpp::command_option(dpp::co_sub_command, "quit", "Quit the current game"));
            bot.global_command_create(abgame);

            // TODO List
            dpp::slashcommand todo("todo", "TODO List", bot.me.id);
            todo.add_option(dpp::command_option(dpp::co_sub_command, "add", "Add a new todo"));
            todo.add_option(
                dpp::command_option(dpp::co_sub_command, "remove", "Remove a todo").
                add_option(dpp::command_option(dpp::co_string, "id", "Enter the id of it", true))
            );
            todo.add_option(dpp::command_option(dpp::co_sub_command, "remove_all", "Remove the whole list"));
            todo.add_option(dpp::command_option(dpp::co_sub_command, "ls", "Show the list in chronological order"));
            todo.add_option(
                dpp::command_option(dpp::co_sub_command, "complete", "Mark a todo as complete").
                add_option(dpp::command_option(dpp::co_string, "id", "Enter the id of it", true))
            );
            todo.add_option(
                dpp::command_option(dpp::co_sub_command, "incomplete", "Mark a todo as incomplete").
                add_option(dpp::command_option(dpp::co_string, "id", "Enter the id of it", true))
            );
            bot.global_command_create(todo);
        }
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}
