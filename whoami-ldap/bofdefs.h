//#include <windows.h>

WINBASEAPI int WINAPI USER32$MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
#define	MessageBoxA USER32$MessageBoxA

// Define the Dynamic Function Resolution declaration for the GetLastError function
DFR(KERNEL32, GetLastError);
// Map GetLastError to KERNEL32$GetLastError 
#define GetLastError KERNEL32$GetLastError 

//#define LDAP_UNICODE
DFR(WLDAP32, ldap_init);
#define ldap_init WLDAP32$ldap_init
DFR(WLDAP32, ldap_get_option);
#define ldap_get_option WLDAP32$ldap_get_option
DFR(WLDAP32, ldap_connect);
#define ldap_connect WLDAP32$ldap_connect
DFR(WLDAP32, ldap_bind_s);
#define ldap_bind_s WLDAP32$ldap_bind_s
DFR(WLDAP32, ldap_set_option);
#define ldap_set_option WLDAP32$ldap_set_option
DFR(WLDAP32, ldap_extended_operation);
#define ldap_extended_operation WLDAP32$ldap_extended_operation
DFR(WLDAP32, ldap_result);
#define ldap_result WLDAP32$ldap_result
DFR(WLDAP32, ldap_parse_extended_resultA);
#define ldap_parse_extended_resultA WLDAP32$ldap_parse_extended_resultA
DFR(WLDAP32, ldap_memfree);
#define ldap_memfree WLDAP32$ldap_memfree
DFR(WLDAP32, ber_bvfree);
#define ber_bvfree WLDAP32$ber_bvfree
DFR(WLDAP32, ldap_msgfree);
#define ldap_msgfree WLDAP32$ldap_msgfree
DFR(WLDAP32, ldap_unbind);
#define ldap_unbind WLDAP32$ldap_unbind


//WINLDAPAPI LDAP* __cdecl LDAPConnectW(LPCWSTR, ULONG);
//WINLDAPAPI LDAP * __cdecl LDAPOpenW(LPCWSTR, ULONG);