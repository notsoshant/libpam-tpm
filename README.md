# libpam-tpm

Proof of Concept for a Linux PAM that uses Trusted Platform Module (TPM) for PIN based authentication

> **Warning**: This code is only a proof of concept as of now. Do not use on live systems. It skips over many safeguards that ideally should be there.

## How it works

libpam-tpm uses NV storage provided by TPMs, and TSS-FAPI to interact with TPM. It basically has two flows as of now:

1. **Enrollment Flow**
2. **Authentication Flow**

### Enrollment Flow

Enrollment Flow is used to register PIN for the user. It is performed using `enroll_user()`. It first calls `Fapi_Initialize()` to initialize FAPI context and connect with TPM. Then, `Fapi_CreateNv()` is called to create an NV index with `authValue` set as the PIN entered by the user. Finally, some dummy data is written in the created NV index.

### Authentication Flow

Authentication Flow is what PAM uses to verify the PIN entered by user. It calls the `auth_user()`. Like Enrollment, it will initialize FAPI, and then call `Fapi_NvRead()` with the PIN provided by user. If the Read operation is successful, the authentication is approved.

## How to use

> **Note:** PAM would only work for user named `test`. This is intentional.

Step 1: Install required packages:

```sh
apt install libpam-dev libtss2-dev tpm2-tools
```

Step 2: Clone this repo

Step 3: Compile files

```sh
gcc -fPIC -c main.c
gcc -fPIC -c tpm.c
sudo ld -x --shared -ltss2-fapi -o /lib/x86_64-linux-gnu/security/pam_tpm.so main.o tpm.o
```

Step 4: Modify `/etc/pam.d/common-auth` to include our PAM:

```
auth	sufficient	pam_tpm.so
```

Step 5: Provision FAPI, if not done already using `tss2_provision`.

Step 6: Compile `tpm.c` and enroll user

```sh
gcc -o tpm-test tpm.c -ltss2-fapi
sudo ./tpm-test
```

Step 7: Run `su test` and check if the PIN authentication is working
