#ifndef BUILTIN_COMMANDS_H
#define BUILTIN_COMMANDS_H

#define DEFCMD(x) \
    void builtin_command_##x(char[MAX_TOKENS][MAX_TOKEN_LENGTH], int);

DEFCMD(echo)
DEFCMD(clear)
DEFCMD(off)

#endif  // BUILTIN_COMMANDS_H