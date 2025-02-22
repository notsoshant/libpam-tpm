#include <stdlib.h>

#include "tpm.h"

char *path = "nv/Owner/libpam-tpm";
char *pin;

/********/
/* Core */
/********/

static TSS2_RC auth_callback(
    const char *objectPath,
    const char *description,
    const char **auth,
    void *userData)
{
    UNUSED(description);
    UNUSED(userData);

    if (!objectPath)
    {
        return TSS2_FAPI_RC_BAD_VALUE;
    }

    *auth = pin;
    return TSS2_RC_SUCCESS;
}

TSS2_RC init(FAPI_CONTEXT **ctx)
{
    TSS2_RC r;

    /* Initialize FAPI and set authorization callback, if successful */

    r = Fapi_Initialize(ctx, NULL);
    if (r == TSS2_RC_SUCCESS)
    {
        r = Fapi_SetAuthCB(*ctx, auth_callback, NULL);
    }

    return r;
}

void finalize(FAPI_CONTEXT **ctx)
{
    Fapi_Finalize(ctx);
}

TSS2_RC create_pin(FAPI_CONTEXT *ctx)
{
    TSS2_RC r;
    char *msg = "Success";

    /* Create NV and write PIN in it, if successful */

    r = Fapi_CreateNv(ctx, path, "noda", 10, "", pin);
    if (r == TSS2_RC_SUCCESS)
    {
        Fapi_NvWrite(ctx, path, msg, strlen(msg));
    }

    return r;
}

TSS2_RC check_pin(FAPI_CONTEXT *ctx, char *pin)
{
    TSS2_RC r;
    uint8_t *tmp;
    size_t tmp_len;

    /* Try to read the NV with provided PIN */

    r = Fapi_NvRead(ctx, path, &tmp, &tmp_len, NULL);

    return r;
}

int validate_input(char *pin)
{
    return 0;
}

/***********/
/* Exports */
/***********/

ENROLL_RESULT enroll_user()
{
    FAPI_CONTEXT *ctx;
    TSS2_RC r;
    char *msg = "Success";
    char input[10];
    int valid;

    /* Ask user for PIN */

    do
    {
        printf("Enter PIN: ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = 0;

        valid = validate_input(input);
    } while (valid);

    pin = input;

    /* Get ctx */

    r = init(&ctx);
    if (r != TSS2_RC_SUCCESS)
    {
        fprintf(stderr, "Error initializing FAPI. Make sure all dependencies are installed and FAPI is provisioned.\n");
        finalize(&ctx);
        return ENROLL_FAILED;
    }

    /* Register PIN for the user */

    r = create_pin(ctx);
    if (r != TSS2_RC_SUCCESS)
    {
        fprintf(stderr, "Error in creating PIN. Make sure the PIN is not already set. Use -d to remove existing PIN first.\n");
        finalize(&ctx);
        return ENROLL_FAILED;
    }

    /* Finalize */

    finalize(&ctx);
    printf("Successfully enrolled user!\n");

    return ENROLL_SUCCESS;
}

AUTH_RESULT auth_user(char *input)
{
    FAPI_CONTEXT *ctx;
    TSS2_RC r;

    pin = input;

    /* Get ctx */

    r = init(&ctx);
    if (r != TSS2_RC_SUCCESS)
    {
        fprintf(stderr, "Error initializing FAPI. Make sure all dependencies are installed and FAPI is provisioned.\n");
        finalize(&ctx);
        return AUTH_FAILED;
    }

    /* Validate PIN */

    r = check_pin(ctx, input);

    /* Finalize */

    finalize(&ctx);

    if (r == TSS2_RC_SUCCESS)
    {
        return AUTH_SUCCESS;
    }
    else
    {
        return AUTH_FAILED;
    }
}

int main(int argc, char *argv[])
{
    setenv("TSS2_LOG", "all+NONE", 1);

    enroll_user();

    return 0;
}