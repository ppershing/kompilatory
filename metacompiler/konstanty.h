
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

string myEOF;

string tr(string aa)
{
if (aa==myEOF) return "EOF";
string bb;
long unsigned i=0;
if ((aa.length()>1) && aa[0]=='_') {bb+="&lt;";i++;}
else bb+="'";
for(;i<aa.length();i++)
{
        if (aa[i]=='\n') bb+="\\n";
        else bb+=aa[i];
}
if ((aa.length()>1) && aa[0]=='_') { bb+="&gt;";}
else bb+="'";
return bb;
}

struct RULE
{
friend bool operator<(const RULE& q1, const RULE& q2)
{
	if( q1.right.size() > q2.right.size()) return true;
	if( q1.right.size() < q2.right.size()) return false;
	if( q1.dot > q2.dot ) return true;
	if( q1.dot < q2.dot ) return false;

    if( q1.semantic < q2.semantic )return true; 
	if( q1.semantic > q2.semantic )return false; 
	if( q1.left < q2.left )return true; 
	if( q1.left > q2.left )return false; 
	for(unsigned long i=0;i<q1.right.size();i++)
	{
		if( q1.right[i] < q2.right[i]) return true;
		if( q1.right[i] > q2.right[i]) return false;
	}
    return false;
}
friend bool operator==(const RULE& q1, const RULE& q2)
{

    if(( q1.semantic == q2.semantic )&&
	  ( q1.left == q2.left )&&
	  ( q1.right.size() == q2.right.size())&&
	  ( q1.dot == q2.dot ))
	{
		bool same=true;
		for(unsigned long i=0;i<q1.right.size();i++)
		{
			if( q1.right[i] != q2.right[i]) {same=false;break;}
		}
		return same;
	}
	
	return false;
}
long number;
string semantic;
string left;
vector<string> right;
unsigned long dot;
};

map<string,set<string> > FIRST;
map<string,set<string> > FOLLOW;
set<string> NT;
set<string> validT;
vector<RULE> rulesin;
string firstNT;
string customStartSymbol;

void vypisZaciatok()
{
cout 
<< ".global table_states" << endl
<< ".global action_table" << endl
<< ".global action_entry_size" << endl
<< ".global action_table_size" << endl
<< ".global goto_table" << endl
<< ".global goto_entry_size" << endl
<< ".global goto_table_size" << endl
<< ".global symbol_table" << endl
<< ".global symbol_table_size" << endl
<< ".global table_entry_size" << endl
<< ".global rule_table" << endl
<< ".global rule_entry_size" << endl
<< ".global symbol0" << endl
<< ".global ACCEPT" << endl
<< ".global SHIFT" << endl
<< ".global REDUCE" << endl
<< ".data" << endl
<< ".set ACCEPT, 0" << endl
<< ".set SHIFT, 1" << endl
<< ".set REDUCE, 2" << endl << endl<< endl<< endl;

}

void vypisRuleTable()
{
	cout << "rule_table:"<<endl;
	for(vector<RULE>::iterator it=rulesin.begin();it!=rulesin.end();it++)
	{
		cout <<  ".int rule" << it->number <<endl;
	}
//	cout << ".set rules, 6" << endl; 
	cout << "rule_table_end:" <<endl;
	cout << ".set rule_entry_size, 4"<<endl <<
	".set rule_table_size, (rule_table_end - rule_table) / rule_entry_size"<<endl <<endl;
	
	for(vector<RULE>::iterator it=rulesin.begin();it!=rulesin.end();it++)
	{
		cout <<  "rule" << it->number << ":" <<endl << ".int "<< it->semantic <<", "  << it->right.size() << ", " <<it->left ;
		for(unsigned long i=0;i<it->right.size();i++)
		{
			if ( it->right[i].size() >1)
			{
				if (it->right[i]==myEOF)
					cout << ", -1";
				else
					cout << ", " << it->right[i];
			}
			else cout << ", symbol" << long(it->right[i][0]);
		}
		cout <<endl;
	}
	cout << endl << endl << endl;
}

void 	vypisDeklaraciuNeterminalov()
{
	for(set<string>::iterator it=NT.begin();it!=NT.end();it++)
	{
		if ((it->size()>1)&&(*it!=myEOF))
		{
			cout << ".int " << *it<< endl;
		}
	}
	cout << "symbol_table_end:" <<endl;
 	cout << ".set symbol_table_size, (symbol_table_end - symbol_table) / rule_entry_size"<<endl <<endl<<endl;

}

void	vypisDefiniciyNeterminalov()
{
	for(set<string>::iterator it=NT.begin();it!=NT.end();it++)
	{
		if ((it->size()>1)&&(*it!=myEOF))
		{
			cout << *it << ":  .string \"" << *it <<"\"" << endl;
		}
	}

}

