#ifndef BUILTIN_COMMANDS_H
#define BUILTIN_COMMANDS_H

#define DEFCMD(x) \
    void builtin_command_##x(char[MAX_TOKENS][MAX_TOKEN_LENGTH], int);

DEFCMD(echo)
DEFCMD(clear)
DEFCMD(off)
DEFCMD(serialw)
DEFCMD(serialr)
DEFCMD(ls)
DEFCMD(cd)
DEFCMD(cat)
DEFCMD(touch)
DEFCMD(rm)
DEFCMD(fwrite)
DEFCMD(mkdir)
DEFCMD(help)
DEFCMD(fsinfo)
DEFCMD(basic)

#endif  // BUILTIN_COMMANDS_H
