
// chess.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <math.h>
#include <graphics.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

using namespace std;
#include "fuzzy.h"

#define MY_INFINITY 65530

#define NO_PIECE 0
#define WHITEPAWN 1
#define WHITEBISHOP 2
#define WHITEROOK 3
#define WHITEKNIGHT 4
#define WHITEKING 5
#define WHITEQUEEN 6

#define BLACKPAWN 7
#define BLACKBISHOP 8
#define BLACKROOK 9
#define BLACKKNIGHT 10
#define BLACKKING 11
#define BLACKQUEEN 12

#define WHITESTART 1
#define WHITEEND 6
#define BLACKSTART 7
#define BLACKEND 12

#define POINTS_PER_BLOCK 1

class pieceboard
{
    unsigned short int at[8][8];
    friend class modify_board;
};
class piece_pos
{
    unsigned short int x;
    unsigned short int y;
    friend class modify_board;
    friend class generic_move;
};
class move_status
{
    unsigned short int isvalid;
    unsigned short int kill;
public:
    move_status();
    friend class modify_board;
};
class generic_move
{
    piece_pos source,dest;
    int points,points2;
    bool ismate;
    friend class modify_board;
public:
    generic_move();
};
generic_move::generic_move()
{
    points = 0;
    points2 = 0;
    ismate = true;
}
move_status::move_status()
{
    isvalid = 0;
    //kill = 0;
}
class modify_board
{
    //static int count;
    int button,x,y;
public:
    
    pieceboard board,prev_board;
    int maximum_depth;
    short int moves;
	
	fuzzy f1;

    modify_board();
    void set_start_pos();
    void display();
    void graphics_display_init();
    void graphics_display();
    int get_difficulty();
    void display_piece(int,int,char*,int);

    void getmousepos();
    void getmouseclick();
    void initmouse();
    void showmouse();
    void mousehide();

    //positional advantage computation
	int positional_advantage_bishop(int,int);
	int positional_advantage_rook(int,int);
	int positional_advantage_knight(int,int);
	int positional_advantage_queen(int,int);
	
	bool king_check(short int);
    move_status validate_white(piece_pos,piece_pos);
    move_status validate_black(piece_pos,piece_pos);
    generic_move traverse_all(int,unsigned short int,int);
    unsigned short int make_move(piece_pos,piece_pos);
    void undo_move(piece_pos,piece_pos,unsigned short int);
	unsigned short int make_promotion_move(piece_pos,piece_pos);
    void undo_promotion_move(piece_pos,piece_pos,unsigned short int);
    int analyse_board();
    void next_move();
	void next_move_cpu();
	void init_fuzzy_model();
	
};
modify_board::modify_board()
{
    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
        {
            board.at[i][j] = 0;
        }

    moves = 0;
}
void modify_board::initmouse() 			//initialise mouse
{
	//i.x.ax=0;
	//int86(0x33,&i,&o);
}

void modify_board::showmouse()		//show mouse
{
       //i.x.ax=1;
       //int86(0x33,&i,&o);
}

void modify_board::getmousepos()   //get mouse
{
	//i.x.ax=3;
	//int86(0x33,&i,&o);
	//*button=o.x.bx;
	/*
	0:button not pressed
	1:left pressed
	2:right pressed
	3:center pressed
	*/
	//*x=o.x.cx;
	//*y=o.x.dx;
    
     
    
    if(ismouseclick(WM_LBUTTONDOWN))
      button = 1;
      
    
    /*else if(ismouseclick(WM_RBUTTONDOWN))
    {
      *button = 2;
      
    }
    
    else if(ismouseclick(WM_MBUTTONDOWN))
    {
      *button = 3;
      
    }*/
    
    else
      button = 0;
     
    //clearmouseclick(WM_RBUTTONDOWN) ;
    clearmouseclick(WM_LBUTTONDOWN) ;
    //clearmouseclick(WM_MBUTTONDOWN) ;
    
     x = mousex();
     y = mousey();
}



