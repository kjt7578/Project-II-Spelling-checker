#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef BUFLENGTH
#define BUFLENGTH 30
#endif
/*if BUFLENGTH is low(16,20,4)? or might certain line), printing last line cause error: idk the reason*/

void read_lines(int fd, void(*use_line)(void *, char *), void *arg){
        //  filename  , print_line에 들어갈 변수        ,  n 라인카운트
  int buflength = BUFLENGTH;
  char *buf = malloc(BUFLENGTH);

  int pos = 0;    // position of arr
  int bytes;      // bytes that written
  int line_start; // location of line starts

  while((bytes = read(fd, buf + pos, buflength - pos)) > 0){  // read return num of wrotten bytes
    if(DEBUG) printf("read %d bytes; pos = %d\n", bytes, pos);
    line_start = 0;
    int bufend = pos + bytes;

    while(pos < bufend){  // when line changed while buffer is not full
      if(DEBUG) printf("start %d, pos %d, char '%c'\n",line_start, pos, buf[pos]);
      if(buf[pos] == '\n' || buf[pos] == ' ' ){    // when found line change or space
        if ( buf[pos] == '\n')                     // replace to \0 (NULL)
          buf[pos] = '\0';      
        else if ( buf[pos] == ' ')
          buf[pos] = '\0';
        use_line(arg, buf + line_start);           // print that line
        line_start = pos + 1;                      // set linestart as the beginning of the line
      }
      pos++;
    }

    if (line_start == pos){  // no partial line
      pos = 0;
    }
    else if (line_start > 0){ // partial line. fill the buffer with partial words
      int segment_length = pos - line_start;
      memmove(buf, buf + line_start, segment_length); 
      // move memory (length of segment_length) which is starting buf + line_start to buf
      pos = segment_length;
      if (DEBUG) printf("move %d bytes to buffer start\n", segment_length);
    }
    else if(bufend == buflength){    // when partial line fills buffer
      buflength *= 2;              
      buf = realloc(buf, buflength); // increase buffer size
      if (DEBUG) printf("increase buffer to %d bytes\n", buflength);
    }
  } // 25 while end
  if (pos > 0){          //when the last line is after eof?
    if(pos == buflength){
      buf = realloc(buf, buflength + 1);
    }
    buf[pos] = '\0';
    use_line(arg, buf + line_start);
  }
  free(buf);
}

//*****************************************************************
void print_line(void *st, char *line){
  int *p = st;  //set pointer pointing n (line count)
  printf("%d: %s\n", *p, line);
  (*p)++;  //pointing next line
}

//*****************************************************************
int main(int argc, char **argv){
  char *fname = argc > 1 ? argv[1] : "test.txt";
  // if we specified any txt file, open that file (argv[1]), if not, test.txt
  int fd = open(fname, O_RDONLY);
  if(fd < 0){
    perror(fname);
    exit(EXIT_FAILURE);
  }
  int n = 0; //line counts
  read_lines(fd,print_line,&n);

    return EXIT_SUCCESS;
}