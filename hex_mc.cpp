#include <bits/stdc++.h>
using namespace std;
/* 	This is a hex game implementation where computer makes moves based on Monte Carlo simulations and you can decide your moves.
	Blue's aim is to make a path between left and right.
	Red's aim is to make a path between top and bottom.*/
enum class colors:short{BLUE, RED, BLANK};//defining categories of hexagons
ostream& operator<<( ostream& out, const colors& color ){// for output representation of hexagon
    switch(color){
        case colors::BLUE:	cout<<"B";
        					break;
        case colors::RED:	cout<<"R";
							break;
        case colors::BLANK:	cout<<".";
        					break;
    }
}
// made disjoint class for disjoint sets data structure
// this helps me to check if we have found a winner.
// we do this using this class to make connected components for the graph
// whenever we get nodes from opposite sides(corresponding to player or computer) in a single component, we get a winner
class disjoint{
private:
    vector<int> parent, rank;
public:
    disjoint(int n){//takes size as input
        parent.resize(n);
        rank.resize(n);
    }
    void make_set(int v) {
        parent[v] = v;
        rank[v] = 0;
    }
    int find_set(int v) {// returns the representative element for the component
        if (v == parent[v])
            return v;
        return parent[v] = find_set(parent[v]);
    }
    void union_sets(int a, int b) {//takes union of the sets of a and b
        a = find_set(a);
        b = find_set(b);
        if (a != b) {
            if (rank[a] < rank[b])
                swap(a, b);
            parent[b] = a;
            if (rank[a] == rank[b])
                rank[a]++;
        }
    }
};

struct hexagon{//structure for node(an hexagon)
    int x,y; // coordinates (starting from 0)
    colors color;// color of the hexagon - blue,red or blank
    vector<pair<int,int>> adj;// adjacent nodes
};
class hex_board{
private:
    int size=7;// size*size hex board
    vector<vector<hexagon>> board;// all the nodes
    vector<pair<int,int>> make_adj(int i,int j){// function for returning the adjacent nodes coordinates
        vector<pair<int,int>> vec;// stores the vector to be returned
        if(i==0){// now these are all the cases including corner, edges and internal nodes
            if(j==0)
                vec = {{0,1},{1,0}};
            else if(j==size-1)
                vec = {{0,size-2},{1,size-1},{1,size-2}};
            else
                vec = {{0,j-1},{0,j+1},{1,j-1},{1,j}};
        }
        else if(i==size-1){
            if(j==0)
                vec = {{size-1,1},{size-2,0},{size-2,1}};
            else if(j==size-1)
                vec = {{size-1,size-2},{size-2,size-1}};
            else
                vec = {{size-1,j-1},{size-1,j+1},{size-2,j},{size-2,j+1}};
        }
        else{
            if(j==0)
                vec = {{i-1,0},{i-1,1},{i,1},{i+1,0}};
            else if(j==size-1)
                vec = {{i-1,size-1},{i,size-2},{i+1,size-1},{i+1,size-2}};
            else
                vec = {{i-1,j},{i-1,j+1},{i,j-1},{i,j+1},{i+1,j},{i+1,j-1}};
        }
        return vec;
    }
public:
    colors player_c = colors::RED;// color of the player
    colors comp_c = colors::RED;// color of the computer
    set<pair<int,int>> available_moves;//storing the currently available moves
    hex_board(int n){//constructor to initialize all the nodes with blank color and also calculate their edges
    	board.resize(n,vector<hexagon>(n));
    	size=n;
		for(int i=0;i<size;i++){
			for(int j=0;j<size;j++){
				board[i][j].x = i;
				board[i][j].y = j;
				board[i][j].color = colors::BLANK;
				board[i][j].adj = make_adj(i,j);
				available_moves.insert(make_pair(i,j));//initially all moves are available
			}
		}
    }
    void print(){// to print current state of the hex board
    	cout<<"Blue - left right and Red - up down"<<endl;
        for(int i=0;i<size;i++){
            for(int j=0;j<2*i;j++)
                cout<<" ";
            for(int j=0;j<size;j++){
                if(j!=size-1)
                    cout<<board[i][j].color<<" - ";
                else
                    cout<<board[i][j].color;
            }
            if(i!=size-1){
                cout<<endl;
                for(int j=0;j<=2*i;j++)
                    cout<<" ";
                for(int j=0;j<size;j++){
                    if(j!=size-1)
                        cout<<"\\ / ";
                    else
                        cout<<"\\";
                }
                cout<<endl;
            }
        }
        cout<<endl;
    }
    bool make_move(int x, int y, colors c){// function to make move for a particular color; returns false if move is unsuccessful else true
        if((x<0)||(x>=size)||(y<0)||(y>=size)||(board[x][y].color!=colors::BLANK))//invalid move
            return false;
        board[x][y].color=c;
        cout<<c<<" moves "<<x<<" "<<y<<endl;
        return true;
    }

