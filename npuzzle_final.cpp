#include <iostream>
#include <stdlib.h>
#include <queue>
#include <vector>
#include <stdio.h>
#include <time.h>
using namespace std;

class npuzzle
{
  int **initial_state;// stores intial state in matrix form
  int **current_state;// stores current state in matrix form
  int current_blank[2]; // current position of the blank column
  int **goal_state;// stores the location of each blocks, goal_state[0][i] => row of the number i in goal, goal_state[1][i]=> column of number i in goal
  int **goal_state_matrix;
  int nval; // stores the value of N
  npuzzle *prnt;//parent pointer
  int f_val;//f=g+h
  int h_val;//
  int g_val;//
public:
  npuzzle(int); // constructor initializes all values to 0
  ~npuzzle(); // destroys all the allocated memory


  bool valid(int **);  // check if a state is valid or not
  bool build_goal_state(int **); // build the object if the goal is valid,returns true if success
  bool build_initial_state(int **);// build the object if initial state is valid and assign current state to initial state, returns true if success

  bool move(int);// moves a block to the empty block, 1 -> moves from top ,2-> from bottom , 3-> from left , 4-> from right, returns true if success
  int maxswaphrts();// N-max swap heuristics
  int manhattan_distance();
  int tiles_out_of_pos();

  void display_goal_state(int); // display the goal, if argument =1 then in location form, argument =2, then in matrix form
  void display_initial_state(); // display the initial state
  void display_current_state();// display current state

  bool solvable();
  int countInversions(int*, int);
  int mergesort(int*, int*, int, int);
  int merge(int*, int*, int, int, int);

  bool goal_test(); // returns true if the current state is goal
  bool goal_test(int**); // check if the argument is goal or not
  int misplaced();//number of misplaced tiles heuristic
  int linear_conflict();//linear conflict heuristic	
  void calcf();//calculates the value of f from g,h
  int f_value() const {return f_val;}
  int A_star1();//returns the step count
  int A_star2();
  int A_star3();
  int A_star4();
  int A_star5();
  void assign_g(int);
  void assign_h(int);
  void copy(npuzzle*);//copies the entire npuzzle object
  bool state_cmp(npuzzle*);//Compares the current state
};

npuzzle::npuzzle(int n):prnt(0)
{
  int i,j;
  nval=n;
  f_val=0;
  g_val=0;
  h_val=0;
  goal_state=new int*[2];
  initial_state=new int*[nval];
  current_state=new int*[nval];
  goal_state_matrix=new int*[nval];
  current_blank[0]=0;
  current_blank[1]=0;
  goal_state[0]=new int[nval*nval];
  goal_state[1]=new int[nval*nval];
  for (i=0;i<nval*nval;i++)
    {
      goal_state[0][i]=0;
      goal_state[1][i]=0;
    }
  for (i=0;i<nval;i++)
    {
      initial_state[i]=new int[nval];
      current_state[i]=new int[nval];
      goal_state_matrix[i]=new int[nval];
    }
  for (i=0;i<nval;i++)
    {
      for(j=0;j<nval;j++)
	{
	  initial_state[i][j]=0;
	  current_state[i][j]=0;
	  goal_state_matrix[i][j]=0;
	}
    }
	
}

npuzzle::~npuzzle()
{
  //  cout<<"destructor"<<endl;
  int i;
  delete goal_state[0];

  delete goal_state[1];	
  
  for(i=0;i<nval;i++)
    {
      delete initial_state[i];
      delete current_state[i];
      delete goal_state_matrix[i];
    }
  delete goal_state;
  delete initial_state;
  delete current_state;
  delete goal_state_matrix;
  //  delete prnt;
}


bool npuzzle::valid(int** rslt) //returns true if the state is valid
{	
  int i,j;
  bool* check=new bool[nval*nval]; // map to validate the goal_state input
  for(i=0;i<nval*nval;i++)
    {
      check[i]=false; //initializing
    }

  for (i=0;i<nval;i++)
    {
      for(j=0;j<nval;j++)
	{
	  if (rslt[i][j]<0 || rslt[i][j]>=nval*nval)
	    {
	      return false;
	    }
	  else
	    {

	      if (check[rslt[i][j]]==false) //checking if number is already present
		{
		  check[rslt[i][j]]=true;
		}
	      else
		{
		  return false; //return false if the number is already present
		}
	    }

	}
    }
  for(i=0;i<nval*nval;i++)
    {
      if (check[i]== false)
	{
	  return false;// return false if some number from 0 to n*n-1 is absent
	}
    }
  delete check;

	
  return true;

}
bool npuzzle::build_initial_state(int** initial) // returns true if success
{
  int i,j;
  if (this->valid(initial)==true)
    {
      

      for (i=0;i<nval;i++)
	{
	  for(j=0;j<nval;j++)
	    {
	      initial_state[i][j]=initial[i][j];
	      current_state[i][j]=initial_state[i][j];
	      if (current_state[i][j] == 0)
		{
		  current_blank[0]=i;
		  current_blank[1]=j;
		}
	    }
	}
      return true;
    }
  else
    {
      return false;
    }

}
void npuzzle::display_initial_state()
{
  int i,j;
  for (i=0;i<nval;i++)
    {
      for(j=0;j<nval;j++)
	{
	  cout<<initial_state[i][j]<<" ";
	}
      cout<<endl;
		
    }
  cout<<endl;
}
void npuzzle::display_current_state()
{
  int i,j;
  for (i=0;i<nval;i++)
    {
      for(j=0;j<nval;j++)
	{
	  cout<<current_state[i][j]<<" ";
	}
      cout<<endl;
		
    }
  cout<<endl;
}

