// kompilator.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <fstream>
#include <vector>
#include <map>
#include <set>
#include "time.h"

using namespace std;

#include "konstanty.h"
#include "read_input.h"




set<RULE> CLOSURE(RULE &pravidlo)
{
set<RULE> ret;		//mnozina ktoru vratime nakonci
unsigned long size=0;   //posledna velkost mnoziny pre kontrolu ci sme do nej nieco pridali
set<RULE>::iterator it;
ret.insert(pravidlo);
do 
{
	bool brk=false;
	size=ret.size();
	for(it=ret.begin();it!=ret.end();it++)   //pre kazde pravidlo ktore uz v uzavere je
	{
		if (it->dot < it->right.size())	// a bodka este nie je na konci pravidla teda to za botkou mozem rozvinut
		{
			for(vector<RULE>::iterator pridajR=rulesin.begin();pridajR!=rulesin.end();pridajR++)  //vezmem vsetky pravidla
			{
				if (pridajR->left == it->right[it->dot])  //vyfiltrujem tie ktore zacinaju neterminalom ktory rozvijane pravidlo koncilo
				{
//					if (ret.find(*pridajR)==ret.end()) 
//					{
						ret.insert(*pridajR); // a pridam ho uzaveru
//						brk=true;
//						break;
//					}
				}
			}
		}
//		if (brk) {brk=false;break;}
	}
} while (size<ret.size());
return ret;
}

set<RULE> GOTO(set<RULE> &stav,string &symbol)
{
set<RULE> ret;			//mnozina ktoru vratime nakonci
for(set<RULE>::iterator it=stav.begin();it!=stav.end();it++)	//pre kazde pravidlo
{
	if (it->dot < it->right.size())		//ktore splna ze bodkou v nom este mozme pohnut
	{
		if (symbol == it->right[it->dot])  //ak je za bodkou spravny symbol
		{
			RULE tmp=*it;
			tmp.dot++;	//posunieme v pravidle bodku
			set<RULE> closure_tmp=CLOSURE(tmp);	//spravime uzaver
			//for(set<RULE>::iterator it2=closure_tmp.begin();it2!=closure_tmp.end();it2++)
			//	ret.insert(*it2);
			ret.insert(closure_tmp.begin(),closure_tmp.end());	//a pridame to do goto mnoziny
		}
	}
}
return ret;
}

long ISEND(set<RULE> &stav)	//vrati ci nejake pravidlo stavu je konecne
{
	for(set<RULE>::iterator it=stav.begin();it!=stav.end();it++)	//zo vsetkych pravidiel v nom
	{
		if (it->dot == it->right.size())	//najde take kde je botka nakonci
			return it->number;	//a vrati cislo toho pravidla
	}
	return -1;
}


int main(int argc, char* argv[])
{
	if (argc==1) 
	{
		fprintf(stderr,"usage>%s meno_suboru\n",argv[0]);
		exit(0);
	}
	if (readInput(argv[1]) != 0) //nacita a poparsuje vstup do rules_in 
  		return -1;


	string vystup=subor.c_str();
	vystup+=".dot";
        ofstream ofs( vystup.c_str());
	ofs << "digraph html {"<<endl;
        ofs << "rankdir=LR;concentrate=true;" <<endl;
	ofs << "node [style=\"filled\", color=greenyellow, fontsize=10];"<<endl;
        ofs << "edge [labeldistance = 1.5]; " << endl; 


  myEOF+=char(0);
  myEOF+="-1";
  customStartSymbol="_MYCUSTOMSTART";
  NT.insert(customStartSymbol);
  NT.insert(myEOF);
  validT.insert(myEOF);
 

 
	RULE zac;zac.dot=0;zac.left=customStartSymbol;zac.right.push_back(firstNT);zac.right.push_back(myEOF);zac.number=0;
	rulesin[0]=zac; // pridanie noveho zaciatocneho pravidla

	ComputeFirst();
	ComputeFollow();

	vector<set<RULE> > automat;
	automat.push_back(CLOSURE(zac));


	for(long ch=0;ch<=255;ch++)
	{
		string tmp;
		tmp+=unsigned char(ch);
		NT.insert(tmp);
	}

	vypisZaciatok();	//vypise veci ktore sa nachadzaju na zaciatku tabulky

	stringstream stramGototable (stringstream::in | stringstream::out);
	stramGototable << "goto_table:" <<endl;

	cout << "action_table:" << endl;

	//# .int state, symbol, shift/reduce, next_state/reduce_rule
	unsigned long STATE=0;
	while(STATE<automat.size())
	{
		for(set<string>::iterator symbol=NT.begin();symbol!=NT.end();symbol++)
		{
			string str=*symbol;
			set<RULE> kam=GOTO(automat[STATE],str);

			if (*symbol==myEOF)// AKCEPTOVANIE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! AKCEPTOVANIE
			{
				RULE konec;konec=zac;konec.dot=1;set<RULE> konecA;konecA.insert(konec);
				if (automat[STATE]==konecA)
				{
					ofs << "  ST_"<< STATE << "-> ACC[label = \"EOF\", len=2];" << endl;
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
					ofs << "   ST_"<< STATE << " -> ST_" << kamState << "[label=\""<< tr(*symbol) << "\" , len=2];" << endl;
					cout << ".int "<< STATE <<", "<< long((*symbol)[0]) <<", SHIFT, " << kamState << "   #" << (*symbol)[0] << endl;
                                        continue;
				}
				else
				{
						ofs << "   ST_"<< STATE << " -> ST_" << kamState << "[label = \""<< tr((*symbol)) << "\", len=2];" << endl;
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
		ofs << "   <TABLE BORDER='1'>"<<endl;
                long riadkov=automat[STATE].size();
		for(set<RULE>::iterator it=automat[STATE].begin();it!=automat[STATE].end();it++)
		{
                        if (it==automat[STATE].begin())
  			  ofs << "       <TR><TD ROWSPAN='" << riadkov << "' BGCOLOR='yellow'>" << STATE << "</TD><TD ALIGN='LEFT'>";
                        else
  			  ofs << "       <TR><TD ALIGN='LEFT'>";

			ofs << "<font color='darkgreen'>"<< tr(it->left)  << " -&gt;</font> ";
			for(unsigned long ii=0;ii<it->right.size();ii++)
			{
				if (ii==it->dot) ofs << "<font color='red'> @</font>";
				ofs << " "<< tr(it->right[ii]);
			}
			if (it->dot==it->right.size()) ofs << "<font color='red'> @</font>";
			ofs << "</TD></TR>" <<endl;
		}
		ofs << "   </TABLE>>];" <<endl;
		STATE++;
	}

	ofs << "}" <<endl;

	cout << stramGototable.str();;
	vypisRuleTable();
	vypisDeklaraciuCharov();
	vypisDeklaraciuNeterminalov();
	vypisDefiniciuCharov();
	vypisDefiniciyNeterminalov();
//	long a;
//	cin >> a;
	return 0;
}

