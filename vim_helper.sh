#!/bin/bash



ctags -R --c++-kinds=+p --fields=+iaS --extra=+q src/include
ctags -R --c++-kinds=+p --fields=+iaS --extra=+q /usr/include/ncurses/