bool npuzzle::build_goal_state(int** goal)
{
  int i,j;
  if (this->valid(goal)==true)
    {


      for (i=0;i<nval;i++)
	{
	  for(j=0;j<nval;j++)
	    {
	      goal_state[0][goal[i][j]]=i;
	      goal_state[1][goal[i][j]]=j;
	      goal_state_matrix[i][j]=goal[i][j];
	    }
	}

      return true;
    }
  else
    {
      return false;
    }

}
void npuzzle::display_goal_state(int n)
{
  if (n==1)
    {
      for (int i=0;i<nval*nval;i++)
	{
	  cout<<i<<":"<<goal_state[0][i]<<" "<<goal_state[1][i]<<endl;
			
	}
    }
  if (n==2)
    {
      for (int i=0;i<nval;i++)
	{
	  for(int j=0;j<nval;j++)
	    {
	      cout<<goal_state_matrix[i][j]<<" ";
	    }
	  cout<<endl;
	}
    }
  cout<<endl;
}

bool npuzzle::move(int mov)
{
  int temp,row,col;
  row=current_blank[0];
  col=current_blank[1];
  if (this->valid(current_state) == false)
    {
      return false;
    }

  switch(mov)
    {
    case 1:     		 //move from up to blank

      if (row==0) // if blank is at the topmost row
	{
	  return false;
	}
      else
	{
	  temp=current_state[row-1][col];
	  current_state[row-1][col]=current_state[row][col];
	  current_state[row][col]=temp;
	  current_blank[0]=row-1;
				
	}




      break;

    case 2:				// move from down to blank	

      if (row==nval-1) // if the blank is at the bottom most row
	{
	  return false;
	}
      else
	{
	  temp=current_state[row+1][col];
	  current_state[row+1][col]=current_state[row][col];
	  current_state[row][col]=temp;
	  current_blank[0]=row+1;
				

	}

      break;

    case 3:				// move from left to blank
      if (col==0) // if blank is at the left most col
	{
	  return false;
	}
      else
	{
	  temp=current_state[row][col-1];
	  current_state[row][col-1]=current_state[row][col];
	  current_state[row][col]=temp;
	  current_blank[1]=col-1;
	}

      break;

    case 4:				// move from right to blank
      if (col==nval-1) // if blank is at the right most col
	{
	  return false;
	}
      else
	{
	  temp=current_state[row][col+1];
	  current_state[row][col+1]=current_state[row][col];
	  current_state[row][col]=temp;
	  current_blank[1]=col+1;
	}

      break;

    default:
      return false;

	
    }
  return true;
}

bool npuzzle::goal_test()  // returns true if the current state is goal
{
  int i,j,val;
  for (i=0;i<nval;i++)
    {
      for (j=0;j<nval;j++)
	{
	  val=current_state[i][j];
	  if (goal_state[0][val]!= i || goal_state[1][val]!= j)
	    {
	      return false;
	    }


	}
    }
  return true;
}

bool npuzzle::goal_test(int** test)  // returns true if the argument is goal or not
{
  int i,j,val;
  if (this->valid(test)==false)
    {
      return false;
    }
  for (i=0;i<nval;i++)
    {
      for (j=0;j<nval;j++)
	{
	  val=test[i][j];
	  if (goal_state[0][val]!= i || goal_state[1][val]!= j)
	    {
	      return false;
	    }


	}
    }
  return true;
}


int npuzzle::maxswaphrts()
{
  int row,col,i,j,count =1,val;
  int rowgoal,colgoal;
	
  rowgoal=goal_state[0][0];
  colgoal=goal_state[1][0];

  int **current_loc=new int*[2];
  current_loc[0]=new int[nval*nval];
  current_loc[1]=new int[nval*nval];
	
	
  for (i=0;i<nval;i++)
    {
      for(j=0;j<nval;j++)
	{
			
	  current_loc[0][current_state[i][j]]=i;
	  current_loc[1][current_state[i][j]]=j;

	}
    }
		
  while ((current_loc[0][0]!=rowgoal) || (current_loc[1][0]!=colgoal))
    {
      val=goal_state_matrix[current_loc[0][0]][current_loc[1][0]];
      row=current_loc[0][val];
      col=current_loc[1][val];
      current_loc[0][val]=current_loc[0][0];
      current_loc[1][val]=current_loc[1][0];
      current_loc[0][0]=row;
      current_loc[1][0]=col;
      count++;
    }


  delete current_loc[0];
  delete current_loc[1];
  delete current_loc;
  return count;
	


}
int npuzzle::manhattan_distance()
{
  int man_dist = 0;
  for(int i = 0; i < nval; i++)
    {
      for(int j = 0; j < nval; j++)
	{
	  if(current_state[i][j] != 0)
	    {
	      if(i > goal_state[0][current_state[i][j]])
		man_dist += i - goal_state[0][current_state[i][j]];
	      else
		man_dist += goal_state[0][current_state[i][j]] - i;
	      if(j > goal_state[1][current_state[i][j]])
		man_dist += j - goal_state[1][current_state[i][j]];
	      else
		man_dist += goal_state[1][current_state[i][j]] - j;			
	    }
	}
    }
  return man_dist;
}

int npuzzle::tiles_out_of_pos()
{
  int count = 0;
  for(int i = 0; i < nval; i++)
    {
      for(int j = 0; j < nval; j++)
	{
	  if(current_state[i][j] != 0)
	    {
	      if(i != goal_state[0][current_state[i][j]])
		count++;
	      if(j != goal_state[1][current_state[i][j]])
		count++;
	    }
	}
    }
  return count;	
}


