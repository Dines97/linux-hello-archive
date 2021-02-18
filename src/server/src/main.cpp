#include <security/pam_modules.h>

#include <cereal/archives/binary.hpp>
#include <csignal>
#include <iostream>

#include "../../../include/CLI11.hpp"
#include "../../../include/cpptoml.h"
#include "timings/timings.h"
#include "User.h"
#include "face_recognition.h"

void sigabrt(int signum) { exit(PAM_AUTH_ERR); }

int main(int argc, char *argv[]) {


    timings *timing = timings::instance();

    timing->add_record("Program start");


    std::cout << std::fixed;

    signal(SIGABRT, sigabrt);

    std::string username;
    if (getenv("SUDO_USER") != nullptr) {
        username = std::string(getenv("SUDO_USER"));
    }


    bool verbose;

    CLI::App app{"Linux Hello provides Windows Hello™ style authentication for Linux."};
    app.add_option("-u,--user", username, "Specifies the user name to use.")->type_name("<User name>");
    app.add_option("-v,--verbose", verbose, "Display verbose info");
    app.add_subcommand("init", "Download required data files.");
    app.add_subcommand("add", "Add a new face model for a user.");
    app.add_subcommand("clear", "Remove all face models for a user.");
    app.add_subcommand("config", "Open config file in text editor.");

    // TODO: Change enable disable command behaviour
    //app.add_subcommand("enable", "Enable Linux Hello.");
    //app.add_subcommand("disable", "Disable Linux Hello.");

    app.add_subcommand("list", "List all saved face models for a user.");
    app.add_subcommand("test", "Test the camera.");
    app.add_subcommand("compare", "Backend compare.")->group("");
    app.require_subcommand(1, 1);

    int id;
    std::string label;
    CLI::App *remove = app.add_subcommand("remove", "Remove a specific model for a user.");
    remove->add_option("-i,--id", id, "Model id.")->check(CLI::NonNegativeNumber)->type_name("<Model id>");
    remove->add_option("-l,--label", label, "Model label")->type_name("<Model label>");
    remove->require_option(1, 1);

    CLI11_PARSE(app, argc, argv);

    // TODO; Find or create better configuration library
    auto config = cpptoml::parse_file("/etc/linux-hello/config.toml");

    auto euid = geteuid();

    std::string sub_cmd = app.get_subcommands()[0]->get_name();

    if (sub_cmd == "compare") {
        if (config->get_as<bool>("disabled").value_or(true)) {
            std::cout << "Linux Hello disabled" << std::endl;
            return PAM_AUTHINFO_UNAVAIL;
        }

        face_recognition faceRecognition(config);
        int status = faceRecognition.compare(username);
        return status;
    } else if (sub_cmd == "add") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }
        face_recognition faceRecognition(config);
        faceRecognition.add(username);
    } else if (sub_cmd == "clear") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }
        std::string model_file_name = "/etc/linux-hello/models/" + username + ".dat";
        std::remove(model_file_name.c_str());
    } else if (sub_cmd == "config") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }
        std::string editor = config->get_as<std::string>("editor").value_or("nano") + " /etc/linux-hello/config.toml";
        int status = system(editor.c_str());
    } else if (sub_cmd == "enable") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }

        config->insert("disabled", false);

        std::ofstream f_output("/etc/linux-hello/config.toml");
        f_output << (*config);
        std::cout << (*config);
        f_output.flush();
        f_output.close();

        std::cout << "Linux Hello has been enabled" << std::endl;

    } else if (sub_cmd == "disable") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }

        config->insert("disabled", true);

        std::ofstream f_output("/etc/linux-hello/config.toml");
        f_output << std::setw(4) << config;
        f_output.flush();
        f_output.close();

        std::cout << "Linux Hello has been disabled" << std::endl;

    } else if (sub_cmd == "list") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }

        std::string models_file_name = "/etc/linux-hello/models/" + username + ".dat";
        std::ifstream f_models(models_file_name);
        User user;

        if (f_models.good()) {
            {
                cereal::BinaryInputArchive input(f_models);
                input(user);
            }

            if (user.user_encodings.empty()) {
                std::cout << "No face model known for the user " << username << " please run:" << std::endl;
                std::cout << "\tsudo howdy --add" << std::endl;
                return 0;
            }

            std::cout << "Known face models for " << username << ":" << std::endl << std::endl;
            std::cout << "\tID\tDate\t\t\t\tLabel" << std::endl;

            for (auto m : user.user_encodings) {
                long time = m.unix_time;
                std::cout << "\t" << m.id << "\t" << std::put_time(std::localtime(&time), "%c") << "\t" << m.label
                          << std::endl;
            }
            std::cout << std::endl;

        } else {
            std::cout << "No face model known for the user " << username << " please run:" << std::endl;
            std::cout << "\tsudo howdy --add" << std::endl;
            return 0;
        }

    } else if (sub_cmd == "remove") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }

        std::string models_file_name = "/etc/linux-hello/models/" + username + ".dat";
        std::fstream f_models;
        f_models.open(models_file_name, std::ios::in);
        User user;

        if (f_models.good()) {
            {
                cereal::BinaryInputArchive input(f_models);
                input(user);
            }

            f_models.close();
            f_models.clear();
            f_models.open(models_file_name, std::ios::out | std::ios::trunc);

            std::size_t i = 0;
            for (auto &m : user.user_encodings) {
                if (m.id == id || m.label == label) break;
                i++;
            }
            if (i == user.user_encodings.size()) {
                std::cout << "Could not find the model with the specified id or label." << std::endl;
                return 0;
            }
            user.user_encodings.erase(user.user_encodings.begin() + i);

            {
                cereal::BinaryOutputArchive output(f_models);
                output(user);
            }

            f_models.flush();
            f_models.close();
        }
    } else if (sub_cmd == "test") {
        timing->add_record("Face recognition init");
        face_recognition faceRecognition(config);
        timing->get_record("Face recognition init")->finish();
        faceRecognition.test();
    } else if (sub_cmd == "init") {
        if (euid != 0) {
            std::cout << "Please run this command as root" << std::endl;
            return 1;
        }
        system("bash /etc/linux-hello/data/install.sh");
    }

    timing->print();
}
