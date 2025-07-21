#include "compiler.h"
#include "common.h"
#include "scanner.h"

void compile(const char *source) {
  initScanner(source);
  int line = -1;
  for (;;) {
    Token token = scanToken();
    if (token.line != line) {
      printf("%d: ", token.line);
      line = token.line;
    }
    if (token.type == TOKEN_EOF) {
      break;
    }
    printf("%s ", token.start);
  }
}