void vypisDeklaraciuCharov()
{
cout << "symbol_table:"<<endl;
unsigned char ch=-1;
do
{
    ch++;
    cout <<".int symbol" << long(ch) <<endl;
} while(ch!=255);
}


void vypisDefiniciuCharov()
{
cout 
<< "symbol0: .string \"CHAR0\"" << endl
<< "symbol1: .string \"CHAR1\"" << endl
<< "symbol2: .string \"CHAR2\"" << endl
<< "symbol3: .string \"CHAR3\"" << endl
<< "symbol4: .string \"CHAR4\"" << endl
<< "symbol5: .string \"CHAR5\"" << endl
<< "symbol6: .string \"CHAR6\"" << endl
<< "symbol7: .string \"CHAR7\"" << endl
<< "symbol8: .string \"CHAR8\"" << endl
<< "symbol9: .string \"CHAR9\"" << endl
<< "symbol10: .string \"CHAR10\"" << endl
<< "symbol11: .string \"CHAR11\"" << endl
<< "symbol12: .string \"CHAR12\"" << endl
<< "symbol13: .string \"CHAR13\"" << endl
<< "symbol14: .string \"CHAR14\"" << endl
<< "symbol15: .string \"CHAR15\"" << endl
<< "symbol16: .string \"CHAR16\"" << endl
<< "symbol17: .string \"CHAR17\"" << endl
<< "symbol18: .string \"CHAR18\"" << endl
<< "symbol19: .string \"CHAR19\"" << endl
<< "symbol20: .string \"CHAR20\"" << endl
<< "symbol21: .string \"CHAR21\"" << endl
<< "symbol22: .string \"CHAR22\"" << endl
<< "symbol23: .string \"CHAR23\"" << endl
<< "symbol24: .string \"CHAR24\"" << endl
<< "symbol25: .string \"CHAR25\"" << endl
<< "symbol26: .string \"CHAR26\"" << endl
<< "symbol27: .string \"CHAR27\"" << endl
<< "symbol28: .string \"CHAR28\"" << endl
<< "symbol29: .string \"CHAR29\"" << endl
<< "symbol30: .string \"CHAR30\"" << endl
<< "symbol31: .string \"CHAR31\"" << endl
<< "symbol32: .string \" \"" << endl
<< "symbol33: .string \"!\"" << endl
<< "symbol34: .string \"CHAR34\"" << endl
<< "symbol35: .string \"CHAR35\"" << endl
<< "symbol36: .string \"$\"" << endl
<< "symbol37: .string \"%\"" << endl
<< "symbol38: .string \"&\"" << endl
<< "symbol39: .string \"'\"" << endl
<< "symbol40: .string \"(\"" << endl
<< "symbol41: .string \")\"" << endl
<< "symbol42: .string \"*\"" << endl
<< "symbol43: .string \"+\"" << endl
<< "symbol44: .string \",\"" << endl
<< "symbol45: .string \"-\"" << endl
<< "symbol46: .string \".\"" << endl
<< "symbol47: .string \"/\"" << endl
<< "symbol48: .string \"0\"" << endl
<< "symbol49: .string \"1\"" << endl
<< "symbol50: .string \"2\"" << endl
<< "symbol51: .string \"3\"" << endl
<< "symbol52: .string \"4\"" << endl
<< "symbol53: .string \"5\"" << endl
<< "symbol54: .string \"6\"" << endl
<< "symbol55: .string \"7\"" << endl
<< "symbol56: .string \"8\"" << endl
<< "symbol57: .string \"9\"" << endl
<< "symbol58: .string \":\"" << endl
<< "symbol59: .string \";\"" << endl
<< "symbol60: .string \"<\"" << endl
<< "symbol61: .string \"=\"" << endl
<< "symbol62: .string \">\"" << endl
<< "symbol63: .string \"?\"" << endl
<< "symbol64: .string \"@\"" << endl
<< "symbol65: .string \"A\"" << endl
<< "symbol66: .string \"B\"" << endl
<< "symbol67: .string \"C\"" << endl
<< "symbol68: .string \"D\"" << endl
<< "symbol69: .string \"E\"" << endl
<< "symbol70: .string \"F\"" << endl
<< "symbol71: .string \"G\"" << endl
<< "symbol72: .string \"H\"" << endl
<< "symbol73: .string \"I\"" << endl
<< "symbol74: .string \"J\"" << endl
<< "symbol75: .string \"K\"" << endl
<< "symbol76: .string \"L\"" << endl
<< "symbol77: .string \"M\"" << endl
<< "symbol78: .string \"N\"" << endl
<< "symbol79: .string \"O\"" << endl
<< "symbol80: .string \"P\"" << endl
<< "symbol81: .string \"Q\"" << endl
<< "symbol82: .string \"R\"" << endl
<< "symbol83: .string \"S\"" << endl
<< "symbol84: .string \"T\"" << endl
<< "symbol85: .string \"U\"" << endl
<< "symbol86: .string \"V\"" << endl
<< "symbol87: .string \"W\"" << endl
<< "symbol88: .string \"X\"" << endl
<< "symbol89: .string \"Y\"" << endl
<< "symbol90: .string \"Z\"" << endl
<< "symbol91: .string \"[\"" << endl
<< "symbol92: .string \"CHAR92\"" << endl;
unsigned char ch=93;ch=ch-1;
do
{
  ch++;
	if ( ch==127) {
		cout << "symbol127: .string \"CHAR127\"" << endl;
		continue;
	}
	cout << "symbol" << long(ch) <<": .string \"" << ch <<"\"" << endl;
		if( ch==255)break;
} while(ch!=255);

}