void modify_board::getmouseclick()
{
    do{
		getmousepos();
		//cout<<button<<"  "<<x<<" "<<y<<"\n";
		delay(1);
	    }while(button == 0);

	    if((x>=620) && (y <= 20))
        {
	        closegraph();
            exit(1);
        }

        do{
		getmousepos();
		//cout<<button<<"  "<<x<<" "<<y<<"\n";
        delay(1);
	  }while(button != 0);

}
void modify_board::mousehide()
{
	//i.x.ax=2;
	//int86(0x33,&i,&o);
}
void modify_board::set_start_pos()
{
    for(int i=0;i<8;i++)
    {
    for(int j=0;j<8;j++)
    {
        if(i==1)
			board.at[i][j] = WHITEPAWN;
		if(i==6)
		    board.at[i][j] = BLACKPAWN;
    }
	}
	
    board.at[0][0] = board.at[0][7] = WHITEROOK;
    board.at[7][0] = board.at[7][7] = BLACKROOK;

    board.at[0][1] = board.at[0][6] = WHITEKNIGHT;
    board.at[7][1] = board.at[7][6] = BLACKKNIGHT;

    board.at[0][2] = board.at[0][5] = WHITEBISHOP;
    board.at[7][2] = board.at[7][5] = BLACKBISHOP;

    board.at[0][3] = WHITEKING;
    board.at[7][3] = BLACKKING;

    board.at[0][4] = WHITEQUEEN;
    board.at[7][4] = BLACKQUEEN;
	
}
void modify_board::display()
{
    char disp[13] = {' ','P','B','R','N','Q','K','P','B','R','N','K','Q'}; 
    for(int i=0;i<8;i++)
    {
        cout<<i<<"  ";
        for(int j=0;j<8;j++)
        {
       
                cout<<disp[board.at[i][j]];
	}
        cout<<endl;
    }
    cout<<"\n   ";
    for(int i=0;i<8;i++)
        cout<<i;
}
int modify_board::get_difficulty()
{
    cleardevice();
    setcolor(DARKGRAY);
    outtextxy(320-50,120,"DIFFICULTY LEVEL");
    rectangle(240,200,300,260);
    outtextxy(240+25,200+30,"1");
    rectangle(300,200,360,260);
    outtextxy(300+25,200+30,"2");
    rectangle(360,200,420,260);
    outtextxy(360+25,200+30,"3");

    while(1)
    {
        getmouseclick();
        if(y>=200 && y< 260)
        {
            if(x>240 && x<300)
                return 1;
            else if(x>300 && x<360)
                return 2;
            else if(x>360 && x<420)
                return 3;

        }
    }
}
void modify_board::graphics_display_init()
{
    unsigned short int temp_y=0,temp_x=0;
    setcolor(DARKGRAY);
    for(int i=0;i<=8;i++)
    {
	line(temp_x,0,temp_x,479);
	line(0,temp_y,479,temp_y);
	temp_x += 60;
	temp_y += 60;
    }
    setfillstyle(SOLID_FILL,LIGHTGRAY);
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if( ((i+j)%2) != 0)
            floodfill(i*60+30,j*60+30,DARKGRAY);
        }
    }
    rectangle(620,0,640,20);
    outtextxy(625,3,"X");
}
void modify_board::graphics_display()
{
    int i,j;
    char str[10];
    mousehide();
    //cleardevice();
    //grid

    
    for(i=0;i<8;i++)
    {
	for(j=0;j<8;j++)
	{
	    
        if(prev_board.at[i][j] == board.at[i][j])
        {
            if((i+j)%2 == 0)
                setcolor(WHITE);
            else
                setcolor(LIGHTGRAY);

		    setlinestyle(SOLID_LINE,0,4);
            rectangle(i*60+3,j*60+3,i*60+60-2,j*60+60-2);
            setlinestyle(SOLID_LINE,0,1);
            continue;
        }
        strcpy(str,"");
	    switch(board.at[i][j])
	    {
	     case 0:
		  strcpy(str,"");
		  break;
	     case 1:
		  strcpy(str,"pawn.dat");
		  break;
	     case 2:
		  strcpy(str,"bishop.dat");
		  break;
	     case 3:
		  strcpy(str,"rook.dat");
		  break;
	     case 4:
		  strcpy(str,"knight.dat");
		  break;
	     case 5:
		  strcpy(str,"king.dat");
		  break;
	     case 6:
		  strcpy(str,"queen.dat");
		  break;
	     case 7:
		  strcpy(str,"pawn.dat");
		  break;
	     case 8:
		  strcpy(str,"bishop.dat");
		  break;
	     case 9:
		  strcpy(str,"rook.dat");
		  break;
	     case 10:
		  strcpy(str,"knight.dat");
		  break;
	     case 11:
		  strcpy(str,"king.dat");
		  break;
	     case 12:
		  strcpy(str,"queen.dat");
		  break;
	    }
        
        //if(board.at[i][j] != 0)
            display_piece(j,i,str,board.at[i][j]);
        setcolor(WHITE);
	}
    }
    prev_board = board;
    showmouse();
}


