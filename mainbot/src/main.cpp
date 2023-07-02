#include <templatebot/templatebot.h>
#include <config.h>
#include <sstream>
#include <fstream>
#include <filesystem>
using std::string;
using std::get;
using std::to_string;
using std::fstream;
using std::ios;
using std::vector;
using std::filesystem::remove;

/* When you invite the bot, be sure to invite it with the
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064 */

const string bot_token = TOKEN;

int number_for_guess = 0;

int main(int argc, char const *argv[]){
    /* Setup the bot */
    dpp::cluster bot(bot_token);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        dpp::interaction interaction = event.command;

        if (interaction.get_command_name() == "ping") {
            event.reply("Pong!");
        }

        else if (interaction.get_command_name() == "greeting") {
            /* Fetch a parameter value from the command parameters */
            string name = get<string>(event.get_parameter("username"));
            /* Reply to the command. There is an overloaded version of this
            * call that accepts a dpp::message so you can send embeds.
            */
            event.reply(string("Hello ") + name);
        }

        else if (interaction.get_command_name() == "add") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Add] The result is ") + to_string(num1) + " + " + to_string(num2) + " = " +to_string(num1+num2));
        }

        else if (interaction.get_command_name() == "sub") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Sub] The result is ") + to_string(num1) + " - " + to_string(num2) + " = " +to_string(num1-num2));
        }

        else if (interaction.get_command_name() == "mul") {
            int num1 = stoi(get<string>(event.get_parameter("number_1")));
            int num2 = stoi(get<string>(event.get_parameter("number_2")));
            event.reply(string("[Mul] The result is ") + to_string(num1) + " * " + to_string(num2) + " = " +to_string(num1*num2));
        }

        else if (interaction.get_command_name() == "reset") {
            number_for_guess = (rand() % 100) + 1;
            // event.reply(to_string(number_for_guess));
        }

        else if (interaction.get_command_name() == "guess") {
            int num = stoi(get<string>(event.get_parameter("number_guess")));
            string ret;
            if (num == number_for_guess) {
                ret = "Bingo!";
                number_for_guess = (rand() % 100) + 1;
            }
            else if (num > number_for_guess) ret = "Guess a smaller number!";
            else ret = "Guess a larger number!";
            event.reply(ret);
        }

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
            /* Trigger the dialog box. All dialog boxes are ephemeral */
            event.dialog(modal);
        }

        else if (interaction.get_command_name() == "read") {
            string date = get<string>(event.get_parameter("date"));
            fstream file("diaries/" + date + ".txt", ios::in);
            if(file){
                string title, line, contents = "";
                getline(file, title);
                while(!file.eof()){
                    getline(file, line);
                    std::cout << "line: " << line << '\n';
                    contents += line;
                    contents += '\n';
                }
                std::cout << "contents: " << contents << '\n';
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

                file.close();

                dpp::message m;
                m.add_embed(embed);
                event.reply(m);
            }else{
                event.reply("Diary not found!!!!");
            }
        }

        else if (interaction.get_command_name() == "remove") {
            string date = get<string>(event.get_parameter("date")), ret;
            try {
                if (remove("diaries/" + date + ".txt")) ret = "Diary deleted successfully :)";
                else ret = "Diary deletion failed :(";
            }catch(const std::filesystem::filesystem_error& err){
                ret = "Diary deleted successfully :)";
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

        //1A2B
        else if (interaction.get_command_name() == "1a2b") {
            event.reply("You can use these commands for 1A2B:\n`/start_game` `/ab_guess` `/quit` `/scoreboard`");
        }
    });
 
    /* This event handles form submission for the modal dialog we create above */
    bot.on_form_submit([&](const dpp::form_submit_t & event) {
        /* For this simple example we know the first element of the first row ([0][0]) is value type string.
         * In the real world it may not be safe to make such assumptions! */
        string date = get<string>(event.components[0].components[0].value);
        string title = get<string>(event.components[1].components[0].value);
        string diary_content = get<string>(event.components[2].components[0].value);

        fstream file("diaries/" + date + ".txt", ios::out);
        file << title << '\n' << diary_content;
        file.close();

        dpp::message m;
        m.set_content("Date: " + date + "\nTitle: " + title + "\nContent:\n" + diary_content).set_flags(dpp::m_ephemeral);
        /* Emit a reply. Form submission is still an interaction and must generate some form of reply! */
        event.reply(m);
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
            bot.global_command_create(dpp::slashcommand("start_game", "Create a new game", bot.me.id));

            dpp::slashcommand ab_guess("ab_guess", "Guess a 4-digit number", bot.me.id);
            ab_guess.add_option(dpp::command_option(dpp::co_string, "number", "Guess a number without repeating digit", true));
            bot.global_command_create(ab_guess);

            bot.global_command_create(dpp::slashcommand("quit", "Quit the current game", bot.me.id));

            bot.global_command_create(dpp::slashcommand("scoreboard", "show the scoreboard", bot.me.id));

            bot.global_command_create(dpp::slashcommand("1a2b", "List command of 1A2B", bot.me.id));
        }
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}