int npuzzle::misplaced()
{

  int i,j,val,cnt=0;
  for (i=0;i<nval;i++)
    {
      for (j=0;j<nval;j++)
	{
	  val=current_state[i][j];
	  if (goal_state[0][val]!= i || goal_state[1][val]!= j)
	    {
	      cnt++;
	    }
	  
	  
	}
    }
  return cnt;
  
}
int npuzzle::linear_conflict()
{

  int Tj,Tk,conf=0;//no.of conflicting pairs
  int man_dist=0;
  //traversing rowwise
  for(int i=0;i<nval;i++)
    for(int j=0;j<nval;j++)
      {
	Tk=current_state[i][j];
	if(Tk!=0 && goal_state[0][Tk]==i)//proceed only if in the same row
	  {
	    for(int k=j;k<nval;k++)
	      {
		Tj=current_state[i][k];
		if(Tj!=0 && goal_state[0][Tj]==i && (goal_state[1][Tk]>goal_state[1][Tj]))
		  { 
		    //cout<<"Here"<<Tj<<" "<<Tk<<endl;
		    man_dist=0;
		    if(j > goal_state[1][Tk])
		      man_dist += j - goal_state[1][Tk];
		    else
		      man_dist += goal_state[1][Tk] - j;

		    if(k > goal_state[1][Tj])
		      man_dist += k - goal_state[1][Tj];
		    else
		      man_dist += goal_state[1][Tj] - k;

		    conf+=man_dist+2;
		    //cout<<"Hello"<<endl;
		    //cout<<man_dist<<"man"<<endl;

		    //cout<<Tj<<" "<<Tk<<endl;
		  }
	      }
	  }
      }

  //traversing columnwise
  for(int j=0;j<nval;j++)
    for(int i=0;i<nval;i++)
      {
	Tk=current_state[i][j];
	if(Tk!=0 && goal_state[1][Tk]==j)//proceed only if in the same column
	  {
	    for(int k=i;k<nval;k++)
	      {
		//cout<<"here"<<endl;
		Tj=current_state[k][j];
		if(Tj!=0 && goal_state[1][Tj]==j && (goal_state[0][Tk]>goal_state[0][Tj]))
		  {
		    //cout<<"Hulllllo"<<endl;
		    man_dist=0;
		    if(i > goal_state[0][Tk])
		      man_dist += i - goal_state[0][Tk];
		    else
		      man_dist += goal_state[0][Tk] - i;			
	
		    if(k > goal_state[0][Tj])
		      man_dist += k - goal_state[0][Tj];
		    else
		      man_dist += goal_state[0][Tj] - k;
	 
		    conf+=man_dist+2;
		    //cout<<Tj<<" "<<Tk<<endl;
		  }
	      }
	  }
      }
  return conf;
}
/*
int npuzzle::linear_conflict()
{

  int Tj,Tk,conf=0;//no.of conflicting pairs
  int man_dist=0;
  //traversing rowwise
  for(int i=0;i<nval;i++)
    for(int j=0;j<nval;j++)
      {
	Tk=current_state[i][j];
	if(Tk!=0 && goal_state[0][Tk]==i)//proceed only if in the same row
	  {
	    for(int k=j;k<nval;k++)
	      {
		Tj=current_state[i][k];
		if(Tj!=0 && goal_state[0][Tj]==i && (goal_state[1][Tk]>goal_state[1][Tj]))
		  { 
		    man_dist=0;
		    if(j > goal_state[1][Tk])
		      man_dist += j - goal_state[1][Tk];
		    else
		      man_dist += goal_state[1][Tk] - j;

		    if(k > goal_state[1][Tj])
		      man_dist += k - goal_state[1][Tj];
		    else
		      man_dist += goal_state[1][Tj] - k;

		    conf+=man_dist+2;

	//	    cout<<Tj<<" "<<Tk<<endl;
		  }
	      }
	  }
      }

  //traversing columnwise
  for(int j=0;j<nval;j++)
    for(int i=0;i<nval;i++)
      {
	Tk=current_state[i][j];
	if(Tk!=0 && goal_state[0][Tk]==j)//proceed only if in the same column
	  {
	    for(int k=i;k<nval;k++)
	      {
		Tj=current_state[k][j];
		if(Tj!=0 && goal_state[1][Tj]==j && (goal_state[0][Tk]>goal_state[0][Tj]))
		  {
		    man_dist=0;
		    if(i > goal_state[0][Tk])
		      man_dist += i - goal_state[0][Tk];
		    else
		      man_dist += goal_state[0][Tk] - i;			
	
		    if(k > goal_state[0][Tj])
		      man_dist += k - goal_state[0][Tj];
		    else
		      man_dist += goal_state[0][Tj] - k;
	 
		    conf+=man_dist+2;
	//	    cout<<Tj<<" "<<Tk<<endl;
      }
	      }
	  }
      }
  return conf;
}

*/
void npuzzle::calcf()
{
  f_val=g_val+h_val;
}

void npuzzle::assign_h(int h)
{
  h_val=h;
}


void npuzzle::assign_g(int g)
{
  g_val=g;
}

//Defining for the min priority queue's compare operator

struct Compare_elmnt : public std::binary_function<npuzzle*, npuzzle*, bool>
{
  bool operator()(const npuzzle *lhs, const npuzzle *rhs) const
  {
    return lhs->f_value() > rhs->f_value();
  }

};

void npuzzle::copy(npuzzle *y)
{

  for(int i=0;i<nval;i++)
    for(int j=0;j<nval;j++)
    {
      initial_state[i][j]=y->initial_state[i][j];// stores intial state in matrix form
      current_state[i][j]=y->current_state[i][j];// stores current state in matrix form
      goal_state_matrix[i][j]=y->goal_state_matrix[i][j];
    }
  current_blank[0]=y->current_blank[0]; // current position of the blank column
  current_blank[1]=y->current_blank[1];

  
  nval=y->nval; // stores the value of N
  prnt=y->prnt;
  f_val=y->f_val;
  h_val=y->h_val;
  g_val=y->g_val;
  for (int i=0;i<nval*nval;i++)
    {
      goal_state[0][i]=y->goal_state[0][i];//// stores the location of each blocks, goal_state[0][i] => row of the number i in goal, goal_state[1][i]=> column of number i in goal

      goal_state[1][i]=y->goal_state[1][i];
    }


}