void modify_board::display_piece(int xgrid,int ygrid,char*filename,int pieceno)
{
    int i=0,k=0,y=0,num,x=0,temp=0,buffer[1000],n,LENGTH=4,piececolor,bgcolor;
    int basex = (xgrid*60)+15,basey =  (ygrid*60)+15;

    char ch;
    
    if((xgrid+ygrid)%2 == 0)
        bgcolor = WHITE;
    else
        bgcolor = LIGHTGRAY;

    if(pieceno > 6)
        piececolor = BLUE;
    else
        piececolor = RED;

    fstream fp;
    fp.open(filename,ios::in);
    fp.seekg(0,ios::beg);
    
    //blanking patch
    for(int i=0;i<35;i++)
        for(int j=0;j<35;j++)
            if(getpixel(basex+i,basey+j) != bgcolor)
                putpixel(basex+i,basey+j,bgcolor);
    if(pieceno == 0)
        return;

    i=0;
    do
    {
     fp.get(ch);
     buffer[i++] = ch;
    }while(!fp.eof() && i<1000);
    n = i;
    fp.close();
    
	for(int i=0;i<n;i++)
	{
        if(i%LENGTH == 0)
         y++;
        
        num  = buffer[n-i-1];
       
        if(num < 0)
              temp = 256+num;
        else
            temp = num; 
        for(int j = 0;j<8;j++)
        {
           
                //if(i%LENGTH < 3)
                  //putpixel(basex + (x++)%(LENGTH*8),basey + y,BLACK);
                if(num == 32)
                
                       putpixel(basex + (x++)%(LENGTH*8),basey + y,piececolor);
                else
                {
                 if(temp%2 == 1)
                 {
                        if(getpixel(basex + ((x++)%(LENGTH*8)),basey + y) != bgcolor)
                                          putpixel(basex + ((x)%(LENGTH*8)),basey + y,bgcolor);
                 }
                 else
                       putpixel(basex + (x++)%(LENGTH*8),basey + y,piececolor); 
                 
                 temp = temp/2;
                }
                  
        }
        
    }
}
move_status modify_board::validate_white(piece_pos source, piece_pos dest)
{
    move_status status;
    unsigned short int piece_source,piece_dest;
    short int x_diff,y_diff;
    piece_source = board.at[source.x][source.y];
    piece_dest = board.at[dest.x][dest.y];
    x_diff =  dest.x - source.x;
    y_diff = dest.y - source.y;

    /*if(piece_source == 0)
    {
	status.isvalid = 0;
	return status;
    }
    if(source.x==dest.x && source.y==dest.y)
    {
	status.isvalid = 0;
	return status;
    }
    if((piece_source<=WHITEEND) && (piece_dest <= WHITEEND) && (piece_dest >= WHITESTART))
    {
	status.isvalid = 0;
	return status;
    }

    if((piece_source>=BLACKSTART) && (piece_dest >= BLACKSTART))
    {
	status.isvalid = 0;
	return status;
    }*/
    if(piece_source == WHITEPAWN)
    {
	if((y_diff) == 0 && (x_diff) == 1)
	{
	    if((board.at[dest.x][dest.y] == NO_PIECE))
	    {
		status.isvalid = 1;
		return status;
	    }
	    status.isvalid = 0;
	    return status;
	}
	if((y_diff) == 0 && (x_diff) == 2)
	{
	    if(source.x == 1)
	    if((board.at[dest.x][dest.y] == NO_PIECE) && (board.at[source.x + 1][source.y] == NO_PIECE))
	    {
	       status.isvalid = 1;
		return status;
	    }
	    status.isvalid = 0;
	    return status;

	}
	if( ((x_diff)==1) && (abs(y_diff)==1) )
	{
	    if((board.at[dest.x][dest.y] >= BLACKSTART))
	    {
		status.isvalid = 1;
		status.kill = 1;
		return status;
	    }
	    status.isvalid = 0;
            return status;
	}
	status.isvalid = 0;
        return status;
    }
    if(piece_source == WHITEROOK)
    {
	unsigned short int flag;
        unsigned short int i;
        if((source.x==dest.x) || (source.y==dest.y))
        {
            flag=0;
            if(source.x==dest.x)
	         {
                if(source.y < dest.y)
                {
                    for(int i = source.y+1;i<dest.y;i++)
			            if(board.at[source.x][i] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
                        status.isvalid = 1;
			            return status;
                    }
                }
                else
		        {
		        for(i = dest.y+1;i<source.y;i++)
                            if(board.at[source.x][i] != NO_PIECE)
                                flag = 1;
                        if(flag==0)
                        {
                            status.isvalid = 1;
			    return status;
                    }
                }
            }
	        else if(source.y==dest.y)
            {
                if(source.x < dest.x)
                {
                    for(i = source.x+1;i<dest.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
		            	status.isvalid = 1;
			            return status;
                    }
                }
                else
                {
                    for(i = dest.x+1;i<source.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
			            status.isvalid = 1;
                        return status;
                    }
                }
            }
	}
        
        status.isvalid = 0;
        return status;
    }
    if(piece_source == WHITEBISHOP)
    {
	if(abs(x_diff) == abs(y_diff))
    {
	    if( ((x_diff)> 0) &&  ((y_diff)>0)  )
	    {
		unsigned short int cnt= 1;
		while(cnt < (x_diff))
		{
		    if((board.at[source.x + cnt][source.y + cnt]) != NO_PIECE)
		    {
			status.isvalid = 0;
			return status;
		    }
		    cnt++;
		}
		status.isvalid = 1;
		return status;
	    }
	    if( ((x_diff)> 0) && ((y_diff)<0)  )
	    {
		unsigned short int cnt= 1;
		while(cnt < (x_diff))
		{
		    if((board.at[source.x + cnt][source.y - cnt]) != NO_PIECE)
		    {
			status.isvalid = 0;
			return status;
		    }
		    cnt++;
		}
		status.isvalid = 1;
		return status;
	    }
	    if( ((x_diff)< 0) &&  ((y_diff)>0)  )
	    {
		unsigned short int cnt= 1;
		while(cnt < (y_diff))
		{
		    if(board.at[source.x - cnt][source.y + cnt] != NO_PIECE)
		    {
			status.isvalid = 0;
			return status;
		    }
		    cnt++;
		}
		status.isvalid = 1;
		return status;
	    }
	    if( ((x_diff)< 0) &&  ((y_diff)<0)  )
	    {
		unsigned short int cnt= 1;
		while(cnt < (-x_diff))
		{
		    if(board.at[source.x - cnt][source.y - cnt] != NO_PIECE)
		    {
			status.isvalid = 0;
			return status;
		    }
		    cnt++;
		}
		status.isvalid = 1;
		return status;
	    }
	}
    }
    if(piece_source == WHITEKNIGHT)
	{
	     if( (abs(source.x-dest.x)==1)  &&  (abs(source.y-dest.y)==2) )
		status.isvalid = 1;
	    else if( (abs(source.y-dest.y)==1)  &&  (abs(source.x-dest.x)==2) )
		status.isvalid = 1;
	    else
		status.isvalid = 0;

	    return status;
	}
    if(piece_source == WHITEKING)
    {
	if(abs(x_diff)<=1 && abs(y_diff)<=1)
            status.isvalid = 1;
        else
            status.isvalid = 0;
    return status;
    }
    if(piece_source == WHITEQUEEN)
    {
	    unsigned short int flag;
        unsigned short int i;
        if((source.x==dest.x) || (source.y==dest.y))
        {
            flag=0;
            if(source.x==dest.x)
	         {
                if(source.y < dest.y)
                {
                    for(int i = source.y+1;i<dest.y;i++)
			            if(board.at[source.x][i] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
                        status.isvalid = 1;
			            return status;
                    }
                }
                else
		        {
		        for(i = dest.y+1;i<source.y;i++)
                            if(board.at[source.x][i] != NO_PIECE)
                                flag = 1;
                    if(flag==0)
                    {
                            status.isvalid = 1;
			                return status;
                    }
                }
            }
	         else if(source.y==dest.y)
            {
                if(source.x < dest.x)
                {
                    for(i = source.x+1;i<dest.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
			            status.isvalid = 1;
			            return status;
                    }
                }
                else
                {
                    for(i = dest.x+1;i<source.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
			            status.isvalid = 1;
                        return status;
                    }
                }
            }
	    }
        else if( abs(x_diff) == abs(y_diff) )
	    {
	        if( ((x_diff)> 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
			}
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)> 0) &&  ((y_diff)<0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y - cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
		        }
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (y_diff))
		    {
			if(board.at[source.x - cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)<0)  )
                {
                    unsigned short int cnt= 1;
		    while(cnt < -x_diff)
                    {
		        if(board.at[source.x - cnt][source.y - cnt] != NO_PIECE)
                        {
                            status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
                    return status;
	        }
        }
        status.isvalid = 0;
        return status;
    }
    status.isvalid = 0;
    return status;
};
move_status modify_board::validate_black(piece_pos source, piece_pos dest)
{
    move_status status;
    unsigned short int piece_source,piece_dest;
    short int x_diff,y_diff;
    x_diff =  dest.x - source.x;
    y_diff = dest.y - source.y;

    piece_source = board.at[source.x][source.y];
    piece_dest = board.at[dest.x][dest.y];

    /*if(piece_source == 0)
    {
	status.isvalid = 0;
	return status;
    }
    if(source.x==dest.x && source.y==dest.y)
    {
	status.isvalid = 0;
	return status;
    }
    if((piece_source<=WHITEEND) && (piece_dest <= WHITEEND) && (piece_dest >= WHITESTART))
    {
	status.isvalid = 0;
	return status;
    }

    if((piece_source>=BLACKSTART) && (piece_dest >= BLACKSTART))
    {
	status.isvalid = 0;
	return status;
    }*/
    if(piece_source == BLACKPAWN)
    {
	if((y_diff) == 0 && (x_diff) == -1)
	{
	    if((board.at[dest.x][dest.y] == NO_PIECE))
	    {
		status.isvalid = 1;
		return status;
	    }
	    status.isvalid = 0;
	    return status;
	}
	if((y_diff) == 0 && (x_diff) == -2)
	{
	    if(source.x == 6)
	    if((board.at[dest.x][dest.y] == NO_PIECE) || (board.at[source.x + 1][source.y] == NO_PIECE))
	    {
	       status.isvalid = 1;
		return status;
	    }
	    status.isvalid = 0;
	    return status;

	}
	if(((x_diff)==-1) &&  (abs(y_diff)==1))
	{
	    if((piece_dest >= WHITESTART) && (piece_dest <= WHITEEND))
	    {
		status.isvalid = 1;
		status.kill = 1;
		return status;
	    }
	    status.isvalid = 0;
	    return status;
	}
	status.isvalid = 0;
	return status;
    }

    if(piece_source == BLACKBISHOP)
    {
	    if( abs(x_diff) == abs(y_diff) )
	    {
	        if( ((x_diff)> 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
			}
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)> 0) &&  ((y_diff)<0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y - cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
		        }
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (y_diff))
		    {
			if(board.at[source.x - cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)<0)  )
                {
                    unsigned short int cnt= 1;
		    while(cnt < -x_diff)
                    {
		        if(board.at[source.x - cnt][source.y - cnt] != NO_PIECE)
                        {
                            status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
                    return status;
	        }
        }
    }
    if(piece_source == BLACKROOK)
    {
	unsigned short int flag;
        unsigned short int i;
        if((source.x==dest.x) || (source.y==dest.y))
        {
            flag=0;
            if(source.x==dest.x)
	         {
                if(source.y < dest.y)
                {
                    for(int i = source.y+1;i<dest.y;i++)
			            if(board.at[source.x][i] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
                        status.isvalid = 1;
			            return status;
                    }
                }
                else
		        {
		        for(i = dest.y+1;i<source.y;i++)
                            if(board.at[source.x][i] != NO_PIECE)
                                flag = 1;
                        if(flag==0)
                        {
                            status.isvalid = 1;
			    return status;
                    }
                }
            }
	        else if(source.y==dest.y)
            {
                if(source.x < dest.x)
                {
                    for(i = source.x+1;i<dest.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
		            	status.isvalid = 1;
			            return status;
                    }
                }
                else
                {
                    for(i = dest.x+1;i<source.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
			            status.isvalid = 1;
                        return status;
                    }
                }
            }
	}
        
        status.isvalid = 0;
        return status;
    }
    if(piece_source == BLACKKNIGHT)
        {
            if( (abs(source.x-dest.x)==1)  &&  (abs(source.y-dest.y)==2) )
                status.isvalid = 1;
            else if( (abs(source.y-dest.y)==1)  &&  (abs(source.x-dest.x)==2) )
                status.isvalid = 1;
	    else
                status.isvalid = 0;
            
            return status;
	}
    if(piece_source == BLACKKING)
    {
	if(abs(x_diff)<=1 && abs(y_diff)<=1)
            status.isvalid = 1;
        else
            status.isvalid = 0;
    return status;
    }
    if(piece_source == BLACKQUEEN)
    {
	    unsigned short int flag;
        unsigned short int i;
        if((source.x==dest.x) || (source.y==dest.y))
        {
            flag=0;
            if(source.x==dest.x)
	         {
                if(source.y < dest.y)
                {
                    for(int i = source.y+1;i<dest.y;i++)
			            if(board.at[source.x][i] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
                        status.isvalid = 1;
			            return status;
                    }
                }
                else
		        {
		        for(i = dest.y+1;i<source.y;i++)
                            if(board.at[source.x][i] != NO_PIECE)
                                flag = 1;
                        if(flag==0)
                        {
                            status.isvalid = 1;
			    return status;
                    }
                }
            }
	        else if(source.y==dest.y)
            {
                if(source.x < dest.x)
                {
                    for(i = source.x+1;i<dest.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
		            	status.isvalid = 1;
			            return status;
                    }
                }
                else
                {
                    for(i = dest.x+1;i<source.x;i++)
			            if(board.at[i][source.y] != NO_PIECE)
                            flag = 1;
                    if(flag==0)
                    {
			            status.isvalid = 1;
                        return status;
                    }
                }
            }
	    }
        else if( abs(x_diff) == abs(y_diff) )
	    {
	        if( ((x_diff)> 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
			}
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)> 0) &&  ((y_diff)<0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (x_diff))
		    {
		        if(board.at[source.x + cnt][source.y - cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
			    return status;
		        }
		        cnt++;
		    }
		    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)>0)  )
	        {
		    unsigned short int cnt= 1;
		    while(cnt < (y_diff))
		    {
			if(board.at[source.x - cnt][source.y + cnt] != NO_PIECE)
		        {
			    status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
		    return status;
	        }
		if( ((x_diff)< 0) &&  ((y_diff)<0)  )
                {
                    unsigned short int cnt= 1;
		    while(cnt < -x_diff)
                    {
		        if(board.at[source.x - cnt][source.y - cnt] != NO_PIECE)
                        {
                            status.isvalid = 0;
                            return status;
		        }
                        cnt++;
                    }
                    status.isvalid = 1;
                    return status;
	        }
        }
        status.isvalid = 0;
        return status;
    }
    status.isvalid = 0;
    return status;
}


