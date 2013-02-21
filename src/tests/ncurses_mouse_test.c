#include <ncurses.h>
#include <stdlib.h>

MEVENT *mev;

void quit(void)
{
  free(mev);
  endwin();
}

int main(void)
{
  int ch;
  mev = (MEVENT *)malloc(sizeof(MEVENT));

  initscr();
  atexit(quit);
  clear();
  noecho();
  curs_set(0);
  cbreak();
  keypad(stdscr, TRUE);
  start_color();
  mousemask(BUTTON1_CLICKED, 0);

  init_pair(1, COLOR_YELLOW, COLOR_BLUE);
  bkgd(COLOR_PAIR(1));
  mvaddstr(5, 3, "Programm durch anklicken der Maustaste 1 beenden");
  refresh();
 
  for(;;)
  {
    ch=getch();
        
    switch(ch)
    {
      case KEY_MOUSE:
      { 
        if(getmouse(mev) == OK)
        {
          exit(0);
        }
      } 
    }   
  }   

  return (0);  
}