bool npuzzle::state_cmp(npuzzle *y)//compare if the two pointers have the same current_state
{
  for(int i=0;i<nval;i++)
    for(int j=0;j<nval;j++)
      if(current_state[i][j]!=y->current_state[i][j])
	{return false;}

  return true;


}


int npuzzle::A_star1()
{

  priority_queue < npuzzle*, vector<npuzzle*>, Compare_elmnt > open_list; 
  vector <npuzzle*> closed_list;
  int step_count=0;
  int f,g=0,h,f_goal,v_i=0;//vector_iterator
  int flag[4];
  int goal=0;
  h=this->maxswaphrts();
  //calc g,h
  f_goal=-1;
  g_val=g;
  h_val=h;
  f=g+h;
  calcf();
  npuzzle x(nval);
  x.copy(this);
  //enter it into the priority queue

  open_list.push(&x);
  int closed_list_flag=0;

  
  npuzzle *y;
  int goal_flag;
  while(!open_list.empty())
    {
      npuzzle *y1=new npuzzle(nval);
      npuzzle *x1=new npuzzle(nval);
      npuzzle *x2=new npuzzle(nval);
      npuzzle *x3=new npuzzle(nval);
      npuzzle *x4=new npuzzle(nval);
      goal_flag=0;
  //    cout<<"While Loop"<<open_list.top()->f_val<<endl;
      y= open_list.top();
      y1->copy(y);
    //  y1->display_current_state();
      //check if y not already on closed list
      closed_list_flag=0;
      
      for(int k=0;k<v_i;k++)
	{
	  if(closed_list_flag==0)
	    {
   //     closed_list[k]->display_current_state();
	      if(y1->state_cmp(closed_list[k]))
		{
		  closed_list_flag=1;

		 break;

		}
	    }
	}
      //pushing y on the closed list
      if(closed_list_flag==0)
	{
	  
	  v_i++;
	  y1->copy(y);
	  closed_list.push_back(y1);
	  x1->copy(y);
	  x1->prnt=closed_list[v_i-1];
	  //check if this is the goal node
	//  cout<<"goal test";
	 
	//  cout<<"goal test:"<<y1->goal_test()<<endl;
	  if(y1->goal_test())
	    {
	      
	 //     cout<<"Goal test success"<<endl;
	 //     y1->display_current_state();
	      if(f_goal==-1)//1st goal node
		{
		  f_goal=y1->f_val;
		  goal=v_i-1;
	       
		}
	      else if(x1->f_val<f_goal)
		{
		  f_goal=y1->f_val;
		  goal=v_i-1;
		}
	      else
		{
		  
		  //no use treading this path so simply add in the closed list with parent...actually need not add also to save place
		}
	      goal_flag=1;
	      break;
	    }
	  for(int i=0;i<4;i++)
	    flag[i]=0;
       
	  //enter the next 4 elements into the open_list only if the flag is 1 and if not goal
      
	  //pop the open list explore it only if its not in the closed list  and push it into the closed list with a parent pointer
	  //ADD x1 to closed list with its parent and only if flag=1
	  if(goal_flag!=1)
	    {
	      x2->copy(y);
	      
	      x3->copy(y);
	      x4->copy(y);
	      
	       x2->prnt=closed_list[v_i-1];
	      x3->prnt=closed_list[v_i-1];
	      x4->prnt=closed_list[v_i-1];
	      

	      flag[0]=x1->move(1);//move from up to blank
	      flag[1]=x2->move(2);//move from down to blank
	      flag[2]=x3->move(3);// move from left to blank
	      flag[3]=x4->move(4);//move from right to blank

	      x1->h_val=x1->maxswaphrts();
	      x2->h_val=x2->maxswaphrts();
	      x3->h_val=x3->maxswaphrts();
	      x4->h_val=x4->maxswaphrts();

	      x1->g_val=x1->g_val+1;
	      x2->g_val=x2->g_val+1;
	      x3->g_val=x3->g_val+1;
	      x4->g_val=x4->g_val+1;

	      x1->calcf();
	      x2->calcf();
	      x3->calcf();
	      x4->calcf();

      
	    
	      open_list.pop();
	      if(flag[0]==1)
		{
		  open_list.push(x1);
		  step_count++;
		}
	      if(flag[1]==1)
		{
		  open_list.push(x2);
		  step_count++;
		}
	      if(flag[2]==1)
		{
		  open_list.push(x3);
		  step_count++;
		}

	      if(flag[3]==1)
		{
		  open_list.push(x4);
		  step_count++;
		}
	    }
	  
	}
      else
	{
	  open_list.pop();
	  
	}
      
      }

//  cout<<"Final Goal State at ";
//  cout<<goal<<endl<<endl;
//  closed_list[goal]->display_current_state();


//  cout<<"Final closed list"<<endl;
  for(int i=0;i<closed_list.size();i++)
//    closed_list[i]->display_current_state();
  
  
  return step_count;

}

