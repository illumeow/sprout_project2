#include <templatebot/templatebot.h>
#include <config.h>
#include <sstream>

/* When you invite the bot, be sure to invite it with the
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */


int main(int argc, char const *argv[])
{
      /* Setup the bot */
    dpp::cluster bot(TOKEN);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        dpp::interaction interaction = event.command;
        dpp::command_interaction cmd_data = interaction.get_command_interaction();

        if (interaction.get_command_name() == "ping") {
            event.reply("Pong!");
        }

        else if (interaction.get_command_name() == "image") {
            /* Get the sub command */
            auto subcommand = cmd_data.options[0];
            /* Check if the subcommand is "dog" */
            if (subcommand.name == "dog") { 
                /* Checks if the subcommand has any options. */
                if (!subcommand.options.empty()) {
                    /* Get the user from the parameter */
                    dpp::user user = interaction.get_resolved_user(
                        subcommand.get_value<dpp::snowflake>(0)
                    );
                    event.reply(user.get_mention() + " has now been turned into a dog."); 
                } else {
                    /* Reply if there were no options.. */
                    event.reply("No user specified");
                }
            }
            /* Check if the subcommand is "cat" */
            if (subcommand.name == "cat") {
                /* Checks if the subcommand has any options. */
                if (!subcommand.options.empty()) {
                    /* Get the user from the parameter */
                    dpp::user user = interaction.get_resolved_user(
                        subcommand.get_value<dpp::snowflake>(0)
                    );
                    event.reply(user.get_mention() + " has now been turned into a cat."); 
                } else {
                    /* Reply if there were no options.. */
                    event.reply("No user specified");
                }
            }
        }
    });

    /* Register slash command here in on_ready */
    bot.on_ready([&bot](const dpp::ready_t& event) {
        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));

            dpp::slashcommand image("image", "Send a specific image.", bot.me.id);
            image.add_option(
                /* Create a subcommand type option for "dog". */
                dpp::command_option(dpp::co_sub_command, "dog", "Send a picture of a dog.").
                add_option(dpp::command_option(dpp::co_user, "user", "User to turn into a dog.", false))
            );
            image.add_option(
                /* Create another subcommand type option for "cat". */
                dpp::command_option(dpp::co_sub_command, "cat", "Send a picture of a cat.").
                add_option(dpp::command_option(dpp::co_user, "user", "User to turn into a cat.", false))
            );
            /* Create command */
            bot.global_command_create(image);
        }
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}