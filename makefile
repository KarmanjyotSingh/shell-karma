all:
	gcc -g -o shell fg.c replay.c bg.c main.c  cd.c echo.c execute.c helper.c history.c input.c ls.c pinfo.c prompt.c pwd.c signals.c system_command.c tokenize.c redirection.c pipe.c jobs.c