int npuzzle::A_star2()
{

  priority_queue < npuzzle*, vector<npuzzle*>, Compare_elmnt > open_list; 
  vector <npuzzle*> closed_list;
  int step_count=0;
  int f,g=0,h,f_goal,v_i=0;//vector_iterator
  int flag[4];
  int goal=0;
  h=this->manhattan_distance();
  //calc g,h
  f_goal=-1;
  g_val=g;
  h_val=h;
  f=g+h;
  calcf();
  npuzzle x(nval);
  x.copy(this);
  //enter it into the priority queue

  open_list.push(&x);
  int closed_list_flag=0;

  
  npuzzle *y;
  int goal_flag;
  while(!open_list.empty())
    {
      npuzzle *y1=new npuzzle(nval);
      npuzzle *x1=new npuzzle(nval);
      npuzzle *x2=new npuzzle(nval);
      npuzzle *x3=new npuzzle(nval);
      npuzzle *x4=new npuzzle(nval);
      goal_flag=0;
   //   cout<<"While Loop"<<open_list.top()->f_val<<endl;
      y= open_list.top();
      y1->copy(y);
  //    y1->display_current_state();
      //check if y not already on closed list
      closed_list_flag=0;
      
      for(int k=0;k<v_i;k++)
  {
    if(closed_list_flag==0)
      {
  //      closed_list[k]->display_current_state();
        if(y1->state_cmp(closed_list[k]))
    {
      closed_list_flag=1;

     break;

    }
      }
  }
      //pushing y on the closed list
      if(closed_list_flag==0)
  {
    
    v_i++;
    y1->copy(y);
    closed_list.push_back(y1);
    x1->copy(y);
    x1->prnt=closed_list[v_i-1];
    //check if this is the goal node
  //  cout<<"goal test";
   
  //  cout<<"goal test:"<<y1->goal_test()<<endl;
    if(y1->goal_test())
      {
        
  //      cout<<"Goal test success"<<endl;
  //      y1->display_current_state();
        if(f_goal==-1)//1st goal node
    {
      f_goal=y1->f_val;
      goal=v_i-1;
         
    }
        else if(x1->f_val<f_goal)
    {
      f_goal=y1->f_val;
      goal=v_i-1;
    }
        else
    {
      
      //no use treading this path so simply add in the closed list with parent...actually need not add also to save place
    }
        goal_flag=1;
        break;
      }
    for(int i=0;i<4;i++)
      flag[i]=0;
       
    //enter the next 4 elements into the open_list only if the flag is 1 and if not goal
      
    //pop the open list explore it only if its not in the closed list  and push it into the closed list with a parent pointer
    //ADD x1 to closed list with its parent and only if flag=1
    if(goal_flag!=1)
      {
        x2->copy(y);
        
        x3->copy(y);
        x4->copy(y);
        
         x2->prnt=closed_list[v_i-1];
        x3->prnt=closed_list[v_i-1];
        x4->prnt=closed_list[v_i-1];
        

        flag[0]=x1->move(1);//move from up to blank
        flag[1]=x2->move(2);//move from down to blank
        flag[2]=x3->move(3);// move from left to blank
        flag[3]=x4->move(4);//move from right to blank

        x1->h_val=x1->manhattan_distance();
        x2->h_val=x2->manhattan_distance();
        x3->h_val=x3->manhattan_distance();
        x4->h_val=x4->manhattan_distance();

        x1->g_val=x1->g_val+1;
        x2->g_val=x2->g_val+1;
        x3->g_val=x3->g_val+1;
        x4->g_val=x4->g_val+1;

        x1->calcf();
        x2->calcf();
        x3->calcf();
        x4->calcf();

      
      
        open_list.pop();
        if(flag[0]==1)
    {
      open_list.push(x1);
      step_count++;
    }
        if(flag[1]==1)
    {
      open_list.push(x2);
      step_count++;
    }
        if(flag[2]==1)
    {
      open_list.push(x3);
      step_count++;
    }

        if(flag[3]==1)
    {
      open_list.push(x4);
      step_count++;
    }
      }
    
  }
      else
  {
    open_list.pop();
    
  }
      
      }

//  cout<<"Final Goal State at ";
  cout<<goal<<endl<<endl;
//  closed_list[goal]->display_current_state();


//  cout<<"Final closed list"<<endl;
  for(int i=0;i<closed_list.size();i++)
//    closed_list[i]->display_current_state();
  
  
  return step_count;

}