    /*	Monte Carlo Strategy
    	Iterate over all the available moves
    		make that move
    		perform simulations with completely random moves many times
    		maintain a best move which captures the move with highest win ratio in these random move simulations
		Use this best move
    */
    pair<int,int> comp_move(){// for computer's move
		srand(time(0));
		int num_trials = 1000;// number of simulations for each move
		pair<int,int> best_move(0,0);//maintains best move
		float best_move_value = 0.0;//maintains best move's win ratio
		for(auto a_move : available_moves){//iterate over all available moves
			int count_win=0;//maintains number of wins out of number of trials
			for(int i=0;i<num_trials;i++){
				//For random simulations, a board copy is made, the board is shuffled
				//Then, all the blank hexagons are filled with red and blue alternatively without checking the winner
				//Then at last, the winner is checked, since once someone has won the game, the winner cant change (No draws possible in hex)

				vector<hexagon> board_copy;//makes a 1-D copy of board
				for(int j=0;j<size;j++){
					board_copy.insert(board_copy.end(),board[j].begin(),board[j].end());
				}
				board_copy[hash(a_move.first,a_move.second)].color = comp_c;//make the move for which the simulations will be held
				int indices[size*size];//firstly stores indices and then is shuffled to get order of iteration
				for(int j=0;j<size*size;j++)
					indices[j]=j;
				random_shuffle(indices,indices+size*size);
				disjoint set_red(size*size);//for checking if red id winner after all the blanks cells have been filled, using disjoint sets
				vector<int> up,down;//for storing the upper and lower boundary nodes which are red colored
        		colors flag=player_c;//this flag keeps on switching after assigning some blank hexagon some color
                for(auto ind : indices) {//updates the already red colored nodes in disjoint set
                    hexagon cur = board_copy[ind];
                    if(cur.color == colors::RED)
                        set_red.make_set(hash(cur.x, cur.y));
                }
				for(auto ind : indices){//iterating over the random ordered indices, to update the board copy, and update the disjoint set
					hexagon& hexa = board_copy[ind];
					if(hexa.color==colors::BLANK){//if blank then assign color and switch flag
						hexa.color = flag;
						if(flag == colors::RED)//if the node is colored red, then update disjoint set
                            set_red.make_set(hash(hexa.x, hexa.y));
                        flag = (flag==colors::RED)?colors::BLUE:colors::RED;

					}
					if(hexa.color==colors::RED){//if color is red then update the disjoint set to check if red is winner
						if(hexa.x==0)//update upper boundary nodes
            				up.push_back(hexa.y);
        				else if(hexa.x==size-1)//update lower boundary nodes
            				down.push_back(hexa.y);
						// set_red.make_set(hash(hexa.x,hexa.y));//firstly make a disjoint set for the move node
        				for(auto k:hexa.adj){// if any neighbour is red, take union of the above set with that neighbour's set
            				if(board_copy[hash(k.first,k.second)].color==colors::RED)
                				set_red.union_sets(hash(hexa.x,hexa.y),hash(k.first,k.second));
        				}
					}
				}
				bool redwin=false;//store if red is winner
				for(auto j:up){//iterate over upper and lower boundary nodes, if check if any pair of them are in same component
            		for(auto k:down){
                		if(set_red.find_set(hash(0,j))==set_red.find_set(hash(size-1,k))){
							redwin = true;
							//cout<<"red_win true with path between"<<j<<k<<endl;
							break;
                		}
            		}
            		if(redwin==true)
            			break;
				}
                if((redwin&&comp_c==colors::RED)||((!redwin) && comp_c==colors::BLUE ) )//update count_win
                	count_win++;
			}
			//cout<<"Move checked: "<<a_move.first<<" "<<a_move.second<<"\tNo of times win "<<count_win<<endl;
			if((float(count_win)/float(num_trials))>=best_move_value){//update best move and best move value
				best_move = make_pair(a_move.first,a_move.second);
				best_move_value = float(count_win)/num_trials;
			}
		}
		cout<<"Best Move Value: "<<best_move_value<<" \tBest move: "<<best_move.first<<" "<<best_move.second<<endl;
		make_move(best_move.first,best_move.second,comp_c);//make the best move for computer
        available_moves.erase(make_pair(best_move.first,best_move.second));//remove the move from available moves
        return make_pair(best_move.first,best_move.second);
    }
    int hash(int x, int y){//simple hashing used in disjoint sets
        return x*(size)+y;
    }
    bool iswinner_b(int x, int y){// checks if blue has become winner after the move(x,y)
        static vector<int> left,right;// stores the left and right boundaries nodes which are blue
        if(y==0)
            left.push_back(x);
        else if(y==size-1)
            right.push_back(x);
        static disjoint sets_b(size*size);//initializes the object with size*size values available
        sets_b.make_set(hash(x,y));//firstly make a disjoint set for the move node
        for(auto i:make_adj(x,y)){// if any neighbour is blue, take union of the above set with that neighbour's set
            if(board[i.first][i.second].color==colors::BLUE)
                sets_b.union_sets(hash(x,y),hash(i.first,i.second));
        }
        for(auto i:left)//checks if any pair of nodes from the right and left boundary respectively are in the same set
            for(auto j:right)
                if(sets_b.find_set(hash(i,0))==sets_b.find_set(hash(j,size-1)))
                    return true;
        return false;
    }
    bool iswinner_r(int x, int y){//similar function which checks if red is the winner
        static vector<int> up,down;
        if(x==0)
            up.push_back(y);
        else if(x==size-1)
            down.push_back(y);
        static disjoint sets_r(size*size);
        sets_r.make_set(hash(x,y));
        for(auto i:make_adj(x,y)){
            if(board[i.first][i.second].color==colors::RED)
                sets_r.union_sets(hash(x,y),hash(i.first,i.second));
        }
        for(auto i:up)
            for(auto j:down)
                if(sets_r.find_set(hash(0,i))==sets_r.find_set(hash(size-1,j)))
                    return true;
        return false;
    }
    bool iswinner(int x, int y, colors c){// to check if color c has become winner after the move(x,y)
        if(c==colors::BLUE)
            return iswinner_b(x,y);
        else
            return iswinner_r(x,y);
    }
};
int main()
{
	int n;
	cout<<"Enter size of the board (enter n for n*n board): ";
	cin>>n;
    hex_board myboard(n);//initialize the board
    char ch;
    while(true){//until color has been assigned to the player
        cout<<"Choose color (Red-r, Blue-b)(Blue goes first):"<<endl;
        cin>>ch;
        if(ch=='r'){
            myboard.comp_c = colors::BLUE;
            break;
        }
        else if(ch=='b'){
            myboard.player_c = colors::BLUE;
            break;
        }
        else
            cout<<"INVALID COLOR: Choose either r or b"<<endl;
    }
    if(myboard.player_c==colors::RED)//since blue moves first, if player chooses red, computer moves first
        myboard.comp_move();
    myboard.print();
    while(true){//until there is a winner
        int x,y;
        cout<<"Enter a move (coordinate;2 numbers from 0-"<<n-1<<")"<<endl;
        cin>>x>>y;
        if(!myboard.make_move(x,y,myboard.player_c)){
            cout<<"INVALID MOVE"<<endl;
            continue;
        }
        myboard.available_moves.erase(make_pair(x,y));
        myboard.print();
        if(myboard.iswinner(x,y,myboard.player_c)){//if player wins then end otherwise let computer move
            cout<<"You are the winner";
            break;
        }
        else {
            pair<int, int> comp_move = myboard.comp_move();
            myboard.print();
            if(myboard.iswinner(comp_move.first,comp_move.second,myboard.comp_c)){//checks if computer is winner
                cout<<"Computer is the winner";
                break;
            }
        }
    }
    return 0;
}
