#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include <string>
#include "stamp.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

// helper function for internal use only
// transforms raw binary hash value into human-friendly hexademical form
void convert_hash(const unsigned char *str, char *output, int hash_length) {
  char append[16];
  strcpy (output, "");
  for (int n=0; n<hash_length; n++) {
    sprintf(append,"%02x",str[n]);
    strcat(output, append);
  }
}

// pre-supplied helper function
// generates the SHA1 hash of input string text into output parameter digest
// ********************** IMPORTANT **************************
// ---> remember to include -lcrypto in your linking step <---
// ---> so that the definition of the function SHA1 is    <---
// ---> included in your program                          <---
// ***********************************************************
void text_to_SHA1_digest(const char *text, char *digest) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1( (const unsigned char *) text, strlen(text), hash);
  convert_hash(hash, digest, SHA_DIGEST_LENGTH);
}

/* add your function definitions here */

int leading_zeros(const char *digest) {
  int count = 0;
  while (count < (int) strlen(digest) && digest[count] == '0')
    ++count;

  return count;
}

bool file_to_SHA1_digest(const char *filename, char *digest) {
  ifstream in;
  in.open(filename);

  if (in.fail()) {
    strcpy(digest, "error");
    return false;
  }

  char message[1000000] = "";
  char buffer[512];
  while (in.getline(buffer, 512))
    strcat(message, buffer);
  
  in.close();
  text_to_SHA1_digest(message, digest);
  return true;
}

bool make_header(const char *recipient, const char *filename, char *header) {
  char digest[41];
  if (!file_to_SHA1_digest(filename, digest))
    return false;

  strcpy(header, recipient);
  strcat(header, ":");
  strcat(header, digest);
  strcat(header, ":");
  int counter_index = strlen(header);

  char hash[41];
  strcpy(hash, "");
  int counter = 0;

  while (leading_zeros(hash) != 5) {
    if (counter > 10000000)
      return false;

    strcpy(header+counter_index, to_string(counter).c_str());
    text_to_SHA1_digest(header, hash);  
    ++counter;
  }

  return true;
}

MessageStatus check_header(const char *email, const char *header, const char *filename) {
  char recipient[512];
  char msg_digest[41];
  char counter[8];
  int header_index = 0;

  set_field(header, recipient, header_index);
  set_field(header, msg_digest, header_index);
  set_field(header, counter, header_index);

  if (!strcmp(recipient, "") || !strcmp(msg_digest, "") || !strcmp(counter, ""))
    return INVALID_HEADER;

  if (strcmp(recipient, email))
    return WRONG_RECIPIENT;

  char msg_digest_check[41];
  file_to_SHA1_digest(filename, msg_digest_check);
  if (strcmp(msg_digest, msg_digest_check))
    return INVALID_MESSAGE_DIGEST;

  char header_digest_check[41];
  text_to_SHA1_digest(header, header_digest_check);
  if (leading_zeros(header_digest_check) != 5)
    return INVALID_HEADER_DIGEST;

  return VALID_EMAIL;
}

void set_field(const char *header, char *field, int &counter) {
  strcpy(field, "");
  int i = 0;

  while (header[counter] != ':' && header[counter] != '\0') {
    field[i] = header[counter];
    ++i;
    ++counter;
  }

  ++counter;
}