int npuzzle::A_star3()
{

  priority_queue < npuzzle*, vector<npuzzle*>, Compare_elmnt > open_list; 
  vector <npuzzle*> closed_list;
  int step_count=0;
  int f,g=0,h,f_goal,v_i=0;//vector_iterator
  int flag[4];
  int goal=0;
  h=this->tiles_out_of_pos();
  //calc g,h
  f_goal=-1;
  g_val=g;
  h_val=h;
  f=g+h;
  calcf();
  npuzzle x(nval);
  x.copy(this);
  //enter it into the priority queue

  open_list.push(&x);
  int closed_list_flag=0;

  
  npuzzle *y;
  int goal_flag;
  while(!open_list.empty())
    {
      npuzzle *y1=new npuzzle(nval);
      npuzzle *x1=new npuzzle(nval);
      npuzzle *x2=new npuzzle(nval);
      npuzzle *x3=new npuzzle(nval);
      npuzzle *x4=new npuzzle(nval);
      goal_flag=0;
 //     cout<<"While Loop"<<open_list.top()->f_val<<endl;
      y= open_list.top();
      y1->copy(y);
  //    y1->display_current_state();
      //check if y not already on closed list
      closed_list_flag=0;
      
      for(int k=0;k<v_i;k++)
  {
    if(closed_list_flag==0)
      {
  //      closed_list[k]->display_current_state();
        if(y1->state_cmp(closed_list[k]))
    {
      closed_list_flag=1;

     break;

    }
      }
  }
      //pushing y on the closed list
      if(closed_list_flag==0)
  {
    
    v_i++;
    y1->copy(y);
    closed_list.push_back(y1);
    x1->copy(y);
    x1->prnt=closed_list[v_i-1];
    //check if this is the goal node
  //  cout<<"goal test";
   
  //  cout<<"goal test:"<<y1->goal_test()<<endl;
    if(y1->goal_test())
      {
        
  //      cout<<"Goal test success"<<endl;
    //    y1->display_current_state();
        if(f_goal==-1)//1st goal node
    {
      f_goal=y1->f_val;
      goal=v_i-1;
         
    }
        else if(x1->f_val<f_goal)
    {
      f_goal=y1->f_val;
      goal=v_i-1;
    }
        else
    {
      
      //no use treading this path so simply add in the closed list with parent...actually need not add also to save place
    }
        goal_flag=1;
        break;
      }
    for(int i=0;i<4;i++)
      flag[i]=0;
       
    //enter the next 4 elements into the open_list only if the flag is 1 and if not goal
      
    //pop the open list explore it only if its not in the closed list  and push it into the closed list with a parent pointer
    //ADD x1 to closed list with its parent and only if flag=1
    if(goal_flag!=1)
      {
        x2->copy(y);
        
        x3->copy(y);
        x4->copy(y);
        
         x2->prnt=closed_list[v_i-1];
        x3->prnt=closed_list[v_i-1];
        x4->prnt=closed_list[v_i-1];
        

        flag[0]=x1->move(1);//move from up to blank
        flag[1]=x2->move(2);//move from down to blank
        flag[2]=x3->move(3);// move from left to blank
        flag[3]=x4->move(4);//move from right to blank

        x1->h_val=x1->tiles_out_of_pos();
        x2->h_val=x2->tiles_out_of_pos();
        x3->h_val=x3->tiles_out_of_pos();
        x4->h_val=x4->tiles_out_of_pos();

        x1->g_val=x1->g_val+1;
        x2->g_val=x2->g_val+1;
        x3->g_val=x3->g_val+1;
        x4->g_val=x4->g_val+1;

        x1->calcf();
        x2->calcf();
        x3->calcf();
        x4->calcf();

      
      
        open_list.pop();
        if(flag[0]==1)
    {
      open_list.push(x1);
      step_count++;
    }
        if(flag[1]==1)
    {
      open_list.push(x2);
      step_count++;
    }
        if(flag[2]==1)
    {
      open_list.push(x3);
      step_count++;
    }

        if(flag[3]==1)
    {
      open_list.push(x4);
      step_count++;
    }
      }
    
  }
      else
  {
    open_list.pop();
    
  }
      
      }

//  cout<<"Final Goal State at ";
//  cout<<goal<<endl<<endl;
//  closed_list[goal]->display_current_state();


//  cout<<"Final closed list"<<endl;
  for(int i=0;i<closed_list.size();i++)
 //   closed_list[i]->display_current_state();
  
  
  return step_count;

}

int npuzzle::A_star4()
{

  priority_queue < npuzzle*, vector<npuzzle*>, Compare_elmnt > open_list; 
  vector <npuzzle*> closed_list;
  int step_count=0;
  int f,g=0,h,f_goal,v_i=0;//vector_iterator
  int flag[4];
  int goal=0;
  h=this->misplaced();
  //calc g,h
  f_goal=-1;
  g_val=g;
  h_val=h;
  f=g+h;
  calcf();
  npuzzle x(nval);
  x.copy(this);
  //enter it into the priority queue

  open_list.push(&x);
  int closed_list_flag=0;

  
  npuzzle *y;
  int goal_flag;
  while(!open_list.empty())
    {
      npuzzle *y1=new npuzzle(nval);
      npuzzle *x1=new npuzzle(nval);
      npuzzle *x2=new npuzzle(nval);
      npuzzle *x3=new npuzzle(nval);
      npuzzle *x4=new npuzzle(nval);
      goal_flag=0;
  //    cout<<"While Loop"<<open_list.top()->f_val<<endl;
      y= open_list.top();
      y1->copy(y);
   //   y1->display_current_state();
      //check if y not already on closed list
      closed_list_flag=0;
      
      for(int k=0;k<v_i;k++)
  {
    if(closed_list_flag==0)
      {
 //       closed_list[k]->display_current_state();
        if(y1->state_cmp(closed_list[k]))
    {
      closed_list_flag=1;

     break;

    }
      }
  }
      //pushing y on the closed list
      if(closed_list_flag==0)
  {
    
    v_i++;
    y1->copy(y);
    closed_list.push_back(y1);
    x1->copy(y);
    x1->prnt=closed_list[v_i-1];
    //check if this is the goal node
  //  cout<<"goal test";
   
  //  cout<<"goal test:"<<y1->goal_test()<<endl;
    if(y1->goal_test())
      {
        
   //     cout<<"Goal test success"<<endl;
  //      y1->display_current_state();
        if(f_goal==-1)//1st goal node
    {
      f_goal=y1->f_val;
      goal=v_i-1;
         
    }
        else if(x1->f_val<f_goal)
    {
      f_goal=y1->f_val;
      goal=v_i-1;
    }
        else
    {
      
      //no use treading this path so simply add in the closed list with parent...actually need not add also to save place
    }
        goal_flag=1;
        break;
      }
    for(int i=0;i<4;i++)
      flag[i]=0;
       
    //enter the next 4 elements into the open_list only if the flag is 1 and if not goal
      
    //pop the open list explore it only if its not in the closed list  and push it into the closed list with a parent pointer
    //ADD x1 to closed list with its parent and only if flag=1
    if(goal_flag!=1)
      {
        x2->copy(y);
        
        x3->copy(y);
        x4->copy(y);
        
         x2->prnt=closed_list[v_i-1];
        x3->prnt=closed_list[v_i-1];
        x4->prnt=closed_list[v_i-1];
        

        flag[0]=x1->move(1);//move from up to blank
        flag[1]=x2->move(2);//move from down to blank
        flag[2]=x3->move(3);// move from left to blank
        flag[3]=x4->move(4);//move from right to blank

        x1->h_val=x1->misplaced();
        x2->h_val=x2->misplaced();
        x3->h_val=x3->misplaced();
        x4->h_val=x4->misplaced();

        x1->g_val=x1->g_val+1;
        x2->g_val=x2->g_val+1;
        x3->g_val=x3->g_val+1;
        x4->g_val=x4->g_val+1;

        x1->calcf();
        x2->calcf();
        x3->calcf();
        x4->calcf();

      
      
        open_list.pop();
        if(flag[0]==1)
    {
      open_list.push(x1);
      step_count++;
    }
        if(flag[1]==1)
    {
      open_list.push(x2);
      step_count++;
    }
        if(flag[2]==1)
    {
      open_list.push(x3);
      step_count++;
    }

        if(flag[3]==1)
    {
      open_list.push(x4);
      step_count++;
    }
      }
    
  }
      else
  {
    open_list.pop();
    
  }
      
      }

//  cout<<"Final Goal State at ";
  cout<<goal<<endl<<endl;
//  closed_list[goal]->display_current_state();


//  cout<<"Final closed list"<<endl;
  for(int i=0;i<closed_list.size();i++)
 //   closed_list[i]->display_current_state();
  
  
  return step_count;

}

