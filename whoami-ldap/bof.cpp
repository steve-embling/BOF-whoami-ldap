#include <Windows.h>
#include <Winldap.h>
#pragma comment(lib, "wldap32.lib")
#include "base\helpers.h"

const size_t newsize = 100;

/**
 * For the debug build we want:
 *   a) Include the mock-up layer
 *   b) Undefine DECLSPEC_IMPORT since the mocked Beacon API
 *      is linked against the the debug build.
 */
#ifdef _DEBUG
#include "base\mock.h"
#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT
#endif

extern "C" {
#include "bofdefs.h"
#include "beacon.h"


    void go(char* args, int len) {
        /**
         * Define the Dynamic Function Resolution declaration for the GetSystemDirectoryA function
         * This time we use the DFR_LOCAL macro which create a local function pointer variable that
         * points to GetSystemDirectoryA. Therefore, we do have to map GetSystemDirectoryA to
         * KERNEL32$GetSystemDirectoryA
         
        DFR_LOCAL(KERNEL32, GetSystemDirectoryA);
        char path[MAX_PATH + 1];

        UINT bytesCopied = GetSystemDirectoryA(path, sizeof(path));
        if (bytesCopied == 0) {
            BeaconPrintf(CALLBACK_ERROR, "Error: %i", GetLastError());
        }
        else if (bytesCopied <= sizeof(path)) {
            BeaconPrintf(CALLBACK_OUTPUT, "System Directory: %s", path);
        }*/
        PCHAR hostName = NULL;
        LDAP* pLdapConnection = NULL;
        ULONG version = LDAP_VERSION3;
        ULONG getOptSuccess = 0;
        ULONG connectSuccess = 0;
        INT returnCode = 0;
        ULONG msgId;

        //  Verify that the user passed a hostname.
        /*if (len > 1)
        {
            //  Convert argv[] to a wchar_t*
            size_t origsize = strlen(argv[1]) + 1;
            size_t convertedChars = 0;
            wchar_t wcstring[newsize];
            mbstowcs_s(&convertedChars, wcstring, origsize, argv[1], _TRUNCATE);
            wcscat_s(wcstring, L" (wchar_t *)");
            hostName = wcstring;
        }
        else
        {
            hostName = NULL;
        }*/

        //  Initialize a session. LDAP_PORT is the default port, 389.
        pLdapConnection = ldap_init(hostName, LDAP_PORT);

        if (pLdapConnection == NULL)
        {
            //  Set the HRESULT based on the Windows error code.
            char hr = HRESULT_FROM_WIN32(GetLastError());
            BeaconPrintf(CALLBACK_ERROR, "ldap_init failed with 0x%x.\n", hr);
            goto error_exit;
        }
        else
            BeaconPrintf(CALLBACK_OUTPUT, "ldap_init succeeded \n");

        //  Set the version to 3.0 (default is 2.0).
        returnCode = ldap_set_option(pLdapConnection,
            LDAP_OPT_PROTOCOL_VERSION,
            (void*)&version);
        if (returnCode == LDAP_SUCCESS)
            BeaconPrintf(CALLBACK_OUTPUT, "ldap_set_option succeeded - version set to 3\n");
        else
        {
            BeaconPrintf(CALLBACK_ERROR, "SetOption Error:%0X\n", returnCode);
            goto error_exit;
        }

        // Connect to the server.
        connectSuccess = ldap_connect(pLdapConnection, NULL);

        if (connectSuccess == LDAP_SUCCESS)
            BeaconPrintf(CALLBACK_OUTPUT, "ldap_connect succeeded \n");
        else
        {
            BeaconPrintf(CALLBACK_ERROR, "ldap_connect failed with 0x%x.\n", connectSuccess);
            goto error_exit;
        }

        //  Bind with current credentials (login credentials). Be
        //  aware that the password itself is never sent over the 
        //  network, and encryption is not used.
        BeaconPrintf(CALLBACK_OUTPUT, "Binding ...\n");

        returnCode = ldap_bind_s(pLdapConnection, NULL, NULL,
            LDAP_AUTH_NEGOTIATE);
        if (returnCode == LDAP_SUCCESS)
        {
            BeaconPrintf(CALLBACK_OUTPUT, "The bind was successful\n");
            // Your original string
            PSTR oid = PSTR("1.3.6.1.4.1.4203.1.11.3");

            // Calculate the size needed for the wide char buffer
            //int size_needed = MultiByteToWideChar(CP_UTF8, 0, &oid[0], (int)oid.size(), NULL, 0);

            // Allocate the wide char buffer
            //PWSTR oid_wide = new WCHAR[size_needed + 1]; // +1 for the null terminator

            // Perform the conversion
            //MultiByteToWideChar(CP_UTF8, 0, &oid[0], (int)oid.size(), oid_wide, size_needed);

            // Add the null terminator
            //oid_wide[size_needed] = L'\0';
            ULONG r = ldap_extended_operation(
                pLdapConnection,
                oid,
                NULL, // No data for this operation
                NULL, // No server controls
                NULL, // No client controls
                &msgId
            );

            if (r == LDAP_SUCCESS) {
                LDAPMessage* res;
                // Wait for the response
                ldap_result(pLdapConnection, msgId, LDAP_MSG_ALL, NULL, &res);
                // Parse the result
                PSTR resultOID;
                berval* resultData;
                ldap_parse_extended_result(
                    pLdapConnection,
                    res,
                    &resultOID,
                    &resultData,
                    0 // Do not free res
                );
                // ... Use resultOID and resultData ...
                BeaconPrintf(CALLBACK_OUTPUT, "\nResult Data: %.*s\n", resultData->bv_len, resultData->bv_val);
                ldap_memfree(resultOID);
                ber_bvfree(resultData);
                ldap_msgfree(res);
            }

        }
        else
            goto error_exit;

        //  Normal cleanup and exit.
        ldap_unbind(pLdapConnection);
        return;

        //  On error cleanup and exit.
    error_exit:
        ldap_unbind(pLdapConnection);
        return;


    }
}

// Define a main function for the bebug build
#if defined(_DEBUG) && !defined(_GTEST)

int main(int argc, char* argv[]) {
    // Run BOF's entrypoint
    // To pack arguments for the bof use e.g.: bof::runMocked<int, short, const char*>(go, 6502, 42, "foobar");
    bof::runMocked<>(go);
    return 0;
}

// Define unit tests
#elif defined(_GTEST)
#include <gtest\gtest.h>

TEST(BofTest, Test1) {
    std::vector<bof::output::OutputEntry> got =
        bof::runMocked<>(go);
    std::vector<bof::output::OutputEntry> expected = {
        {CALLBACK_OUTPUT, "System Directory: C:\\Windows\\system32"}
    };
    // It is possible to compare the OutputEntry vectors, like directly
    // ASSERT_EQ(expected, got);
    // However, in this case, we want to compare the output, ignoring the case.
    ASSERT_EQ(expected.size(), got.size());
    ASSERT_STRCASEEQ(expected[0].output.c_str(), got[0].output.c_str());
}
#endif