bool modify_board::king_check(short int iswhite)
{
    short int i,j;
    piece_pos source,dest;
    move_status  status;
    if(iswhite)
    {
        for(i=0;i<8;i++)
            for(j=0;j<8;j++)
                if(board.at[i][j] == WHITEKING )
                {
                    dest.x = i;
                    dest.y = j;
                }

        for(source.x=0;source.x<8;source.x++)
        {    
            for(source.y=0;source.y<8;source.y++)
            {
                if(board.at[source.x][source.y] == NO_PIECE)
                    continue;
                if(source.x==dest.x && source.y==dest.y)
                    continue;
                if((board.at[source.x][source.y]<=WHITEEND) && (board.at[dest.x][dest.y] <= WHITEEND) && (board.at[dest.x][dest.y] >= WHITESTART))
                    continue;
                if((board.at[source.x][source.y]>=BLACKSTART) && (board.at[dest.x][dest.y] >= BLACKSTART))
                    continue;

                status = validate_black(source,dest);
                if(status.isvalid)
                    return true;
            }
        }
        return false;
    }
    else
    {
         for(i=0;i<8;i++)
            for(j=0;j<8;j++)
                if(board.at[i][j] == BLACKKING )
                {
                    dest.x = i;
                    dest.y = j;
                }

        for(source.x=0;source.x<8;source.x++)
        {    
            for(source.y=0;source.y<8;source.y++)
            {
                if(board.at[source.x][source.y] == NO_PIECE)
                    continue;
                if(source.x==dest.x && source.y==dest.y)
                    continue;
                if((board.at[source.x][source.y]<=WHITEEND) && (board.at[dest.x][dest.y] <= WHITEEND) && (board.at[dest.x][dest.y] >= WHITESTART))
                    continue;
                if((board.at[source.x][source.y]>=BLACKSTART) && (board.at[dest.x][dest.y] >= BLACKSTART))
                    continue;

                
                status = validate_white(source,dest);
                if(status.isvalid)
                    return true;
            }
        }
        return false;
    }
}
generic_move modify_board::traverse_all(int depth,unsigned short int iswhite,int alpha)
{
    piece_pos source,dest;

    generic_move trial_move,best_move;//,quickest_move;
    int points2;
    //trial_move = new generic_move;
    //best_move = new generic_move;
    //quickest_move = new generic_move;

    move_status status;
    unsigned short int killed_piece;
    bool flag_first_valid=true,flag_pawn_promotion=false;//flag_first_loop=1;//,flag_in=0

    depth++;
    //count++;
    //for(int i=max_depth;(i>=1);i=i-2)
    //{
	if(depth < maximum_depth)
	{
	    //flag_in = 1;
	    flag_first_valid = true;
	    
        //  INIT POINTS //
        if(iswhite)
		    points2 = best_move.points = -MY_INFINITY;
	    else
	       points2 = best_move.points = MY_INFINITY;
	    
        // TRAVERSE ALL //
        for(source.x =0 ;source.x<8;source.x++)
	    {
		for(source.y =0 ;source.y<8;source.y++)
		{
		    for(dest.x =0 ;dest.x<8;dest.x++)
		    {
			for(dest.y =0 ;dest.y<8;dest.y++)
			{

				if(board.at[source.x][source.y] == NO_PIECE)
                    continue;
                if(source.x==dest.x && source.y==dest.y)
                    continue;
                if((board.at[source.x][source.y]<=WHITEEND) && (board.at[dest.x][dest.y] <= WHITEEND) && (board.at[dest.x][dest.y] >= WHITESTART))
                    continue;
                if((board.at[source.x][source.y]>=BLACKSTART) && (board.at[dest.x][dest.y] >= BLACKSTART))
                    continue;
                
                if(iswhite)
				    status = validate_white(source,dest);
				else
				    status = validate_black(source,dest);

				
                if(status.isvalid == 1)
				{
                    if( (board.at[source.x][source.y] == WHITEPAWN && dest.x == 7))
						flag_pawn_promotion = true;
					
					killed_piece = make_move(source,dest);
                    
					if(king_check(iswhite))
                    {
                        if(flag_pawn_promotion)
						{
							undo_promotion_move(source,dest,killed_piece);
							flag_pawn_promotion = false;
						}
						else
							undo_move(source,dest,killed_piece);
                        continue;
                    }
				    
                    trial_move = traverse_all(depth,!iswhite,best_move.points);
				    
				    
					if(flag_pawn_promotion)
					{
						undo_promotion_move(source,dest,killed_piece);
						flag_pawn_promotion = false;
					}
					else
						undo_move(source,dest,killed_piece);
					
					
				    if(flag_first_valid)
				    {
					    best_move.source = source;
					    best_move.dest = dest;
					    
                        best_move.points = trial_move.points;
					    best_move.points2 = trial_move.points2;
                        
                        //alpha = best_move.points;
                        best_move.ismate = false;
                        flag_first_valid = false;
				    }
				    if(iswhite)
				    {
					    if((trial_move.points > best_move.points))
					    {
 
                            points2 = best_move.points;
                            
                            best_move.points = trial_move.points;
					        best_move.points2 = trial_move.points2;
                            best_move.source = source;
					        best_move.dest = dest;
                            //alpha =  best_move.points;

                           
					    }
                        else if(trial_move.points > points2)
                            points2 = trial_move.points;
                        
                        if(best_move.points > alpha)
                        {
                            best_move.points2 = points2;
                            return best_move;
                        }
				    }
				    else
				    {
					    if((trial_move.points < best_move.points) || ( (trial_move.points == best_move.points) && (trial_move.points2 < best_move.points2)) )
					    {
 
                            points2 = best_move.points;
                            
                            best_move.points = trial_move.points;
					        best_move.points2 = trial_move.points2;
                            best_move.source = source;
					        best_move.dest = dest;
                            //alpha =  best_move.points;

                           
					    }
                        else if(trial_move.points < points2)
                            points2 = trial_move.points;
			            
                        if(best_move.points < alpha)
                        {
                            best_move.points2 = points2;
                            return best_move;
                        }
				    }

                }
			}//dy
		    }//dx
		}//sy
	    }//sx
	    //if(flag_first_loop)
	    //{
		//quickest_move = best_move;
		//flag_first_loop = 0;
	    //}

	    /*if(iswhite)
	    {
	    if(best_move.points == quickest_move.points)
		quickest_move = best_move;
	    }
	    else
	    {
	    if(best_move.points == quickest_move.points)
		quickest_move = best_move;
	    }*/

	    //if(depth== 1)
	    //{
		//cout<<best_move.points;
	    //}
	    //return(best_move);
        best_move.points2 = points2;
	    
        return(best_move);
    }
    //}//end of for
    //if(flag_in == 0)
    //{
	best_move.points = analyse_board();
    best_move.points2 = 0;
	return(best_move);
    //}
    
}

