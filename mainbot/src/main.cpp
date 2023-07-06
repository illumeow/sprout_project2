#include <templatebot/templatebot.h>
#include <config.h>
#include <limits.h>

using std::cout;
using std::string;
using std::get;
using std::getline;
using std::to_string;
using std::fstream;
using std::ios;
using std::vector;
using std::time;
namespace fs = std::filesystem;


/* for task 3 */
int number_for_guess = 0;

/* date validation */
bool is_valid_date(const string& s) {
    int year = stoi(s.substr(0, 4));
    int month = stoi(s.substr(4, 2));
    int day = stoi(s.substr(6, 2));

    if (year > 0) {
        if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) &&
             day > 0 && day <= 31) return true;
        else if ((month == 4 || month == 6 || month == 9|| month == 11) 
                && day>0 && day<=30) return true;
        else if (month == 2) {
            if((year%400 == 0 || (year%100 != 0 && year%4 == 0)) &&
                day > 0 && day <= 29) return true;
            else if (day>0 && day<=28) return true;
            else return false;
        }
        else return false;
    }
    else return false;
}

/* 1A2B */
vector<int> abgame_answer;
bool abgame_started = false;
int guess_count;
dpp::snowflake user_playing(0);

struct PlayRecord{
    int best_play_int;
    string username, best_play, play_count;
    PlayRecord(string username, string best_play, string play_count):
    username(username), best_play(best_play), play_count(play_count){
        best_play_int = (best_play == "N/A")?INT_MAX:stoi(best_play);
    };
};

/* check if a string is a number */
bool is_number(const string& s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return (it == s.end());
}

/* check if a string is repeated */
bool is_repeated(const string& s) {
    int count[10] = {0};
    for (auto& c: s) {
        if (count[c - '0']) return true;
        count[c - '0']++;
    }
    return false;
}

/* TODO List */
bool todo_ls_used = false;
vector<string> encourage_words = {
    "Every noble work is at first impossible",
    "This is tough, but you're tougher",
    "Limit your 'always' and your 'nevers'",
    "It's never too late to be what you might have been",
    "People say nothing is impossible, I do nothing"
};

string get_random_words() {
    return encourage_words[rand() % encourage_words.size()];
}

vector<string> split(const string& s, const string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> ret;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        ret.push_back (token);
    }

    ret.push_back (s.substr (pos_start));
    return ret;
}

vector<int> id_parser(const string& id) {
    vector<int> ret;
    vector<string> numbers;

    /* 1,3,4,6 */
    if (id.find(',') != std::string::npos) {
        numbers = split(id, ",");
        for(auto& number: numbers)
            ret.push_back(stoi(number));
    }
    /* 3-5 */
    else if (id.find('-') != std::string::npos) {
        numbers = split(id, "-");
        int start = stoi(numbers[0]), end = stoi(numbers[1]);
        for(int i=start; i<=end; i++)
            ret.push_back(i);
    }
    /* single id */
    else {
        if(!is_number(id)) ret.push_back(-1);
        else ret.push_back(stoi(id));
    }

    return ret;
}

struct Date{
    int year, month, day;
    Date(){};
    Date(string date) {
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(4, 2));
        day = stoi(date.substr(6, 2));
    };
};

struct ToDo: public Date{
    bool complete;
    string unique_id, raw_date, todo;
    Date date;
    ToDo(bool complete, string unique_id, string raw_date, string todo): 
    complete(complete), unique_id(unique_id), raw_date(raw_date), todo(todo), date(Date(raw_date)){};
};

vector<ToDo> todos;

bool todo_cmp(const ToDo& a, const ToDo& b) {
    if (a.date.year != b.date.year)
        return a.date.year < b.date.year;
    if (a.date.month != b.date.month)
        return a.date.month < b.date.month;
    return a.date.day < b.date.day;
};

/* random jokes */
vector<string> jokes;
string latest_joke = "";

string get_random_joke() {
    return jokes[rand() % jokes.size()];
}

/* go wash dishes */
int dishes_count = 0;

