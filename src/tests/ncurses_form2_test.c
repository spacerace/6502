#include <form.h>
#include <stdlib.h>
#include <string.h>

FIELD  **fi;
FORM   *fo;

void quit(void)
{
  int i; 

  unpost_form(fo);
  free_form(fo);

  for(i=0; i<=3; i++)
  {
    free_field(fi[i]);
  }

  free(fi);
  endwin();
}

int main(void)
{
  int ch, i;

  initscr();
  atexit(quit);
  clear();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  start_color();

  init_pair(1, COLOR_YELLOW, COLOR_BLUE);
  init_pair(2, COLOR_BLACK, COLOR_WHITE);

  bkgd(COLOR_PAIR(1));

  fi = (FIELD **)calloc(4, sizeof(FIELD *));
  fi[0] = new_field(1, 10, 2, 3, 0, 0);
  fi[1] = new_field(1, 10, 2, 18, 0, 0);
  fi[2] = new_field(1, 15, 2, 33, 0, 0);
  fi[3] = 0;

  for(i=0; i<3; i++)
  {
    set_field_fore(fi[i], COLOR_PAIR(2));
    set_field_back(fi[i], COLOR_PAIR(2));
  }

  fo = new_form(fi);
  post_form(fo);        

  mvaddstr(2, 15, "+");
  mvaddstr(2, 30, "=");
  mvaddstr(5, 3, "Programm mittels F1-Funktionstaste beenden");

  refresh();
 
  while((ch=getch()) != KEY_F(1))
  {
    switch(ch)
    {
      case KEY_RIGHT:
      { 
        char str[20];
                                                        
        form_driver(fo, REQ_NEXT_FIELD);

        if(field_index(current_field(fo)) == 2)
        {
          snprintf(str, 20, "%s%s", field_buffer(fi[0], 0), field_buffer(fi[1], 0));
          set_field_buffer(fi[2], 0, str);
          refresh();
        }
        break;
      }
      case KEY_LEFT:
        form_driver(fo, REQ_PREV_FIELD);
        break;
      default: /* Feldeingabe */        
        form_driver(fo, ch);
    }
  }   
  return (0);  
}
