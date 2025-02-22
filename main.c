#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <security/pam_ext.h>
#include <security/pam_modules.h>

#include "tpm.h"

int authenticate(pam_handle_t *handle, int flags, int argc, const char **argv)
{
    setenv("TSS2_LOG", "all+NONE", 1);

    int pam_code;
    int count = 3;

    const char *username = NULL;
    char *password = NULL;
    const char *user = "test";

    /* Asking the application for an  username */
    pam_code = pam_get_user(handle, &username, "USERNAME: ");
    if (pam_code != PAM_SUCCESS)
    {
        fprintf(stderr, "Can't get username");
        return PAM_PERM_DENIED;
    }

    if (strcmp(username, user) != 0)
    {
        return PAM_USER_UNKNOWN;
    }

    while (count--)
    {
        /* Asking the application for a password */
        pam_code = pam_prompt(handle, PAM_PROMPT_ECHO_OFF, &password, "%s", "PIN: ");
        if (pam_code != PAM_SUCCESS)
        {
            fprintf(stderr, "Can't get password");
            return PAM_PERM_DENIED;
        }

        /* Checking the PAM_DISALLOW_NULL_AUTHTOK flag: if on, we can't accept empty passwords */
        if (flags & PAM_DISALLOW_NULL_AUTHTOK)
        {
            if (password == NULL || strcmp(password, "") == 0)
            {
                fprintf(stderr,
                        "Null authentication token is not allowed!.");
                return PAM_PERM_DENIED;
            }
        }

        if (auth_user(password) == AUTH_SUCCESS)
        {
            return PAM_SUCCESS;
        }

        fprintf(stderr, "Invalid PIN. Try again.\n");
    }

    fprintf(stderr, "Authentication failed: 3 invalid attempts\n");
    return PAM_AUTH_ERR;
}

/* PAM related functions */

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc,
                                   const char **argv)
{
    return authenticate(pamh, flags, argc, argv);
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc,
                                   const char **argv)
{
    return PAM_IGNORE;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc,
                                const char **argv)
{
    return PAM_IGNORE;
}
PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc,
                                    const char **argv)
{
    return PAM_IGNORE;
}
PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc,
                                const char **argv)
{
    return PAM_IGNORE;
}
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc,
                              const char **argv)
{
    return PAM_IGNORE;
}