int main(int argc, char const *argv[]) {
    /* Setup the bot */
    dpp::cluster bot(TOKEN);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Setup random seed */
    std::srand(time(0));

    /* todo id initialize */
    fstream todo_id_file("myfiles/todolist/unique_id.txt", ios::in);
    int todo_unique_id; todo_id_file >> todo_unique_id;
    todo_id_file.close();
    cout << "todo unique_id: " << todo_unique_id << '\n';

    /* jokes initialize */
    fstream joke_file("myfiles/joke.txt", ios::in);
    string line;
    while (getline(joke_file, line)) jokes.push_back(line);
    joke_file.close();

    /* Handle slash command */
    bot.on_slashcommand([&](const dpp::slashcommand_t& event) {
        dpp::interaction interaction = event.command;
        string command_name = interaction.get_command_name();
        if (command_name == "ping") {
            event.reply("Pong!");
        }

        /* task 1.1 */
        else if (command_name == "greeting") {
            /* fetch a parameter value from the command parameters */
            string name = get<string>(event.get_parameter("username"));
            /* reply to the command */
            event.reply("Hello " + name);
        }

        /* task 2.1 */
        else if (command_name == "add") {
            int64_t num1 = get<int64_t>(event.get_parameter("number_1"));
            int64_t num2 = get<int64_t>(event.get_parameter("number_2"));
            event.reply("[Add] The result is " + to_string(num1) + " + " + to_string(num2) + " = " +to_string(num1+num2));
        }

        /* task 2.2 */
        else if (command_name == "sub") {
            int64_t num1 = get<int64_t>(event.get_parameter("number_1"));
            int64_t num2 = get<int64_t>(event.get_parameter("number_2"));
            event.reply("[Sub] The result is " + to_string(num1) + " - " + to_string(num2) + " = " +to_string(num1-num2));
        }

        /* task 2.3 */
        else if (command_name == "mul") {
            int64_t num1 = get<int64_t>(event.get_parameter("number_1"));
            int64_t num2 = get<int64_t>(event.get_parameter("number_2"));
            event.reply("[Mul] The result is " + to_string(num1) + " * " + to_string(num2) + " = " +to_string(num1*num2));
        }

        /* task 3.1 */
        else if (command_name == "reset") {
            /* set the answer in range [1, 100] */
            number_for_guess = rand() % 100 + 1;
            std::cout << "[Guess Number] New answer is: " << number_for_guess << '\n';
            event.reply("[Guess Number] Reset Successful!");
        }

        /* task 3.2 */
        else if (command_name == "guess") {
            int64_t num = get<int64_t>(event.get_parameter("number_guess"));
            string ret;
            if (number_for_guess == 0) {
                ret = "Please `/reset` first.";
            }
            else if (1 > num || num > 100) {
                ret = "Invalid guess.";
            }
            else if (num == number_for_guess) {
                ret = "Bingo!";
                /* reset the answer if Bingo */
                number_for_guess = rand() % 100 + 1;
                std::cout << "[Guess Number] New answer is: " << number_for_guess << '\n';
            }
            else if (num > number_for_guess) ret = "Guess a smaller number!";
            else ret = "Guess a larger number!";
            event.reply(ret);
        }

        /* task 4.1 */
        else if (command_name == "write") {
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
        else if (command_name == "read") {
            string date = get<string>(event.get_parameter("date"));
            fstream diary_read("myfiles/diaries/" + date + ".txt", ios::in);
            if (diary_read) {
                string title, line, contents = "";
                getline(diary_read, title);
                while (!diary_read.eof()) {
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
            }
            else{
                /* file not opened */
                event.reply("Diary not found!!!!");
            }
        }

        /* task 4.3 */
        else if (command_name == "remove") {
            string date = get<string>(event.get_parameter("date")), ret;
            try {
                /* use std::filesystem to remove */
                if (fs::remove("myfiles/diaries/" + date + ".txt")) ret = "Diary deleted successfully :)";
                else ret = "Diary deletion failed :(";
            }
            catch (...) {
                /* catch any */
                ret = "Diary deletion failed :(";
            }
            event.reply(ret);
        }
        
        /* custom things */
        else if (command_name == "calculator") {
            event.reply("You can use these commands for calculator:\n`/add` `/sub` `/mul`");
        }

        else if (command_name == "diary") {
            event.reply("You can use these commands for diary:\n`/write` `/read` `/remove`");
        }

        // 1A2B
        else if (command_name == "1a2b") {
            /* Get the sub command */
            auto subcommand = interaction.get_command_interaction().options[0];
            if (subcommand.name == "start") {
                if (user_playing.empty()) {
                    /* reset abgame_answer */
                    abgame_answer.clear();
                    for (int i=0; i<10; i++) abgame_answer.push_back(-1);
                    bool chose[10] = {};
                    cout << "1A2B answer: ";
                    int n;
                    for (int i=0; i<4; i++) {
                        do {
                            /* number in [0, 9] */
                            n = rand() % 10;
                        } while (chose[n]);
                        abgame_answer[n] = i;
                        chose[n] = true;
                        cout << n;
                    }
                    cout << '\n';

                    /* reset game settings */
                    abgame_started = true;
                    guess_count = 0;

                    /* get current playing user */
                    user_playing = interaction.get_issuing_user().id;
                    
                    event.reply("New game started!");
                }
                /* someone is playing */
                else {
                    event.reply("The game has been started.");
                }
            }
            
            else if (subcommand.name == "guess") {
                string ret, guess = get<string>(event.get_parameter("number"));
                dpp::snowflake user = interaction.get_issuing_user().id;
                if(user != user_playing && !user_playing.empty()) event.reply("You aren't the user who started this game.");
                else {
                    bool valid = true;
                    /* check if input is valid or is game started */
                    if ((guess.length() != 4) || (!is_number(guess)) || is_repeated(guess)) valid = false;
                    if (!abgame_started) ret = "Please start a new game first.";
                    else if (!valid) ret = "Not a valid guess.";
                    /* counting the A and B */
                    else{
                        int a = 0, b = 0, indx = 0;
                        for (auto& i: guess) {
                            int answer_index = abgame_answer[i - '0'];
                            if (answer_index == indx) a++;
                            else if (answer_index != -1) b++;
                            indx++;
                        }
                        if (a != 4) {
                            ret = guess + ": " + to_string(a) + "A" + to_string(b) + "B";
                            /* record the number of guesses */
                            guess_count++;
                        }
                        else{
                            ret = "Congrats! `" + guess + "` is the correct answer!\nYou used " + to_string(guess_count+1) + " guesses.\n";
                            
                            /* update record */
                            string user_id = to_string(interaction.get_issuing_user().id);
                            /* if registered */
                            if (fs::exists("myfiles/1A2B/" + user_id + ".txt")) {
                                fstream record;
                                string username, best_play, play_count, writeBuffer;
                                record.open("myfiles/1A2B/" + user_id + ".txt", ios::in);
                                record >> username >> best_play >> play_count;
                                record.close();
                                if (best_play == "N/A") {
                                    ret += "Best Play Updated!";
                                    writeBuffer +=  username + ' ' + to_string(guess_count+1) + " 1";
                                }
                                else {
                                    if(guess_count+1 < stoi(best_play)) {
                                        ret += "Best Play Updated!";
                                        writeBuffer +=  username + ' ' + to_string(guess_count+1) + ' ' + to_string(stoi(play_count)+1);
                                    }
                                    else {
                                        ret += "Good luck on next game!";
                                        writeBuffer +=  username + ' ' + best_play + ' ' + to_string(stoi(play_count)+1);
                                    }
                                }
                                record.open("myfiles/1A2B/" + user_id + ".txt", ios::out);
                                record << writeBuffer;
                                record.close();
                            }
                            /* anonymous user */
                            else {
                                ret += "This game isn't record because you haven't register yet.";
                            }

                            /* reset when Bingo */
                            abgame_started = false;
                            guess_count = 0;
                            user_playing = 0;
                        }
                    }
                    event.reply(ret);
                }
            }
            
            else if (subcommand.name == "quit") {
                dpp::snowflake user = interaction.get_issuing_user().id;
                if(user != user_playing && !user_playing.empty()) event.reply("You aren't the user who started this game.");
                else {
                    /* reset */
                    abgame_started = false;
                    guess_count = 0;
                    user_playing = 0;
                    event.reply("Game quitted.");
                }
            }

            else if (subcommand.name == "register") {
                string username = get<string>(event.get_parameter("username"));
                string user_id = to_string(interaction.get_issuing_user().id);
                fstream record;
                
                /* an update */
                if (fs::exists("myfiles/1A2B/" + user_id + ".txt")) {
                    string old_name, best_play, play_count;
                    record.open("myfiles/1A2B/" + user_id + ".txt", ios::in);
                    record >> old_name >> best_play >> play_count;
                    record.close();
                    record.open("myfiles/1A2B/" + user_id + ".txt", ios::out);
                    record << username << ' ' << best_play << ' ' << play_count;
                    record.close();
                    event.reply("Your username has been updated from `" + old_name + "` to `" + username + "`");
                }
                /* new register */
                else {
                    record.open("myfiles/1A2B/" + user_id + ".txt", ios::out);
                    record << username << ' ' << "N/A N/A\n";
                    record.close();
                    event.reply("Your username is registered as `" + username + "`");
                }
            }

            else if (subcommand.name == "scoreboard") {
                
                /* read all records */
                vector<PlayRecord> records;
                string file_name;
                fstream record_file;
                string username, best_play, play_count;
                for (const auto& entry: fs::directory_iterator("myfiles/1A2B/")) {
                    file_name = entry.path().string();
                    record_file.open(file_name, ios::in);
                    record_file >> username >> best_play >> play_count;
                    record_file.close();
                    PlayRecord record(username, best_play, play_count);
                    records.push_back(record);
                }

                /* sort by best_play */
                std::sort(records.begin(), records.end(),
                          [](const PlayRecord& a, const PlayRecord& b) {
                            return a.best_play_int < b.best_play_int;
                        });

                /* add them to embed */
                dpp::embed embed = dpp::embed().
                    set_color(0xFFA5CE).
                    set_title("1A2B Scoreboard");

                for(auto& record: records)
                    embed.add_field(record.username, "best play: " + record.best_play + "\nplay count: " + record.play_count);    

                dpp::message m;
                m.add_embed(embed);
                event.reply(m);
            }

            else if (subcommand.name == "delete") {
                string user_id = to_string(interaction.get_issuing_user().id), ret;
                try {
                    if (fs::remove("myfiles/1A2B/" + user_id + ".txt")) ret = "record deleted ╰(*°▽°*)╯";
                    else ret = "you have no play record （；´д｀）ゞ";
                }
                catch (...) {
                    ret = "record deletion failed.";
                }
                event.reply(ret);
            }
        }

        // TODO List
        else if (command_name == "todo") {
            auto subcommand = interaction.get_command_interaction().options[0];
            if (subcommand.name == "add") {
                /* use dialog box */
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
            
            else if (subcommand.name == "ls") {
                /* make remove, complete, incomplete valid */
                todo_ls_used = true;

                /* read all todos */
                todos.clear();
                string file_name;
                fstream todo_file;
                bool complete;
                string unique_id, raw_date, todo;
                int count = 0;
                for (const auto& entry: fs::directory_iterator("myfiles/todolist/")) {
                    file_name = entry.path().string();
                    if (file_name == "myfiles/todolist/unique_id.txt") continue;
                    todo_file.open(file_name, ios::in);
                    todo_file >> complete >> unique_id >> raw_date;
                    todo_file.ignore();
                    getline(todo_file, todo);
                    todo_file.close();
                    ToDo todo_obj(complete, unique_id, raw_date, todo);
                    todos.push_back(todo_obj);
                    count++;
                }

                dpp::embed embed = dpp::embed().
                    set_color(0xF4A0A0).
                    set_title("TODO List").
                    set_description(get_random_words());

                if (count) {
                    /* sort in chronological order */
                    std::sort(todos.begin(), todos.end(), todo_cmp);

                    /* add them to ret */
                    int id = 1;
                    for (auto& todo: todos) {
                        string field_name = "", field_value = "- ";
                        field_name += todo.complete?"\u2611   ":"\u2610   ";
                        field_name += to_string(id++) + ". " + todo.todo;
                        field_value += todo.raw_date;
                        embed.add_field(field_name, field_value);
                    }
                }
                else{
                    embed.add_field(
                        "No any todos! (/≧▽≦)/",
                        ""
                    );
                }

                dpp::message m;
                m.add_embed(embed);
                event.reply(m);
            }

            else if (subcommand.name == "remove") {
                if(!todo_ls_used) event.reply("Please list all todos first.");
                else {
                    string id = get<string>(event.get_parameter("id")), ret;
                    todo_ls_used = false;
                    /* all completes */
                    if (id == "c") {
                        for(auto& todo: todos){
                            if (todo.complete) {
                                try {
                                    fs::remove("myfiles/todolist/" + todo.unique_id + ".txt");
                                }
                                catch (...) {
                                    event.reply("Remove failed");
                                }
                            }
                        }
                        ret = "Removed all completes ( •̀ ω •́ )✧";
                    }
                    /* all incompletes */
                    else if (id == "i") {
                        for(auto& todo: todos){
                            if (!todo.complete) {
                                try {
                                    fs::remove("myfiles/todolist/" + todo.unique_id + ".txt");
                                }
                                catch (...) {
                                    event.reply("Remove failed");
                                }
                            }
                        }
                        ret = "Removed all incompletes ( •̀ ω •́ )✧";
                    }
                    /* specified id */
                    else {
                        vector<int> ids = id_parser(id);
                        if (ids[0] != -1) {
                            ret = "Removed ";
                            for(auto& i: ids){
                                try {
                                    fs::remove("myfiles/todolist/" + todos[i-1].unique_id + ".txt");
                                    ret += to_string(i) + ". ";
                                }
                                catch (...) {
                                    event.reply("Remove failed");
                                }
                            }
                            ret += "( •̀ ω •́ )✧";
                        }
                        else {
                            ret = "invalid id (´。＿。｀)";
                        }
                    }
                    event.reply(ret);
                }
            }
            
            else if (subcommand.name == "remove_all") {
                /* remove all todos and count (id.txt excluded)*/
                int file_count = -1;
                for (const auto& entry: fs::directory_iterator("myfiles/todolist/")) {
                    fs::remove(entry.path());
                    file_count++;
                }

                /* reset todo id */
                fstream id_file("myfiles/todolist/unique_id.txt", ios::out);
                id_file << "0";
                id_file.close();
                todo_unique_id = 0;

                if (file_count) {
                    string s_or_not = file_count>1?" todos ":" todo ";
                    event.reply("Removed " + to_string(file_count) + s_or_not + "( ´▽` )ﾉ");
                }
                else event.reply("There are no todos to be removed ( ´▽` )ﾉ");
            }

            else if (subcommand.name == "complete") {
                if(!todo_ls_used) event.reply("Please list all todos first.");
                else {
                    string id = get<string>(event.get_parameter("id")), ret;
                    todo_ls_used = false;
                    /* all incompletes */
                    if (id == "i") {
                        for(auto& todo: todos){
                            if (!todo.complete) {
                                /* use buffer to contain the data */
                                fstream todo_file_in("myfiles/todolist/" + todo.unique_id + ".txt", ios::in);
                                string writeBuffer, lineBuffer;
                                getline(todo_file_in, lineBuffer);
                                while (getline(todo_file_in, lineBuffer)) {
                                    writeBuffer += lineBuffer + '\n';
                                }
                                todo_file_in.close();

                                /* write the buffer and change [complete] at the same time */
                                fstream todo_file_out("myfiles/todolist/" + todo.unique_id + ".txt", ios::out);
                                todo_file_out << "1\n" << writeBuffer;
                                todo_file_out.close();
                            }
                        }
                        ret = "Marked all incomplete as complete ヾ(≧▽≦*)o";
                    }
                    /* specified id */
                    else {
                        vector<int> ids = id_parser(id);
                        if (ids[0] != -1) {
                            ret = "Marked ";
                            for(auto& i: ids){
                                fstream todo_file_in("myfiles/todolist/" + todos[i-1].unique_id + ".txt", ios::in);
                                string writeBuffer, lineBuffer;
                                getline(todo_file_in, lineBuffer);
                                while (getline(todo_file_in, lineBuffer)) {
                                    writeBuffer += lineBuffer + '\n';
                                }
                                todo_file_in.close();

                                /* write the buffer and change [complete] at the same time */
                                fstream todo_file_out("myfiles/todolist/" + todos[i-1].unique_id + ".txt", ios::out);
                                todo_file_out << "1\n" << writeBuffer;
                                todo_file_out.close();

                                ret += to_string(i) + ". ";
                            }
                            ret += "as complete ヾ(≧▽≦*)o";
                        }
                        else {
                            ret = "invalid id (´。＿。｀)";
                        }
                    }
                    event.reply(ret);
                }
            }

            else if (subcommand.name == "incomplete") {
                if(!todo_ls_used) event.reply("Please list all todos first.");
                else {
                    string id = get<string>(event.get_parameter("id")), ret;
                    todo_ls_used = false;
                    /* all completes */
                    if (id == "c") {
                        for(auto& todo: todos){
                            if (todo.complete) {
                                /* use buffer to contain the data */
                                fstream todo_file_in("myfiles/todolist/" + todo.unique_id + ".txt", ios::in);
                                string writeBuffer, lineBuffer;
                                getline(todo_file_in, lineBuffer);
                                while (getline(todo_file_in, lineBuffer)) {
                                    writeBuffer += lineBuffer + '\n';
                                }
                                todo_file_in.close();

                                /* write the buffer and change [complete] at the same time */
                                fstream todo_file_out("myfiles/todolist/" + todo.unique_id + ".txt", ios::out);
                                todo_file_out << "0\n" << writeBuffer;
                                todo_file_out.close();
                            }
                        }
                        ret = "Marked all complete as incomplete ~(>_<。)\\";
                    }
                    /* specified id */
                    else {
                        vector<int> ids = id_parser(id);
                        if (ids[0] != -1) {
                            ret = "Marked ";
                            for(auto& i: ids){
                                fstream todo_file_in("myfiles/todolist/" + todos[i-1].unique_id + ".txt", ios::in);
                                string writeBuffer, lineBuffer;
                                getline(todo_file_in, lineBuffer);
                                while (getline(todo_file_in, lineBuffer)) {
                                    writeBuffer += lineBuffer + '\n';
                                }
                                todo_file_in.close();

                                /* write the buffer and change [complete] at the same time */
                                fstream todo_file_out("myfiles/todolist/" + todos[i-1].unique_id + ".txt", ios::out);
                                todo_file_out << "0\n" << writeBuffer;
                                todo_file_out.close();

                                ret += to_string(i) + ". ";
                            }
                            ret += "s incomplete ~(>_<。)\\";
                        }
                        else {
                            ret = "invalid id (´。＿。｀)";
                        }
                    }
                    event.reply(ret);
                }
            }
        }

        else if (command_name == "joke") {
            auto subcommand = interaction.get_command_interaction().options[0];
            if (subcommand.name == "get") {
                string joke;
                do {
                    joke = get_random_joke();
                } while (joke == latest_joke);
                latest_joke = joke;
                event.reply(joke);
            }

            else if (subcommand.name == "add") {
                string joke = get<string>(event.get_parameter("joke"));
                fstream joke_file("myfiles/joke.txt", ios::app);
                joke_file << joke << '\n';
                joke_file.close();
                event.reply("Joke added ㄟ(≧◇≦)ㄏ");
            }

            else if (subcommand.name == "remove") {
                fstream joke_file;

                joke_file.open("myfiles/joke.txt", ios::in);
                string writeBuffer, lineBuffer;
                while (getline(joke_file, lineBuffer)) {
                    if (lineBuffer != latest_joke) writeBuffer += lineBuffer;
                }
                joke_file.close();

                joke_file.open("myfiles/joke.txt", ios::out);
                joke_file << writeBuffer << '\n';
                joke_file.close();

                event.reply("Removed joke: " + latest_joke);
            }

            else if (subcommand.name == "update") {
                fstream joke_file("myfiles/joke.txt", ios::in);
                string line;
                jokes.clear();
                while (getline(joke_file, line)) jokes.push_back(line);
                joke_file.close();
                event.reply("Jokes updated!");
            }
        }

        else if (command_name == "go_washing_dishes") {
            string ret;
            if (dishes_count == 0) ret = "好的，主人~~ ε٩(๑> ₃ <)۶з";
            else if (dishes_count == 1) ret = "好啦我去我去~ (๑¯∀¯๑)";
            else if (dishes_count == 2) ret = "你一直講煩不煩啊 (｡ŏ_ŏ)";
            else ret = "小心我宰了你喔 (╬ﾟдﾟ)▄︻┻┳═一";
            dishes_count++;
            if (dishes_count == 4) dishes_count = 0;
            event.reply(ret);
        }

        else if (command_name == "today_in_history") {
            string date = get<string>(event.get_parameter("date"));
            if(is_valid_date("2000" + date)) {
                fstream today_in_history("myfiles/today_in_history/" + date + ".txt", ios::in);
                if (today_in_history) {
                    string line;
                    getline(today_in_history, line);
                    today_in_history.close();

                    dpp::embed embed = dpp::embed().set_title(line);
                    
                    dpp::message m;
                    m.add_embed(embed);
                    event.reply(m);
                }
            }
			else {
                event.reply("Invalid date!");
            }
        }
    });
 
    /* This event handles form submission for the modal dialog created above */
    bot.on_form_submit([&](const dpp::form_submit_t & event) {
        /* use custom_id to ensure which command is used*/
        string form_id = event.custom_id;
        if (form_id == "diary") {
            string date = get<string>(event.components[0].components[0].value);
            string title = get<string>(event.components[1].components[0].value);
            string diary_content = get<string>(event.components[2].components[0].value);

            if(is_valid_date(date)) {
                 /* save it */
                fstream diary_write("myfiles/diaries/" + date + ".txt", ios::out);
                diary_write << title << '\n' << diary_content;
                diary_write.close();

                /* Emit a reply. Form submission is still an interaction and must generate some form of reply! */
                dpp::message m;
                m.set_content("Date: " + date + "\nTitle: " + title + "\nContent:\n" + diary_content).set_flags(dpp::m_ephemeral);
                event.reply(m);
            }
            else event.reply("[Diary] date is invalid");
        }
        else if (form_id == "todo") {
            string date = get<string>(event.components[0].components[0].value);
            string todo = get<string>(event.components[1].components[0].value);

            if(is_valid_date(date)) {
                /* update todo_id and save it */
                todo_unique_id++;
                fstream id_file("myfiles/todolist/unique_id.txt", ios::out);
                id_file << todo_unique_id;
                id_file.close();
                fstream todo_add("myfiles/todolist/" + to_string(todo_unique_id) + ".txt", ios::out);
                todo_add << "0\n" << todo_unique_id << '\n' << date << '\n' << todo << '\n';
                todo_add.close();

                dpp::message m;
                m.set_content("Date: " + date + "\nToDo: " + todo).set_flags(dpp::m_ephemeral);
                event.reply(m);
            }
            else event.reply("[TODO] date is invalid");
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
            add.add_option(dpp::command_option(dpp::co_integer, "number_1", "Enter an integer", true));
            add.add_option(dpp::command_option(dpp::co_integer, "number_2", "Enter an integer", true));
            bot.global_command_create(add);

            dpp::slashcommand sub("sub", "Substract two given integers", bot.me.id);
            sub.add_option(dpp::command_option(dpp::co_integer, "number_1", "Enter an integer", true));
            sub.add_option(dpp::command_option(dpp::co_integer, "number_2", "Enter an integer", true));
            bot.global_command_create(sub);

            dpp::slashcommand mul("mul", "Multiply two given integers", bot.me.id);
            mul.add_option(dpp::command_option(dpp::co_integer, "number_1", "Enter an integer", true));
            mul.add_option(dpp::command_option(dpp::co_integer, "number_2", "Enter an integer", true));
            bot.global_command_create(mul);

            bot.global_command_create(dpp::slashcommand("reset", "Ramdomly generate an integer between 1 and 100", bot.me.id));
        
            dpp::slashcommand guess("guess", "Guess an integer between 1 and 100, reset on Bingo", bot.me.id);
            guess.add_option(dpp::command_option(dpp::co_integer, "number_guess", "Guess an integer between 1 and 100", true));
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
            abgame.add_option(
                dpp::command_option(dpp::co_sub_command, "register", "Set your name").
                add_option(dpp::command_option(dpp::co_string, "username", "Enter any custom name you want!", true))
            );
            abgame.add_option(dpp::command_option(dpp::co_sub_command, "scoreboard", "List all players' play records"));
            abgame.add_option(dpp::command_option(dpp::co_sub_command, "delete", "Delete your play record"));
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

            // random jokes
            dpp::slashcommand joke("joke", "jokes!", bot.me.id);
            joke.add_option(dpp::command_option(dpp::co_sub_command, "get", "Get a random joke"));
            joke.add_option(
                dpp::command_option(dpp::co_sub_command, "add", "Add your own joke").
                add_option(dpp::command_option(dpp::co_string, "joke", "Enter your own joke", true))
            );
            joke.add_option(dpp::command_option(dpp::co_sub_command, "remove", "Remove the last joke"));
            joke.add_option(dpp::command_option(dpp::co_sub_command, "update", "Update the jokes"));
            bot.global_command_create(joke);

            // go wash dishes!
            bot.global_command_create(dpp::slashcommand("go_washing_dishes", "command your slave~", bot.me.id));

            // today in history
            dpp::slashcommand today_in_history("today_in_history", "today in history", bot.me.id);
            today_in_history.add_option(dpp::command_option(dpp::co_string, "date", "Please enter a date (MMDD)", true));
            bot.global_command_create(today_in_history);
        }
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}