int npuzzle::A_star5()
{

  priority_queue < npuzzle*, vector<npuzzle*>, Compare_elmnt > open_list; 
  vector <npuzzle*> closed_list;
  int step_count=0;
  int f,g=0,h,f_goal,v_i=0;//vector_iterator
  int flag[4];
  int goal=0;
  h=this->linear_conflict();
  //calc g,h
  f_goal=-1;
  g_val=g;
  h_val=h;
  f=g+h;
  calcf();
  npuzzle x(nval);
  x.copy(this);
  //enter it into the priority queue

  open_list.push(&x);
  int closed_list_flag=0;

  
  npuzzle *y;
  int goal_flag;
  while(!open_list.empty())
    {
      npuzzle *y1=new npuzzle(nval);
      npuzzle *x1=new npuzzle(nval);
      npuzzle *x2=new npuzzle(nval);
      npuzzle *x3=new npuzzle(nval);
      npuzzle *x4=new npuzzle(nval);
      goal_flag=0;
 //     cout<<"While Loop"<<open_list.top()->f_val<<endl;
      y= open_list.top();
      y1->copy(y);
 //     y1->display_current_state();
      //check if y not already on closed list
      closed_list_flag=0;
      
      for(int k=0;k<v_i;k++)
  {
    if(closed_list_flag==0)
      {
 //       closed_list[k]->display_current_state();
        if(y1->state_cmp(closed_list[k]))
    {
      closed_list_flag=1;

     break;

    }
      }
  }
      //pushing y on the closed list
      if(closed_list_flag==0)
  {
    
    v_i++;
    y1->copy(y);
    closed_list.push_back(y1);
    x1->copy(y);
    x1->prnt=closed_list[v_i-1];
    //check if this is the goal node
 //   cout<<"goal test";
   
  //  cout<<"goal test:"<<y1->goal_test()<<endl;
    if(y1->goal_test())
      {
        
  //      cout<<"Goal test success"<<endl;
  //      y1->display_current_state();
        if(f_goal==-1)//1st goal node
    {
      f_goal=y1->f_val;
      goal=v_i-1;
         
    }
        else if(x1->f_val<f_goal)
    {
      f_goal=y1->f_val;
      goal=v_i-1;
    }
        else
    {
      
      //no use treading this path so simply add in the closed list with parent...actually need not add also to save place
    }
        goal_flag=1;
        break;
      }
    for(int i=0;i<4;i++)
      flag[i]=0;
       
    //enter the next 4 elements into the open_list only if the flag is 1 and if not goal
      
    //pop the open list explore it only if its not in the closed list  and push it into the closed list with a parent pointer
    //ADD x1 to closed list with its parent and only if flag=1
    if(goal_flag!=1)
      {
        x2->copy(y);
        
        x3->copy(y);
        x4->copy(y);
        
         x2->prnt=closed_list[v_i-1];
        x3->prnt=closed_list[v_i-1];
        x4->prnt=closed_list[v_i-1];
        

        flag[0]=x1->move(1);//move from up to blank
        flag[1]=x2->move(2);//move from down to blank
        flag[2]=x3->move(3);// move from left to blank
        flag[3]=x4->move(4);//move from right to blank

        x1->h_val=x1->linear_conflict();
        x2->h_val=x2->linear_conflict();
        x3->h_val=x3->linear_conflict();
        x4->h_val=x4->linear_conflict();

        x1->g_val=x1->g_val+1;
        x2->g_val=x2->g_val+1;
        x3->g_val=x3->g_val+1;
        x4->g_val=x4->g_val+1;

        x1->calcf();
        x2->calcf();
        x3->calcf();
        x4->calcf();

      
      
        open_list.pop();
        if(flag[0]==1)
    {
      open_list.push(x1);
      step_count++;
    }
        if(flag[1]==1)
    {
      open_list.push(x2);
      step_count++;
    }
        if(flag[2]==1)
    {
      open_list.push(x3);
      step_count++;
    }

        if(flag[3]==1)
    {
      open_list.push(x4);
      step_count++;
    }
      }
    
  }
      else
  {
    open_list.pop();
    
  }
      
      }

//  cout<<"Final Goal State at ";
//  cout<<goal<<endl<<endl;
//  closed_list[goal]->display_current_state();


//  cout<<"Final closed list"<<endl;
  for(int i=0;i<closed_list.size();i++)
//    closed_list[i]->display_current_state();
  
  
  return step_count;

}

