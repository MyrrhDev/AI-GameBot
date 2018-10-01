#include "Player.hh"

#define PLAYER_NAME MPDaliLlamax



struct PLAYER_NAME : public Player {

	static Player* factory () {
		return new PLAYER_NAME;
	}
  
	const int nsX[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
	const int nsY[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
	const int heliX[4] = { 1, 0, -1, 0 };
	const int heliY[4] = { 0, 1, 0, -1 };
	
	const int helSouth = 0;
	const int helEast = 1;
	const int helNorth = 2;
	const int helWest = 3;
	
	typedef vector <int> VecInt;
	typedef vector <VecInt> Graph;
	typedef vector <Post> VectPosts;
	typedef vector <Position> VectPositions;
	typedef vector <VectPositions> mapPOS;

	
	bool canAttack(int myTeam, int id) {
		Position myPos = data(id).pos;
		for (int i = 0; i < 8; ++i) {
		  int yox = myPos.i + nsX[i];
		  int yoy = myPos.j + nsY[i];
		  if (pos_ok(yox,yoy)) {
			int soldx = which_soldier(yox,yoy);
			if (soldx and data(soldx).player != myTeam) {   
			  command_soldier(id,yox,yoy);
			  return true;	
			}
		  }
		}
		return false;
	}
	
	bool mySoldierCanMove(int myTeam, int x, int y) {
		if (what(x,y) == WATER or what(x,y) == MOUNTAIN) return false;
		if (fire_time(x,y) != 0) return false;
		
		for(int i = 0; i < 8; ++i) {
			if(fire_time(x+nsX[i],y+nsY[i]) != 0) return false;
		}
		int ids = which_soldier(x,y);
		if (ids and data(ids).player == myTeam) return false;		
		return true;
	}
	
	void soldierFindPath(int sX, int sY, int myTeam, int soldier) {
		Graph distance(MAX, VecInt(MAX, 60));
		distance[sX][sY] = 0;
		Graph visited(MAX, VecInt(MAX, -1));
		
		Position r;
		r.i = -1;
		r.j = -1;
		
		mapPOS whereFrom(MAX, VectPositions(MAX,r));

		priority_queue< pair<int, pair<int,int>>, vector< pair<int, pair<int,int>> >, greater< pair<int, pair<int,int>> > > pQ;
		
		pQ.push(make_pair(0,make_pair(sX,sY)));
		
		bool found = false;
		while (not pQ.empty() and not found) {
			int new_i = pQ.top().second.first;
			int new_j = pQ.top().second.second;
			pQ.pop();
			if (visited[new_i][new_j] == -1) {
				visited[new_i][new_j] = 0;
				int i = 0;
					while (i < 8) {
						int yox = new_i + nsX[i];
						int yoy = new_j + nsY[i];
						if (pos_ok(yox,yoy) and mySoldierCanMove(myTeam, yox, yoy) and visited[yox][yoy] == -1) {
							Position aux;
							aux.i = yox;
							aux.j = yoy;
							
							Position prx;
							prx.i = new_i;
							prx.j = new_j;
							if (post_owner(yox,yoy) != -2 and post_owner(yox,yoy) != myTeam) { 
								found = true;
								i = 8;
								while (not pQ.empty()) pQ.pop();
									distance[aux.i][aux.j] = distance[new_i][new_j] + 1;
								  
								  whereFrom[aux.i][aux.j] = prx;
								  pQ.push(make_pair(distance[aux.i][aux.j], make_pair(aux.i,aux.j)));
								
							} else {
								if (distance[new_i][new_j] + 1 < distance[aux.i][aux.j]) {
								  distance[aux.i][aux.j] = distance[new_i][new_j] + 1;
								  
								  whereFrom[aux.i][aux.j] = prx;
								  pQ.push(make_pair(distance[aux.i][aux.j], make_pair(aux.i,aux.j)));
								}
							}				
						} 
						++i;
					}
			}    
		}
	 
		if (pQ.empty()) {
			int d = random (1,8);
			command_soldier(soldier,sX+nsX[d-1],sY+nsY[d-1]);
		 return;
		} else {
			int rX = 0;
			int rY = 0;
			bool found2 = false;
			int iX = pQ.top().second.first;
			int jY = pQ.top().second.second;

		  
			while (not found2) {
			  if (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY) {
				  rX = iX;
				  rY = jY;
				  found2 = true;
			  }
			  Position temp = whereFrom[iX][jY];
			  iX = temp.i; 
			  jY = temp.j;
			}
			
			command_soldier(soldier,rX,rY);
			return;
		}
	}	
  
  
    bool canHeliGoThere(int yox, int yoy, int helOrientation) {
		int fromi;
		int toi;
		int fromj;
		int toj;
		
		if (helOrientation == helSouth) {
			fromi = 2;
		    toi = 4;
		    fromj = -2;
			toj = 2;
		} else if (helOrientation == helEast) {
			fromi = -4;
		    toi = 4;
		    fromj = 2;
			toj = 3;
		} else if (helOrientation == helWest) {
			fromi = -3;
		    toi = 3;
		    fromj = -3;
			toj = -2;
		} else {
			fromi = -4;
		    toi = -2;
		    fromj = -2;
			toj = 2;
		}
		
		for (int i = fromi; i <= toi; ++i) {
			for (int j = fromj; j <= toj; ++j) {
				int ix = yox + i;
				int jy = yoy + j;
				if (pos_ok(ix,jy)) {
					if (what(ix,jy) == MOUNTAIN or which_helicopter(ix,jy) != 0) {
						return false;
					}
				} else return false;
			}
		}
		return true;
	}
	
	
	void BFSHeliPath2(int sX, int sY, int myTeam, int& rX, int& rY, VectPositions okayPos) {
		Graph visited(MAX, VecInt(MAX, -1));
		
		Position r;
		r.i = -1;
		r.j = -1;
		
		mapPOS whereFrom(MAX, VectPositions(MAX,r));
		queue<Position> pQ;
		
		for (int m = 0; m < okayPos.size(); ++m) {
				Position temp = okayPos[m];
				int s = temp.i;
				int d = temp.j;
		  pQ.push(temp);
		     
		} 
		visited[sX][sY] = 0;
		
		
		bool found = false;
		while (not pQ.empty() and not found) {
		
			int new_i = pQ.front().i;
			int new_j = pQ.front().j;
			pQ.pop();
			if (visited[new_i][new_j] == -1) {
				visited[new_i][new_j] = 0;
				int i = 0;
			
				while (i < 4) {
				  int yox = new_i + heliX[i];
				  int yoy = new_j + heliY[i];
				  if (pos_ok(yox,yoy) and visited[yox][yoy] == -1) {
					  
					if(canMyHelicopMove(yox,yoy)) {
						Position aux;
						aux.i = yox;
						aux.j = yoy;
						
						Position prx;
						prx.i = new_i;
						prx.j = new_j;
						if (post_owner(yox,yoy) != -2 and (post_owner(yox,yoy) != -1 and post_owner(yox,yoy) != myTeam)) { 
							found = true;
							i = 4;
							while (not pQ.empty()) pQ.pop();							
							whereFrom[aux.i][aux.j] = prx;
							pQ.push(aux);
						}
						else{
							whereFrom[aux.i][aux.j] = prx;
							pQ.push(aux);
						}
					}
				} 
				++i;
			   }
			}    
		}
	
		if (pQ.empty()) {
			
		 return;
		} else {
			int iX = pQ.front().i;
			int jY = pQ.front().j;
			bool found = false;
		  
			while (not found) {
			  if ((whereFrom[iX][jY].i == sX+1 and whereFrom[iX][jY].j == sY) or (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY+1) or (whereFrom[iX][jY].i == sX-1 and whereFrom[iX][jY].j == sY) or (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY-1)) {
				if (whereFrom[iX][jY].i == sX+1 and whereFrom[iX][jY].j == sY) {
					rX = sX+1;
					rY = sY;
					found = true;	
				} else if (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY+1) {
					rX = sX;
					rY = sY+1;
					found = true;
				} else if (whereFrom[iX][jY].i == sX-1 and whereFrom[iX][jY].j == sY) {
					rX = sX-1;
					rY = sY;
					found = true;
				} else if (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY-1) {
					rX = sX;
					rY = sY-1;
					found = true;
				}
					  
			  }
			  Position temp = whereFrom[iX][jY];
			  iX = temp.i; 
			  jY = temp.j;
			}
		return;
		}	  
	}
	
	void BFSHeliPathTT(int sX, int sY, int oX, int oY, int myTeam, int& rX, int& rY, VectPositions okayPos) {
		Graph visited(MAX, VecInt(MAX, -1));
		Position r;
		r.i = -1;
		r.j = -1;
		
		mapPOS whereFrom(MAX, VectPositions(MAX,r));
		queue<Position> pQ;
		
		for (int m = 0; m < okayPos.size(); ++m) {
				Position temp = okayPos[m];
				int s = temp.i;
				int d = temp.j;
		  pQ.push(temp);
		     
		} 
		visited[sX][sY] = 0;
		bool found = false;
		
		while (not pQ.empty() and not found) {
			int new_i = pQ.front().i;
			int new_j = pQ.front().j;
			pQ.pop();
			if (visited[new_i][new_j] == -1) {
				visited[new_i][new_j] = 0;
				int i = 0;
			
				while (i < 4) {
					int yox = new_i + heliX[i];
					int yoy = new_j + heliY[i];
					if (pos_ok(yox,yoy) and visited[yox][yoy] == -1) {
						if(canMyHelicopMove(yox,yoy)) {
							Position aux;
							aux.i = yox;
							aux.j = yoy;
							
							Position prx;
							prx.i = new_i;
							prx.j = new_j;
							if (yox == oX and yoy == oY) { 
								found = true;
								i = 4;
								while (not pQ.empty()) pQ.pop();							
								whereFrom[aux.i][aux.j] = prx;
								pQ.push(aux);
							} else {
								whereFrom[aux.i][aux.j] = prx;
								pQ.push(aux);
							}
						}
					} 
				++i;
			   }
			}    
		}
	
		if (pQ.empty()) {
		 return;
		} else {
			int iX = pQ.front().i;
			int jY = pQ.front().j;
			bool found = false;
			
			while (not found) {
			  if ((whereFrom[iX][jY].i == sX+1 and whereFrom[iX][jY].j == sY) or (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY+1) or (whereFrom[iX][jY].i == sX-1 and whereFrom[iX][jY].j == sY) or (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY-1)) {
				if (whereFrom[iX][jY].i == sX+1 and whereFrom[iX][jY].j == sY) {
					rX = sX+1;
					rY = sY;
					found = true;
				} else if (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY+1) {
					rX = sX;
					rY = sY+1;
					found = true;
				} else if (whereFrom[iX][jY].i == sX-1 and whereFrom[iX][jY].j == sY) {
					rX = sX-1;
					rY = sY;
					found = true;
					
				} else if (whereFrom[iX][jY].i == sX and whereFrom[iX][jY].j == sY-1) {
					rX = sX;
					rY = sY-1;
					found = true;
				}
					  
			  }
			  Position temp = whereFrom[iX][jY];
			  iX = temp.i;
			  jY = temp.j;
			}
			return;
		}	  
	}
	
	
	bool canMyParachuterFall(int x, int y) {
		if (not pos_ok(x,y)) return false;
		else {
			if (what(x,y) == WATER or what(x,y) == MOUNTAIN) return false;
			
			if (fire_time(x,y) != 0) return false;
			if (which_soldier(x,y) != 0) return false;
		}
		return true;
	}

   
	bool canMyHelicopMove(int x, int y) {
		if (what(x,y) == MOUNTAIN or which_helicopter(x,y) != 0) return false;
		return true;
	}
  
 
	bool moreOfThem(int i, int j, int myTeam) {
		int mine = 0;
		int them = 0;
		
		if(pos_ok(i,j)) {
			if((which_soldier(i,j) != 0)) {
				int soldier = which_soldier(i,j);
				if (soldier and data(soldier).player != myTeam) {
					++them;
				} else if (soldier and data(soldier).player == myTeam) ++mine;
			}
		}
		
		for (int l = -REACH; l <= REACH; ++l) {
		 for (int k = -REACH; k <= REACH; ++k) {
			int ix = i + l;
			int jy = j + k;
				
				if(pos_ok(ix,jy)) {
					if((which_soldier(ix,jy)) != 0) {
						int soldier = which_soldier(ix,jy);
						if (soldier and data(soldier).player != myTeam) {
							++them;
						} else if (soldier and data(soldier).player == myTeam) ++mine;
					}
				}
			}
		}
		
		if (them <= 3) return false;
		else return them > mine;
    }
  
  	bool parachuterAboutToDie(VecInt parachuters) {
	   if(not parachuters.empty()) {
		   for (int i = 0; i < parachuters.size(); ++i) {
			   if (parachuters[i] == 0) return true;
		   }
	   }
	   return false;
   }
   
   bool checkIfWater(int x, int y) {
	   int w = 0;
	   for (int i = x-3; i <= x-2; ++i) {
			for (int j = y-7; j <= y+7; ++j) {
				if (what(i,j) == WATER) ++w;
			}
		}
		if(w > 22) return true; //26
		else return false;
   }
   
   bool checkIfIsland(int x, int y) {   
	   int w = 0;
	   	   for (int i = x; i >= x-1; --i) {		   
			for (int j = y-4; j <= y+3; ++j) {			
				if (what(i,j) == WATER) ++w;
			}
			
		}
		if(w >= 13) return true;
		
	return false;
   }
   
   int findMapStrategy(VectPosts thePosts) {
	   int i = 0;
	   
	   while (i < thePosts.size()) {
		   if(thePosts[i].pos.i == 49 and thePosts[i].pos.j == 37) return 4;		   
		   if(thePosts[i].pos.i == 18 and thePosts[i].pos.j == 5) return 5; //MiddleLine
		   if(thePosts[i].pos.i == 26 and thePosts[i].pos.j == 21) return 6; //MiddleLine?
		   if(thePosts[i].pos.i == 2 and thePosts[i].pos.j == 56) return 4; //SqDancev2
		   if(thePosts[i].pos.i == 20 and thePosts[i].pos.j == 23) return 2; //SqDance
		   if(thePosts[i].pos.i == 10 and thePosts[i].pos.j == 5) return 2; //SqDance
		   if(thePosts[i].pos.i == 10 and thePosts[i].pos.j == 14) return 2; //SqDance
		   if(thePosts[i].pos.i == 6 and thePosts[i].pos.j == 22) return 2; //SqDance
		   if(thePosts[i].pos.i == 26 and thePosts[i].pos.j == 27) return 3; //line
		   if(thePosts[i].pos.i == 20 and thePosts[i].pos.j == 15) return 6; //linev2
		   if(thePosts[i].pos.i == 35 and thePosts[i].pos.j == 23) return 1;
		   if(thePosts[i].pos.i == 4 and thePosts[i].pos.j == 24) return 0;
		   if(thePosts[i].pos.i == 51 and thePosts[i].pos.j == 42) return 2;
		   if(thePosts[i].pos.i == 18 and thePosts[i].pos.j == 17) return 2;
		   if(thePosts[i].pos.i == 26 and thePosts[i].pos.j == 7) return 2;//Square Da
		   if(thePosts[i].pos.i == 12 and thePosts[i].pos.j == 27) return 7;
		   
		   ++i;
		}
		return -1;
   }
   
  
	void shouldHeliNapalm(bool& done, Data heli, int myTeam) {
		if (heli.napalm == 0 and not parachuterAboutToDie(heli.parachuters)) {			
			if (moreOfThem(heli.pos.i, heli.pos.j,myTeam) ) {
				command_helicopter(heli.id,NAPALM);
				done = true;
			}
		}
    }
   
	void heliShouldThrowChuter(Data heli, int& thrown, int i, bool& canChuters) {
		int numChuters = heli.parachuters.size();
							
		for (int l = -REACH; l <= REACH; ++l) {
			for (int k = -REACH; k <= REACH; ++k) {
				int ix = heli.pos.i + l;
				int jy = heli.pos.j + k;
					if(canMyParachuterFall(ix,jy)) {
						command_parachuter(ix,jy);
						++thrown;
						--numChuters;
							
					}
						
					if (numChuters == 0 or thrown == 3) {
						l = REACH+4;
						k = REACH+4;
					}
				}
			}
		if (i == 0) {
			if (thrown == 3) canChuters = false;
		} else if (i == 1) if (thrown == 4) canChuters = false;
   }
   
	void findHeliStrategy(bool& oneStep, int& resX, int& resY, Data heli, int myTeam, VectPositions okayPos, int i, VectPosts thePosts) {
		bool mSix = false;
		bool mFteen = false;
		int rX = 0;
		int rY = 0;
		int objX = 0;
		int objY = 0;
		int order = -1;
		
		if (i == 0) {
			if (checkIfWater(30,30)) {
				objX = 19;
				objY = 30;
				if(round() > 130) {
					objX = 42;
					objY = 30;
				}
			} else if (checkIfIsland(30,30)) {
				objX = 56;
				objY = 30;
				
				if(round() > 110) {
					objX = 5;
					objY = 30;
				}
			} else {
				order = findMapStrategy(thePosts);
				
				if(order == 1) {
					objX = 48;
					objY = 30;
					
					if(round() > 110) {
						objX = 12;
						objY = 30;
					}
				} else if (order == 0) {
					objX = 56;
					objY = 30;
				
				
					if(round() > 100) {
						objX = 4;
						objY = 30;
					}
				} else if (order == 2) {
					objX = 5;
					objY = 7;
					
					if ((round() > 28 and round() < 60)) {
						objX = 53;
						objY = 7;
					} else if ((round() > 59 and round() < 87) or (round() > 150)) {
						objX = 53;
						objY = 52;
					} else if (round() > 88 and round() < 116) {
						objX = 5;
						objY = 52;
					}
				} else if (order == 3) { 
					if(round() < 45 or round() > 108) {
						objX = 30;
						objY = 54;
					} else if(round() > 46) {
						objX = 53;
						objY = 30;
					} else if(round() > 70) {
						objX = 7;
						objY = 30;
					}
				} else if (order == 4) { 
					objX = 5;
					objY = 5;
					
					if((round() > 28 and round() < 60)) {
						objX = 53;
						objY = 5;
					} else if ((round() > 60 and round() < 100) or (round() > 150)) {
						objX = 53;
						objY = 53;
					} else if (round() > 101 and round() < 150) {
						objX = 5;
						objY = 53;
					}
				} else if (order == 5) { 
					objX = 6;
					objY = 30;
					
					if((round() > 28 and round() < 60)) {
						objX = 30;
						objY = 30;
					} else if ((round() > 60 and round() < 100) or (round() > 150)) {
						objX = 52;
						objY = 30;
					} else if (round() > 101 and round() < 150) {
						objX = 30;
						objY = 40;
					}
				} else if (order == 6) { 
					objX = 30;
					objY = 5;
					mSix = true;
				} else if (order == 7) { 
					objX = 47;
					objY = 30;
				
					mFteen = true;
					if(round() > 100) {
						objX = 11;
						objY = 30;
					}
				} else { //if -1
					objX = 30;
					objY = 30;
				}
			}	
					
			bool look = true;
			
			//if it's within my reach
			Position obj;
			obj.i = objX;
			obj.j = objY;
			
			
			if (heli.pos != obj) {
				for (int j = 0; j < okayPos.size(); ++j) {
					if (okayPos[j].i == objX and okayPos[j].j == objY) {
						resX = heli.pos.i - objX;
						resY = heli.pos.j - objY;
						look = false;
						oneStep = true;
					}
				}
				if (look) {
					BFSHeliPathTT(heli.pos.i, heli.pos.j, objX, objY, myTeam, rX, rY,okayPos);
					resX = heli.pos.i - rX;
					resY = heli.pos.j - rY;
				}
			}
		} else if (i == 1) {
			if (mSix) {
				objX = 30;
				objY = 53;
				BFSHeliPathTT(heli.pos.i, heli.pos.j, objX, objY, myTeam, rX, rY,okayPos);
			} else if (mFteen) {
				objX = 11;
				objY = 30;
				BFSHeliPathTT(heli.pos.i, heli.pos.j, objX, objY, myTeam, rX, rY,okayPos);
			} else {
				BFSHeliPath2(heli.pos.i, heli.pos.j, myTeam, rX, rY,okayPos);
			}
			
			resX = heli.pos.i - rX;
			resY = heli.pos.j - rY;					
		}
   }
   
	void executeHeliMove(Data heli, int resX, int resY, bool oneStep) {
		if (resX == 1 and resY == 0) { //North
			if (heli.orientation == helNorth) {
				if (oneStep) command_helicopter(heli.id,FORWARD1);
				else command_helicopter(heli.id,FORWARD2);
			} else {
				if (heli.orientation ==helEast) command_helicopter(heli.id,COUNTER_CLOCKWISE);
				else if (heli.orientation ==helWest) command_helicopter(heli.id,CLOCKWISE);
				else command_helicopter(heli.id,CLOCKWISE);
			}
		} else if (resX == 0 and resY == 1) { //West
			if (heli.orientation == helWest) {
				if (oneStep) command_helicopter(heli.id,FORWARD1);
				else command_helicopter(heli.id,FORWARD2);
			} else {
				if (heli.orientation ==helSouth) command_helicopter(heli.id,CLOCKWISE);
				else if (heli.orientation ==helNorth) command_helicopter(heli.id,COUNTER_CLOCKWISE);
				else command_helicopter(heli.id,CLOCKWISE);
			}
		} else if(resX == -1 and resY == 0) { //South
			if (heli.orientation == helSouth) {
				if (oneStep) command_helicopter(heli.id,FORWARD1);
				else command_helicopter(heli.id,FORWARD2);
			} else {
				if (heli.orientation ==helWest) command_helicopter(heli.id,COUNTER_CLOCKWISE);
				else if (heli.orientation ==helEast) command_helicopter(heli.id,CLOCKWISE);
				else command_helicopter(heli.id,CLOCKWISE);
			}
		} else if(resX == 0 and resY == -1) { //East
			if (heli.orientation == helEast) {
				if (oneStep) command_helicopter(heli.id,FORWARD1);
				else command_helicopter(heli.id,FORWARD2);
			} else {
				if (heli.orientation == helNorth) command_helicopter(heli.id,CLOCKWISE);
				else if (heli.orientation == helSouth) command_helicopter(heli.id,COUNTER_CLOCKWISE);
				else command_helicopter(heli.id,CLOCKWISE);
			}
		}
	   
	}
   
   
   void moveMyHelis(VecInt myHelis, int myTeam,VecInt mySolds,VectPosts thePosts) {
		bool canChuters = true;
		int numRestrictionSoldiers;
		
		if (round() < 173) numRestrictionSoldiers = 54;
		else numRestrictionSoldiers = 70;
		
		if (mySolds.size() >= numRestrictionSoldiers) canChuters = false;
		
		int thrown = 0;
		
		for(int i = 0; i < myHelis.size(); ++i) {
			Data heli = data(myHelis[i]);
			bool done = false;
			
			shouldHeliNapalm(done,heli,myTeam);
			
			if (canChuters and not done and not heli.parachuters.empty()) heliShouldThrowChuter(heli,thrown,i,canChuters);
			
			if(not done) {	
				int resX = 0;
				int resY = 0;
				bool oneStep = false;
				VectPositions okayPos;
				
				for (int hi = 0; hi < 4; ++hi) {
				  int ix = heli.pos.i + heliX[hi];
				  int jy = heli.pos.j + heliY[hi];
				  Position temp;
				  temp.i = ix;
				  temp.j = jy;
				  if (canHeliGoThere(ix,jy,hi)) okayPos.push_back(temp);
				}
				
				findHeliStrategy(oneStep, resX, resY, heli, myTeam, okayPos, i, thePosts);
				
				executeHeliMove(heli, resX, resY, oneStep);
			}
				
		}
   }
  
   
   
   
  /**
   * Play method, invoked once per each round - strategy!
   */
  virtual void play () {
	int myTeam = me();

	VecInt myHelis = helicopters(myTeam); //my helicopters
	VecInt mySolds = soldiers(myTeam); //my soldiers
	VectPosts thePosts = posts();
	
	moveMyHelis(myHelis,myTeam,mySolds,thePosts);

    if(not mySolds.empty()) {
		for(int i = 0; i < mySolds.size(); ++i) {
			if(not canAttack(myTeam,mySolds[i])) {
				Data d = data(mySolds[i]);
				int x = d.pos.i;
				int y = d.pos.j;
					soldierFindPath(x,y,myTeam,mySolds[i]);
			}
				
		}
	}
}


};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