/*unsigned short int modify_board::make_move(piece_pos source,piece_pos dest)
{
    unsigned short int killed_piece;
    killed_piece = board.at[dest.x][dest.y];
    
	
	board.at[dest.x][dest.y] = board.at[source.x][source.y];
    board.at[source.x][source.y] = NO_PIECE;
    return killed_piece;
}*/
void modify_board::undo_move(piece_pos source,piece_pos dest,unsigned short int killed_piece)
{
    unsigned short int curr_piece;
	moves--;
	curr_piece = board.at[dest.x][dest.y];
    board.at[dest.x][dest.y] = killed_piece;
    board.at[source.x][source.y] = curr_piece;
}
unsigned short int modify_board::make_move(piece_pos source,piece_pos dest)
{
    unsigned short int killed_piece;
    killed_piece = board.at[dest.x][dest.y];
    moves++;
	
	if(board.at[source.x][source.y] == WHITEPAWN && dest.x == 7)
		board.at[dest.x][dest.y] = WHITEQUEEN;
	else if(board.at[source.x][source.y] == BLACKPAWN && dest.x == 0)
		board.at[dest.x][dest.y] = BLACKQUEEN;
	else
		board.at[dest.x][dest.y] = board.at[source.x][source.y];
		
	board.at[source.x][source.y] = NO_PIECE;
    return killed_piece;
}
void modify_board::undo_promotion_move(piece_pos source,piece_pos dest,unsigned short int killed_piece)
{
    unsigned short int curr_piece;
    moves--;
	
	if(board.at[dest.x][dest.y] == WHITEQUEEN)
		curr_piece = WHITEPAWN;
	else
		curr_piece = BLACKPAWN;
		
    board.at[dest.x][dest.y] = killed_piece;
    board.at[source.x][source.y] = curr_piece;
}
bool within_board(int num1,int num2)
{
	if((num1 >= 0 && num1 <=7) && (num2 >= 0 && num2 <=7))
		return true;
	else
		return false;
}
int modify_board::positional_advantage_bishop(int x,int y)
{
	int blocks_covered = 0;
	
	int i = x + 1,j = y + 1;
	for(;within_board(i,j) && board.at[i][j] == NO_PIECE;i++,j++)
		blocks_covered ++;
	
	i = x - 1,j = y - 1;
	for(;within_board(i,j) && board.at[i][j] == NO_PIECE;i--,j--)
		blocks_covered ++;
	
	i = x + 1,j = y - 1;
	for(;within_board(i,j) && board.at[i][j] == NO_PIECE;i++,j--)
		blocks_covered ++;
	
	i = x - 1,j = y + 1;
	for(;within_board(i,j) && board.at[i][j] == NO_PIECE;i--,j++)
		blocks_covered ++;
		
	return (blocks_covered*POINTS_PER_BLOCK);
}
int modify_board::positional_advantage_rook(int x,int y)
{
	int blocks_covered = 0;
	
	int i = x + 1;
	for(;within_board(i,y) && board.at[i][y] == NO_PIECE;i++)
		blocks_covered ++;
	
	i = x - 1;
	for(;within_board(i,y) && board.at[i][y] == NO_PIECE;i--)
		blocks_covered ++;
	
	i = x + 1;
	for(;within_board(x,i) && board.at[x][i] == NO_PIECE;i++)
		blocks_covered ++;
	
	i = x - 1;
	for(;within_board(x,i) && board.at[x][i] == NO_PIECE;i--)
		blocks_covered ++;
		
	return (blocks_covered*POINTS_PER_BLOCK);
}
int modify_board::positional_advantage_knight(int x,int y)
{
	int blocks_covered = 0;
	
	int i = x + 2,j = y + 1;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	i = x + 2,j = y - 1;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
		
	i = x + 1,j = y + 2;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	i = x + 1,j = y - 2;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
		
	i = x - 1,j = y + 2;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	i = x - 1,j = y - 2;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	i = x - 2,j = y + 1;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	i = x - 2,j = y - 1;
	if(within_board(i,j) && board.at[i][j] == NO_PIECE)
		blocks_covered ++;
	
	return (blocks_covered*POINTS_PER_BLOCK);
}
int modify_board::positional_advantage_queen(int x,int y)
{
	return (positional_advantage_bishop(x,y) + positional_advantage_rook(x,y));
}

