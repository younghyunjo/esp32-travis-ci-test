/* This is the CA certificate for the CA trust chain of
   Key in PEM format, as dumped via:

	using self signed server.crt as input

   this dont work for me:

   	openssl s_client -showcerts -connect localhost:8883 </dev/null

   The CA cert is the last cert in the chain output by the server.
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*
 1 s:/C=US/O=Let's Encrypt/CN=Let's Encrypt Authority X3
   i:/O=Digital Signature Trust Co./CN=DST Root CA X3
 */
const char *server_root_cert = "-----BEGIN CERTIFICATE-----\r\n"
"MIIDoDCCAogCCQCzzVQyLqdAvzANBgkqhkiG9w0BAQsFADCBkTELMAkGA1UEBhMC\r\n"
"S1IxDjAMBgNVBAgTBVNlb3VsMQ8wDQYDVQQHEwZKb25ncm8xEjAQBgNVBAoTCURh\r\n"
"bGl3b3JrczEMMAoGA1UECxMDSU9UMRkwFwYDVQQDExBjYS5kYWxpd29ya3MubmV0\r\n"
"MSQwIgYJKoZIhvcNAQkBFhVjb250YWN0QGRhbGl3b3Jrcy5uZXQwHhcNMTMwODMx\r\n"
"MTgwMTQ4WhcNNDEwMTE1MTgwMTQ4WjCBkTELMAkGA1UEBhMCS1IxDjAMBgNVBAgT\r\n"
"BVNlb3VsMQ8wDQYDVQQHEwZKb25ncm8xEjAQBgNVBAoTCURhbGl3b3JrczEMMAoG\r\n"
"A1UECxMDSU9UMRkwFwYDVQQDExBjYS5kYWxpd29ya3MubmV0MSQwIgYJKoZIhvcN\r\n"
"AQkBFhVjb250YWN0QGRhbGl3b3Jrcy5uZXQwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n"
"DwAwggEKAoIBAQDQFsofdqVk3/srSNCnyqQHmlKkfrtXdBJ1VenDT/HU564wGjyl\r\n"
"Ku++Wmh8/KzDkHdDlsqKA4TPhyI3baJauYHTSyVpRxQ1khCSbCdEG1KMD4YggYnw\r\n"
"JIanrktOHDgZrwthDRqTHy2d7TAJvHnVjXhLtU9yeQafO0fByAXUYVYZ9fFa5Wbc\r\n"
"KByd/TWD7/8lKa5R0WIpzpfwWwwmB/dAjp5defr2xUHJbdBWndSsTfbi3DxfP+Nl\r\n"
"4iAS3R1Zp3DjYyC4Fq3WpV7uQuo5Z7k08GY9v80nAID/P1sJKJlDCBLLweFf7fJl\r\n"
"F4jUxpbELtJMNXHwtRjMBOfuLMVD+HkJ0O31AgMBAAEwDQYJKoZIhvcNAQELBQAD\r\n"
"ggEBACqkd3D3rgIgTQ9RKoiDk4V44xxvD4f4wYwCkNxnnBON8AeoN4gVEHqtvrsk\r\n"
"4RH95wtuAde0+thcokYu7qaWu0AuPRRZ7kyMcoGLDm0Beatma6ZW/Kv4DoEHo2Zz\r\n"
"uTILyWZ+jProaNGjeLZztZ01KrOWuVM2SZbQaC6eK5jPFtTHbkf+lR9FHE4rMDbj\r\n"
"HbT1BmjuHrGabVOLOOLesz3v+kVQwV0Y/3/3uLWL5j4xaFeanTRJfdTlgGA2m9o8\r\n"
"B1v82hqLoYNidhAUPGfH+kuJifH9S5cx120RQXIUMVOb8xu0WJ1dfxiL4dkG0Rcl\r\n"
"VE8GlZykSVNxiFDmX5S0rIVSPso=\r\n"
"-----END CERTIFICATE-----\r\n";