void ComputeFirst()
{
	set<string> empty;empty.clear();
	for(set<string>::iterator it=validT.begin();it!=validT.end();it++)
	{
		FIRST[*it]=empty;
		FIRST[*it].insert(*it);
	}
	bool changed=false;
	do
	{
		changed=false;

		for(set<string>::iterator it=NT.begin();it!=NT.end();it++)
		{
			for(vector<RULE>::iterator itr=rulesin.begin();itr!=rulesin.end();itr++)
			{
				if (itr->left==*it)  //mam pravidlo matchujuce mojmu neterminalu
				{
					if (itr->right.size()==0)
					{
						if (FIRST[*it].find("")==FIRST[*it].end())
						{
							FIRST[*it].insert("");changed=true;
						}
					} else {
						long velkost=FIRST[*it].size();
						bool isepsi=(FIRST[*it].find("")!=FIRST[*it].end());
						FIRST[*it].insert(FIRST[itr->right[0]].begin(),FIRST[itr->right[0]].end());
						if (isepsi!=(FIRST[*it].find("")!=FIRST[*it].end()) && itr->right.size()>1)
						{
							FIRST[*it].erase("");
						}

						unsigned long j=0;
						while (FIRST[itr->right[j]].find("")!=FIRST[itr->right[j]].end())
						{
							if (j==(itr->right.size()-1)) break;
							bool isepsi=(FIRST[*it].find("")!=FIRST[*it].end());
							FIRST[*it].insert(FIRST[itr->right[j+1]].begin(),FIRST[itr->right[j+1]].end());
							if (j!=(itr->right.size()-2)) {
								if (isepsi!=(FIRST[*it].find("")!=FIRST[*it].end()))
								{
									FIRST[*it].erase("");
								}
							}
							j++;
						}

						if (velkost!=(long)FIRST[*it].size()) 
							changed=true;
					}
				}
			}
		}
			

	}while (changed);
}
void ComputeFollow()
{
	set<string> empty;empty.clear();
	FOLLOW[firstNT].insert(myEOF);
	bool changed=false;
	do
	{
		changed=false;
		for(vector<RULE>::iterator itr=rulesin.begin();itr!=rulesin.end();itr++)
		{
			for(unsigned long i=0;i<itr->right.size();i++)
			{
			    // RULE IS B->\alpha A \betha   pricom B==itr->right  A==itr->right[i]
				if (i!=itr->right.size()-1)
				{
					long size=FOLLOW[itr->right[i]].size();
					FOLLOW[itr->right[i]].insert(FIRST[itr->right[i+1]].begin(),FIRST[itr->right[i+1]].end());
					FOLLOW[itr->right[i]].erase("");
					if (size!=(long)FOLLOW[itr->right[i]].size()) changed=true;
					//check if FIRST[\betha]==epsilon
					if (itr->right.size()==4)
					{
						//long a=1;
					}
					long pocet=0;
					for (unsigned long j=i+1;j<itr->right.size();j++)
					{
						if (FIRST[itr->right[j]].find("")!=FIRST[itr->right[j]].end()) 
						{
							if (j!=itr->right.size()-1)
							{
								long size=FOLLOW[itr->right[i]].size();
								FOLLOW[itr->right[i]].insert(FIRST[itr->right[j+1]].begin(),FIRST[itr->right[j+1]].end());
								FOLLOW[itr->right[i]].erase("");
								if ((unsigned long)size!=FOLLOW[itr->right[i]].size()) changed=true;
							}
							pocet++;
						}
						else break;
					}
					if (pocet==((long(itr->right.size()))-long(i)-1))
					{
						long size=FOLLOW[itr->right[i]].size();
						FOLLOW[itr->right[i]].insert(FOLLOW[itr->left].begin(),FOLLOW[itr->left].end());
						if ((unsigned long)size!=FOLLOW[itr->right[i]].size()) changed=true;
					}
				} else
				{
                                        unsigned long size=FOLLOW[itr->right[i]].size();
					FOLLOW[itr->right[i]].insert(FOLLOW[itr->left].begin(),FOLLOW[itr->left].end());
					if (size!=FOLLOW[itr->right[i]].size()) changed=true;
				}
			}
		}
	}
	while(changed);
}
