// kompilator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <fstream>
#include <vector>
#include <map>
#include <set>
#include "time.h"
#include "konstanty.h"
using namespace std;


set<RULE> CLOSURE(RULE &zac)
{
set<RULE> ret;
set<RULE>::iterator it;
ret.insert(zac);
unsigned long size=0;
do 
{
	bool brk=false;
	size=ret.size();
	for(it=ret.begin();it!=ret.end();it++)
	{
		if (it->dot<it->right.size())
		{
			for(vector<RULE>::iterator pridajR=rulesin.begin();pridajR!=rulesin.end();pridajR++)
			{
				if (pridajR->left==it->right[it->dot])
				{
					if (ret.find(*pridajR)==ret.end()) 
					{
						ret.insert(*pridajR);
						brk=true;
						break;
					}
				}
			}
		}
		if (brk) {brk=false;break;}
	}
} while (size<ret.size());
return ret;
}

set<RULE> GOTO(set<RULE> &I,string &X)
{
set<RULE> ret;
for(set<RULE>::iterator it=I.begin();it!=I.end();it++)
{
	if (it->dot<it->right.size())
	{
		if (X==it->right[it->dot])
		{
			RULE tmp=*it;tmp.dot++;
			set<RULE> closure_tmp=CLOSURE(tmp);
			for(set<RULE>::iterator it2=closure_tmp.begin();it2!=closure_tmp.end();it2++)
				ret.insert(*it2);
		}
	}
}
return ret;
}

long ISEND(set<RULE> &stav)
{
	for(set<RULE>::iterator it=stav.begin();it!=stav.end();it++)
	{
		if (it->dot==it->right.size())
			return it->number;
	}
	return -1;
}