bool npuzzle::solvable()
{
  //compose the 1d array for passing to count inversions
  int *Array = new int[nval*nval];
  int num = 0;
  int blankPos = -1;
  for(int i = 0; i < nval; i++)
  {
    for(int j = 0; j < nval; j++)
    {
      Array[num++] = initial_state[i][j];
      if(Array[num -1] == 0)
        blankPos = num - 1;
    }
  }
  int inversions = countInversions(Array, nval*nval);
  inversions -= blankPos;
  delete Array;
  if(nval%2 != 0)
  {
    if(inversions%2 == 0)
      return true;
    else
      return false;
  }
  else
  {
    int blankRow = current_blank[0];
    if((blankRow % 2 == 0) && (inversions % 2 != 0))
      return true;
    else if((blankRow % 2 != 0) && (inversions % 2 == 0))
      return true;
    else
      return false;
  }
}
int npuzzle::countInversions(int *Array, int size)
{
  int *temp = (int *)malloc(sizeof(int)*size);
  int inversions = mergesort(Array, temp, 0, size - 1);
  delete temp;
  return inversions;
}


int npuzzle::mergesort(int *Array, int *temp, int left, int right)
{
  int mid = 0, leftInversions = 0, rightInversions = 0, splitInversions = 0;
  if(right > left)
  {
    mid = (right + left)/2;
    leftInversions = mergesort(Array, temp, left, mid);
    rightInversions = mergesort(Array, temp, mid + 1, right);
    splitInversions = merge(Array, temp, left, mid + 1, right);
  }
  int inversions = leftInversions + rightInversions + splitInversions;
  return inversions;
}


int npuzzle::merge(int *Array, int *temp, int left, int mid, int right)
{
  int inversions = 0;
  int i = left;
  int j = mid;
  int k = left;
  while((i <= mid - 1) && (j <= right))
  {
    if(Array[i] <= Array[j])
    {
      temp[k] = Array[i];
      k++;
      i++;
    }
    else
    {
      inversions += mid - i;
      temp[k] = Array[j];
      k++;
      j++;
    }
  }
  while(i <= mid - 1)
  {
    temp[k] = Array[i];
    k++;
    i++;
  }
  while(j <= right)
  {
    temp[k] = Array[j];
    k++;
    j++;
  }
  for(i = left; i <= right; i++)
    Array[i] = temp[i];
  return inversions;
}

int main()
{
  int n,val,j,i;
  int **rslt; 
  /*cout<<"Enter the size of n:";
    cin>>n;*/
  n=3;
  rslt=new int*[n];
  for(i=0;i<n;i++)
    {
      rslt[i]=new int[n];
    }
  /*cout <<"Enter the goal_state matrix(0 for blank):";
    for (i=0;i<n;i++)
    {
    cout<<"Enter the "<<n<<" elements of "<<i+1<<"th :";
    for (j=0;j<n;j++)
    {
    cin>>val;
    rslt[i][j]=val;
    }


    }*/
      int cnt = 1;
  npuzzle puzzle(n);
  for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
    {
      rslt[i][j] = cnt;
      cnt++;
    }

  rslt[n-1][n-1] = 0;
      
  /*npuzzle puzzle(n);
  rslt[0][0]=1;
  rslt[0][1]=2;
  rslt[0][2]=3;

  rslt[1][0]=4;
  rslt[1][1]=5;
  rslt[1][2]=6;

  rslt[2][0]=7;
  rslt[2][1]=8;
  rslt[2][2]=0;*/




	

  puzzle.build_goal_state(rslt);
  /*cout <<"Enter the current matrix(0 for blank):";
    for (i=0;i<n;i++)
    {
    cout<<"Enter the "<<n<<" elements of "<<i+1<<"th :";
    for (j=0;j<n;j++)
    {
    cin>>val;
    rslt[i][j]=val;
    }


    }*/
  /*rslt[0][0]=1;
  rslt[0][1]=0;
  rslt[0][2]=3;

  rslt[1][0]=5;
  rslt[1][1]=2;
  rslt[1][2]=6;

  rslt[2][0]=4;
  rslt[2][1]=7;
  rslt[2][2]=8;*/

  int arr[9] = {0};
  for(int i = 0; i < n*n; i++)
  arr[i] = i;
  int z = sizeof(arr)/ sizeof(arr[0]);
  srand ( time(NULL) );

   // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = z-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);

        // Swap arr[i] with the element at random index

        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

  cnt = 0;
  for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
    {
      rslt[i][j] = arr[cnt];
      cnt++;
    }
  

	

  puzzle.build_initial_state(rslt);
  /*cout<<"goal state 1"<<endl;
  puzzle.display_goal_state(1);
  cout<<"goal state 2"<<endl;
  puzzle.display_goal_state(2);
  cout<<"initial state "<<endl;
  puzzle.display_initial_state();
  cout<<"current state"<<endl;
  puzzle.display_current_state();
  
	
  cout<<endl<<puzzle.maxswaphrts()<<endl;*/

 // puzzle.display_current_state();
 // cout<<"Goal State"<<endl;
 // puzzle.display_goal_state(2);
  int solvable_flag=puzzle.solvable();
  cout<<"Solvable:"<<solvable_flag<<endl;

  int steps=0;

  vector<int> stepcount;
  if(solvable_flag==1)
  {
    
    steps=puzzle.A_star1();
    stepcount.push_back(steps);
    cout<<"max swap"<<steps<<endl<<endl;

    steps=puzzle.A_star2();
    stepcount.push_back(steps);
    cout<<"man"<<steps<<endl<<endl;

    steps=puzzle.A_star3();
    stepcount.push_back(steps);
    cout<<"tiles"<<steps<<endl<<endl;
    
    steps=puzzle.A_star4();
    stepcount.push_back(steps);
    cout<<"misplaced"<<steps<<endl<<endl;
    
    cout<<"Linear"<<endl;
    steps=puzzle.A_star5();
    stepcount.push_back(steps);
    cout<<steps<<endl;
  }  
  else
    cout<<"Not solvable"<<endl;

  for(int i = 0; i < stepcount.size(); i++)
    cout << stepcount[i] << " ";

  
  return 0;

}


