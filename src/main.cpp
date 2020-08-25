#include <iostream>
#include <csignal>
#include <security/pam_modules.h>
#include <nlohmann/json.hpp>
#include <boost/program_options.hpp>
#include <cereal/archives/binary.hpp>
#include <z3.h>

#include "face_recognition.h"
#include "User.h"

void sigabrt(int signum) {
	std::cout << "Abort!!!" << std::endl;
	std::cout << "signum: " << signum << std::endl;

	exit(PAM_AUTH_ERR);
}

int main(int argc, char *argv[]) {

	std::cout << std::fixed;

	signal(SIGABRT, sigabrt);

	std::string username;
	if (getenv("SUDO_USER") != nullptr) {
		username = std::string(getenv("SUDO_USER"));
	}

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
			("init", "Download required data files")
			("add", "Add a new face model for an user.")
			("clear", "Remove all face models for an user.")
			("config", "Open config file in text editor")
			("enable", "Enable Linux Hello")
			("disable", "Disable Linux Hello")
			("list", "List all saved face models for an user.")
			("remove", po::value<int>(), "Remove a specific model for an user.")
			("test", "Test the camera")
			("help", "Show this help message and exit.")
			("compare", po::value<std::string>(), "Backend compare");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	std::ifstream f_setting("/etc/linux-hello/config.json");
	nlohmann::json settings;
	f_setting >> settings;
	f_setting.close();

	auto euid = geteuid();

	if (vm.count("compare")) {
		std::string username;
		username = vm["compare"].as<std::string>();

		if (settings["disabled"]) {
			std::cout << "Linux Hello disabled" << std::endl;
			return PAM_AUTHINFO_UNAVAIL;
		}

		face_recognition faceRecognition(settings);
		int status = faceRecognition.compare(username);
		return status;
	} else if (vm.count("add")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}
		face_recognition faceRecognition(settings);
		faceRecognition.add(username);
	} else if (vm.count("clear")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}
		std::string model_file_name = "/etc/linux-hello/models/" + username + ".dat";
		std::remove(model_file_name.c_str());
	} else if (vm.count("config")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}
		std::string editor = settings["editor"].get<std::string>() + " /etc/linux-hello/config.json";
		int status = system(editor.c_str());
	} else if (vm.count("enable")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}

		settings["disabled"] = false;

		std::ofstream f_output("/etc/linux-hello/config.json");
		f_output << std::setw(4) << settings;
		f_output.flush();
		f_output.close();

		std::cout << "Linux Hello has been enabled" << std::endl;

	} else if (vm.count("disable")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}

		settings["disabled"] = true;

		std::ofstream f_output("/etc/linux-hello/config.json");
		f_output << std::setw(4) << settings;
		f_output.flush();
		f_output.close();

		std::cout << "Linux Hello has been disabled" << std::endl;

	} else if (vm.count("list")) {
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

			for (auto m: user.user_encodings) {
				long time = m.unix_time;
				std::cout << "\t" << m.id << "\t" << std::put_time(std::localtime(&time), "%c") << "\t"
						  << m.label << std::endl;
			}
			std::cout << std::endl;

		} else {
			std::cout << "No face model known for the user " << username << " please run:" << std::endl;
			std::cout << "\tsudo howdy --add" << std::endl;
			return 0;
		}

	} else if (vm.count("remove")) {
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

			int i = 0;
			int id = vm["remove"].as<int>();
			for (auto &m:user.user_encodings) {
				if (m.id == id) break;
				i++;
			}
			user.user_encodings.erase(user.user_encodings.begin() + i);

			{
				cereal::BinaryOutputArchive output(f_models);
				output(user);
			}

			f_models.flush();
			f_models.close();
		}
	} else if (vm.count("test")) {
		face_recognition faceRecognition(settings);
		faceRecognition.test();
	} else if (vm.count("help")) {
		std::cout << desc;
	} else if (vm.count("init")) {
		if (euid != 0) {
			std::cout << "Please run this command as root" << std::endl;
			return 1;
		}
		system("bash /etc/linux-hello/data/install.sh");
	} else {
		std::cout << desc;
	}

}