int _tmain(int argc, char* argv[])
{
	if (argc==1) 
	{
		printf("usage>%s meno_suboru",argv[0]);
		exit(0);
	}

 char sdate[9];
char stime[9];
_strdate( sdate );
_strtime( stime );
cout << "#" << "time: " << stime << " date: " << sdate << endl;


  myEOF+=char(0);
  myEOF+="-1";
  customStartSymbol="_MYCUSTOMSTART";
  NT.insert(customStartSymbol);
  validT.insert(myEOF);
  NT.insert(myEOF);

	string subor;
	subor=argv[1];
//	cout << subor <<endl;
//	 ifstream ifs( "D:\\SKOLA\\KOMPILATORY\\kompilator\\Debug\\simple_syntax.txt" );
	string vystup=subor.c_str();
	vystup+="_automat.txt";
	 ifstream ifs( subor.c_str() );
	 ofstream ofs( vystup.c_str());
	ofs << "digraph html {"<<endl;
//	ofs << "digraph finite_state_machine {"<<endl;
		ofs << "node [style=\"filled\" color=yellow];"<<endl;
  string line;
  long linenumber=-1;
	long CisloPravidla=1;
	rulesin.push_back(RULE());
  while( getline( ifs, line ) )
  {
	  linenumber++;
	  RULE newrule;
	  newrule.dot=0;
	  newrule.number=CisloPravidla;
	  newrule.semantic=="";
	  unsigned long i=0; //som na zaciatku stringu
	  while(i<line.size())
	  {
  		  if (line[i]==':'){ i++;break;}
		  if ((line[i]<'0')||(line[i]>'9'))
		  {
			  cout<< "Pravidlo "<<CisloPravidla << " znak " << i <<" semantika obsahuje znak ktory nie je cislo #"<< line[i] << "# (chyba znak '<')" <<endl;exit(8);
		  }
		  newrule.semantic+=line[i];
		  i++;
	  }
	  if (newrule.semantic.empty()) newrule.semantic+="0";
	  if (line[i]!='<') {cout<< "Pravidlo "<<CisloPravidla << " znak " << i <<" nezacina neterminalom(chyba znak '<')" <<endl;exit(8);}
	  else i++;
	  newrule.left="_";
	  while(i<line.size())
	  {
		  if (line[i]=='>'){ //newrule.left+="_";
								i++;break;}
		  newrule.left+=line[i];
		  i++;
	  }
	  if (firstNT.empty()) firstNT=newrule.left;
	  NT.insert(newrule.left);
	  string znak;
	  while(i<line.size())
	  {
		 if ((line[i]!='"') && (line[i]!='<')) 
		 {
			 cerr << "Zly znak na vstupe >" << line[i] << "<[" << linenumber <<","<< i << "]" << endl;
			 i++;
		 }
		 if (line[i]=='"')
		 {
			 i++;
			  while(i<line.size())
			  {
				  if (line[i]=='"'){ znak.clear();i++;break;}
				  if (line[i]!='\\')
				  {
					  string aa;aa+=line[i];
					  newrule.right.push_back(aa);
					  validT.insert(aa);
				  } else
				  {
					  if (line[i+1]=='\\') {newrule.right.push_back("\\");validT.insert("\\");}
					  if (line[i+1]=='n')  {newrule.right.push_back(string()+char(10));validT.insert("\n");}
					  if (line[i+1]=='u')  {newrule.right.push_back("'");validT.insert("\'");}
					  i++;
				  }
				  i++;
			  }
		 }
		 if (line[i]=='<')
		 {
			 i++;
			 znak+="_";
			  while(i<line.size())
			  {
				  if (line[i]=='>'){ //znak+="_";
										newrule.right.push_back(znak);znak.clear();i++;break;}
				  znak+=line[i];
				  i++;
			  }
		 }
	  }
	  rulesin.push_back(newrule);
	  CisloPravidla++;
     //text_file.push_back( temp );
  }
    RULE zac;zac.dot=0;zac.left=customStartSymbol;zac.right.push_back(firstNT);zac.right.push_back(myEOF);zac.number=0;

    rulesin[0]=zac;

	ComputeFirst();
	ComputeFollow();
	vector<set<RULE>> automat;
	automat.push_back(CLOSURE(zac));
	set<RULE> test=CLOSURE(rulesin[0]);
	set<RULE> test2=GOTO(automat[0],string("("));

	for(unsigned char a=0;a<=255;a++)
	{
		string aa;aa+=a;
		NT.insert(aa);
		if (a==255) break;
	}
	vypisZaciatok();

	stringstream stramGototable (stringstream::in | stringstream::out);
	stramGototable << "goto_table:" <<endl;

	cout << "action_table:" << endl;

	//# .int state, symbol, shift/reduce, next_state/reduce_rule
	unsigned long STATE=0;
	while(STATE<automat.size())
	{
		for(set<string>::iterator symbol=NT.begin();symbol!=NT.end();symbol++)
		{
			set<RULE> kam=GOTO(automat[STATE],*symbol);

			if (*symbol==myEOF)// AKCEPTOVANIE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! AKCEPTOVANIE
			{
				RULE konec;konec=zac;konec.dot=1;set<RULE> konecA;konecA.insert(konec);
				if (automat[STATE]==konecA)
				{
					ofs << "  ST_"<< STATE << "-> ACC[label = \"ACCEPT\"]" << endl;
					cout << ".int "<< STATE <<", "<< "-1" <<", ACCEPT, 0" <<endl;
					continue;
				}
			}
	
			if (kam.size()>0)
			{
				long kamState=-1;
				for(unsigned long i=0;i<automat.size();i++)
				{
					if (automat[i]==kam)
					{
						kamState=i;
						break;
					}
				}
				if (kamState<0)
				{
					automat.push_back(kam);
					kamState=automat.size()-1;
				}
				if (symbol->size()==1)
				{
					//if (reduceOnPravidlo==0)
					//	cout << ".int "<< STATE <<", "<< long((*symbol)[0]) <<", ACCEPT, 0" <<endl;
					//else
					ofs << "   ST_"<< STATE << " -> ST_" << kamState << "[label = \""<< tr(*symbol) << "\"]" << endl;
					cout << ".int "<< STATE <<", "<< long((*symbol)[0]) <<", SHIFT, " << kamState << "   #" << (*symbol)[0] << endl;
					continue;
				}
				else
				{
						ofs << "   ST_"<< STATE << " -> ST_" << kamState << "[label = \""<< tr((*symbol)) << "\"]" << endl;
//						cout << "Zo stavu " << STATE << "mam na znak " << *symbol << " ist do stavu " << kamState << " redukuj "<< reduceOnPravidlo <<endl; 					
						stramGototable << ".int " << STATE << ", " << *symbol << ", " << kamState << endl;
						continue;
				}
			}
			//CI MAM REDUKOVAT
			if (validT.find(*symbol)!=validT.end())
			{
				for(set<RULE>::iterator itr=automat[STATE].begin();itr!=automat[STATE].end();itr++)
				{
					if (itr->dot==itr->right.size())
					{
						//je tam pravidlo ktore je konecne
						if (FOLLOW[itr->left].find(*symbol)!=FOLLOW[itr->left].end())
						{
							if (*symbol==myEOF)
								cout << ".int "<< STATE <<", "<< "-1" << ", REDUCE, " << itr->number << endl; 
							else
								cout << ".int "<< STATE <<", "<< long((*symbol)[0]) << ", REDUCE, " << itr->number << endl; 
							break;
						}
					}
				}
			}
		}
		STATE++;
	}
	cout << "action_table_end:"<<endl << ".set action_entry_size, 16"<<endl << ".set action_table_size, (action_table_end - action_table) / action_entry_size"<<endl<<endl<<endl<<endl;
	stramGototable << "goto_table_end:" <<endl << ".set goto_entry_size, 12" <<endl << ".set goto_table_size, (goto_table_end - goto_table) / goto_entry_size" <<endl<<endl<<endl;



	STATE=0;
	while(STATE<automat.size())
	{
		ofs << " ST_"<<STATE << " [shape=none, margin=0, label=<" <<endl;
		ofs << "   <TABLE>"<<endl;
		for(set<RULE>::iterator it=automat[STATE].begin();it!=automat[STATE].end();it++)
		{
			ofs << "       <TR><TD ALIGN='LEFT'>";
			ofs << tr(it->left)  << " -&gt; ";
			for(long ii=0;ii<it->right.size();ii++)
			{
				if (ii==it->dot) ofs << "&copy;";
				ofs << tr(it->right[ii]);
			}
			if (it->dot==it->right.size()) ofs << "&copy;";
			ofs << "</TD></TR>" <<endl;
		}
		ofs << "   </TABLE>>];" <<endl;
		STATE++;
	}

	cout << stramGototable.str();;
	ofs << "}" <<endl;
	vypisRuleTable();
	vypisDeklaraciuCharov();
	vypisDeklaraciuNeterminalov();
	vypisDefiniciuCharov();
	vypisDefiniciyNeterminalov();
//	long a;
//	cin >> a;
	return 0;
}

