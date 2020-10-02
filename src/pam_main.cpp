#include <security/pam_ext.h>
#include <security/pam_modules.h>

#include <csignal>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "syslog.h"

void sigabrt(int signum) {
    std::cout << "Abort!!!" << std::endl;
    std::cout << "signum: " << signum << std::endl;

    exit(PAM_AUTH_ERR);
}

int auth(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *username;
    const void *vp_service;
    pam_get_user(pamh, &username, nullptr);
    pam_get_item(pamh, PAM_SERVICE, &vp_service);
    const char *service = (char *)vp_service;

    std::string command = "linux-hello --compare " + std::string(username);
    int status;

    if (!strcmp(service, "sudo")) {
        status = system(command.c_str());
    } else {
        status = system((command + std::string("> /dev/null")).c_str());
    }

    // system() call returns different value than expected so call WEXITSTATUS()
    return WEXITSTATUS(status);
}

int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int status = auth(pamh, flags, argc, argv);

    return status;
}

int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int status = auth(pamh, flags, argc, argv);

    return status;
}

int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) { return PAM_SUCCESS; }

int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) { return PAM_SUCCESS; }