int modify_board::analyse_board()
{
    piece_pos source,white_king_pos,black_king_pos;
	move_status status;
	int pts = 0;
    
    f1.assign_input("game_phase",moves);
	
	int pawn_promotion_factor = f1.defuzzify("pawn_promotion_factor");

	for(source.x =0 ;source.x<8;source.x++)
    {
	for(source.y =0 ;source.y<8;source.y++)
	{
	    switch(board.at[source.x][source.y])
	    {
	    case 0:
		break;
	    case WHITEPAWN:
		pts += 100;
		break;
	    case WHITEBISHOP:
		pts += 300 + positional_advantage_bishop(source.x,source.y);
		break;
	    case WHITEROOK:
		pts += 400 + positional_advantage_rook(source.x,source.y);
		break;
	    case WHITEKNIGHT:
		pts += 300 + positional_advantage_knight(source.x,source.y);
		break;
	    case WHITEKING:
		pts += 5000;
		white_king_pos = source;
		break;
	    case WHITEQUEEN:
		pts += 900 + positional_advantage_queen(source.x,source.y);
		break;
	    case BLACKPAWN:
		pts -= 100;
		break;
	    case BLACKBISHOP:
		pts -= 300 + positional_advantage_bishop(source.x,source.y);
		break;
	    case BLACKROOK:
		pts -= 400 + positional_advantage_rook(source.x,source.y);
		break;
	    case BLACKKNIGHT:
		pts -= 300 + positional_advantage_knight(source.x,source.y);
		break;
	    case BLACKKING:
		pts -= 5000;
		black_king_pos = source;
		break;
	    case BLACKQUEEN:
		pts -= 900 + positional_advantage_queen(source.x,source.y);
		break;
	    }
	    
        // BISHOP ON PRINCIPLE DIAGONAL IN OPENING
		if(moves <= 5)
        {
            if( (((source.x+source.y)==7)||(source.x==source.y))  && board.at[source.x][source.y] == WHITEBISHOP)
		     pts += 5;
	        if( (((source.x+source.y)==7)||(source.x==source.y))  && board.at[source.x][source.y] == BLACKBISHOP)
		     pts -= 5;
        }
		
		//PAWN PROMOTION IN END GAME
        if(board.at[source.x][source.y] == WHITEPAWN)
	     pts += source.x  * (pawn_promotion_factor)/10;
        else if(board.at[source.x][source.y] == BLACKPAWN)
	     pts -= (7 - source.x) * (pawn_promotion_factor)/10;
	
	}
    }
    
	//KING CHECKED
	if(king_check(1))
		pts += 2;
	if(king_check(0))
		pts -= 2;
		
    //BOARD CENTER CONTROL IN OPENING
	int center_control_pts = f1.defuzzify("center_control_pts");

	
    for(int i=2;i<=5;i++)
    {
    for(int j=2;j<=5;j++)
    {
        if(board.at[i][j] == WHITEKNIGHT)
	        pts += center_control_pts;
        if(board.at[i][j] == BLACKKNIGHT)
	        pts -= center_control_pts;
        if(board.at[i][j] == WHITEPAWN)
            pts += center_control_pts/2;
        if(board.at[i][j] == BLACKPAWN)
            pts -= center_control_pts/2;
    }
    }

    
    
	if(pts > MY_INFINITY)
        pts = MY_INFINITY;
    else if(pts < -MY_INFINITY)
        pts = -MY_INFINITY;
    return pts;
}
void modify_board::next_move()
{
    unsigned short int tempx,tempy;
    generic_move best_move,comp_move;
    move_status s1;
    char str[10];
    
    moves++;
    best_move = traverse_all(0,1,MY_INFINITY);
    if(!best_move.ismate)
        make_move(best_move.source,best_move.dest);
    else
    {
        setcolor(WHITE);
        outtextxy(500,240,"WHITE CHECKMATED");
        while(!kbhit());
        exit(1);
    }   
    comp_move = best_move;
    
    do{
        graphics_display();
        
        Beep(2000,175);
        Beep(1000,125);

        setcolor(RED);
	    setlinestyle(SOLID_LINE,0,4);
	    rectangle(comp_move.source.y*60+3,comp_move.source.x*60+3,comp_move.source.y*60+60-2,comp_move.source.x*60+60-2);
        rectangle(comp_move.dest.y*60+3,comp_move.dest.x*60+3,comp_move.dest.y*60+60-2,comp_move.dest.x*60+60-2);
        setlinestyle(SOLID_LINE,0,1);
        setcolor(WHITE);
        /*cout<<"\n\nBLACK's MOVE :\n";
        cout<<"\nSOURCE :";
        cin>>best_move.source.x;
        cin>>best_move.source.y;
        cout<<"\nDEST :";
        cin>>best_move.dest.x;
        cin>>best_move.dest.y;
        */
	    do{
	        getmouseclick();

	        tempx = y/60;
	        tempy = x/60;

	        if(tempx<8 && tempy < 8)
	        {
		    setcolor(BLUE);
		    setlinestyle(SOLID_LINE,0,4);
            rectangle(tempy*60+3,tempx*60+3,tempy*60+60-2,tempx*60+60-2);
		    setlinestyle(SOLID_LINE,0,1);
            setcolor(WHITE);
		    best_move.source.x = tempx;
		    best_move.source.y = tempy;
		    break;
	        }
            
	    }while(1);

	    do{
	        getmouseclick();

	        tempx = y/60;
	        tempy = x/60;

	        if(tempx<8 && tempy < 8)
	        {
		        setcolor(BLUE);
		        setlinestyle(SOLID_LINE,0,4);
                rectangle(tempy*60+3,tempx*60+3,tempy*60+60-2,tempx*60+60-2);
		        setlinestyle(SOLID_LINE,0,1);
		        setcolor(WHITE);

		        best_move.dest.x = tempx;
		        best_move.dest.y = tempy;
		        break;
	        }
	    }
	    while(1);
        
        if(board.at[best_move.source.x][best_move.source.y] == NO_PIECE)
            continue;
        if(best_move.source.x==best_move.dest.x && best_move.source.y==best_move.dest.y)
            continue;
        if((board.at[best_move.source.x][best_move.source.y]<=WHITEEND) && (board.at[best_move.dest.x][best_move.dest.y] <= WHITEEND) && (board.at[best_move.dest.x][best_move.dest.y] >= WHITESTART))
            continue;
        if((board.at[best_move.source.x][best_move.source.y]>=BLACKSTART) && (board.at[best_move.dest.x][best_move.dest.y] >= BLACKSTART))
            continue;
                    
        s1 = validate_black(best_move.source,best_move.dest);

        if(s1.isvalid)
        {
            int killed_piece = make_move(best_move.source,best_move.dest);
            if(king_check(0))//0 !iswhite
                undo_move(best_move.source,best_move.dest,killed_piece);
            else
                break;
        }
        
    }while(1);
    graphics_display();
    
};
void modify_board::next_move_cpu()
{
    unsigned short int tempx,tempy;
    generic_move best_move,comp_move;
    move_status s1;
    char str[10];
    
    
    // WHITE MOVE
	best_move = traverse_all(0,1,MY_INFINITY);
    
	if(!best_move.ismate)
	{
        make_move(best_move.source,best_move.dest);
		graphics_display();
	}
	else
    {
        setcolor(BLACK);
        outtextxy(475,240,"WHITE CHECKMATED");
        while(!kbhit());
        exit(1);
    }   
    if(kbhit())
		getchar();
    
    // BLACK MOVE
	best_move = traverse_all(0,0,-MY_INFINITY);
    
	if(!best_move.ismate)
	{
        make_move(best_move.source,best_move.dest);
		graphics_display();
    }
	else
    {
        setcolor(BLACK);
        outtextxy(475,240,"BLACK CHECKMATED");
        while(!kbhit());
        exit(1);
    } 
	if(kbhit())
		getchar();
    
    
};
void modify_board::init_fuzzy_model()
{
	f1.add_input("game_phase");
	f1.add_input_subset("game_phase","opening",0,20,30,TRAPEZOID_LEFT_UNBOUNDED);
	f1.add_input_subset("game_phase","mid_game",30,10,60,TRAPEZOID);
	f1.add_input_subset("game_phase","end_game",60,10,40,TRAPEZOID_RIGHT_UNBOUNDED);

	//Board centre capture, King check points , bishop on principle diagonal , rook on open rank , pawn promotion points , piece square occupied points

	f1.add_output("center_control_pts");
	f1.add_output_subset("center_control_pts","small",0,10,20,TRAPEZOID_LEFT_UNBOUNDED);
	f1.add_output_subset("center_control_pts","medium",10,10,20,TRAPEZOID);
	f1.add_output_subset("center_control_pts","large",20,10,20,TRAPEZOID_RIGHT_UNBOUNDED);
	
	f1.add_output("pawn_promotion_factor");
	f1.add_output_subset("pawn_promotion_factor","small",0,10,20,TRAPEZOID_LEFT_UNBOUNDED);
	f1.add_output_subset("pawn_promotion_factor","medium",20,10,20,TRAPEZOID);
	f1.add_output_subset("pawn_promotion_factor","large",40,10,20,TRAPEZOID_RIGHT_UNBOUNDED);

	f1.define_rule("IF game_phase IS opening THEN center_control_pts IS large");
	f1.define_rule("IF game_phase IS mid_game THEN center_control_pts IS small");
	f1.define_rule("IF game_phase IS end_game THEN center_control_pts IS small");

	f1.define_rule("IF game_phase IS opening THEN pawn_promotion_factor IS small");
	f1.define_rule("IF game_phase IS mid_game THEN pawn_promotion_factor IS small");
	f1.define_rule("IF game_phase IS end_game THEN pawn_promotion_factor IS large");

	
}
int main()
{
    int gd = DETECT,gm;
    initgraph(&gd,&gm,"");
    setbkcolor(WHITE);
    cleardevice();
    
    int ch,pts;
    char str[10];
    modify_board modify;
    modify.set_start_pos();
   
    cout<<"\nINITIALIZING MOUSE.";
    modify.initmouse();
    modify.showmouse();

    cout<<"\nINITIALIZING FUZZY MODEL.";
	modify.init_fuzzy_model();
    
	modify.maximum_depth = modify.get_difficulty();
    modify.maximum_depth += 3;
    
	cout<<"\nINITIALIZING GRAPHICS.";
	cleardevice();
    modify.graphics_display_init();
    do
    {
		modify.next_move_cpu();
		//pts = modify.analyse_board();
		
	}while(1);
    return 0;
}

