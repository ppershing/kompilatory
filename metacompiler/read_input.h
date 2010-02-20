//dorobit funkciu ze parse terminal
//dorobit funkciu ze parse neterminal



int  readInput(string menoSuboru)
{
	ifstream ifs( subor.c_str() );	
	string line;
	long lineNumber=-1;
	unsigned long i;	//na ktorom znaku sa aktualne nachadzam
	long cisloPravidla=0;	//pravidla budu cislovane od 1
	rulesin.push_back(RULE());
	while( getline( ifs, line ) )
	{
		lineNumber++;
		i=0; //som na zaciatku stringu

		RULE newrule;
		newrule.dot=0;
		newrule.number=++cisloPravidla;
		newrule.semantic=="";


		//Nacitame semantiku pravidla ---------------------------------

		while(i < line.size())
		{
  			if (line[i] == ':')	//ak najdeme dvojbodku tak skoncime s nacitavanim semantiky
			{
				i++;
				break;
			}
			if ((line[i] < '0')||(line[i] > '9'))	//v semantike ocakavame cislo
			{
				cerr << "Pravidlo " << cisloPravidla << " riadok " << lineNumber << " znak " << i <<" cislo semantickeho pravidla obsahuje znak ktory nie je cislica #"<< line[i] << "#" <<endl;
				cerr << "Vysledok nebude vygenerovany !!!!!!!!!!!" << endl;
				exit(8);
			}
			newrule.semantic+=line[i];	//pridame najdenu cislicu do semantiky
			i++;
		}
		if (newrule.semantic.empty()) newrule.semantic+="0";	//ak by sa nahodou semantika nenacitala tak to osetrime tak ze ju nastavime na 0


		//Nacitame lavu stranu pravidla -------------------------------

		if (line[i] != '<') 
		{
			cerr<< "Pravidlo "<<cisloPravidla << " riadok " << lineNumber << " znak " << i <<" nezacina neterminalom(chyba znak '<')" <<endl;
			cerr << "Vysledok nebude vygenerovany !!!!!!!!!!!" << endl;
			exit(8);
		}
		else
		{
			i++;
		}
		newrule.left="_";	//vsetky neterminaly interne reprezentujeme tak ze zacinaju potrznikom aby maly dlzku vzdy veciu ako 1
		while(i < line.size())
		{
			if (line[i] == '>')
			{
				i++;
				break;
			}
			newrule.left+=line[i];
			i++;
		}
		NT.insert(newrule.left);	//pridame neterminal do mnoziny neterminalov

		if (firstNT.empty()) firstNT=newrule.left;	//ak nemame ziadny zaciatocny neterminal tak to bude prvy ktory najdeme

		//Nacitame pravu stranu pravidla ------------------------------
	
		while(i < line.size())
		{
			if ((line[i] != '"') && (line[i] != '<')) 
			{
				cerr << "Zly znak na vstupe >" << line[i] << "< riadok " << lineNumber <<" znak "<< i << endl;
				cerr << "Pokracujem s vynechanim tohto znaku" << endl;
				i++;
			}
			if (line[i] == '"')	//nacitame terminalne symboly ak ich je viacej zmenime ich na viacero po sebe iducich terminalov
			{
				i++;
				while(i < line.size())
				{
					if (line[i] == '"')
					{
						i++;
						break;
					}
					if (line[i] != '\\')	//ak to nie je lomitko tak terminalny symbol 
					{
						string tmp;
						tmp+=line[i];
						newrule.right.push_back(tmp);	//pridam do pravidla
						validT.insert(tmp);		//pridam do mnoziny terminalov
					} 
					else
					{
						if (i+1 >= line.size()) 
						{
							cerr << "Zly znak na vstupe >" << line[i] << "< riadok " << lineNumber <<" znak "<< i << endl;
							cerr << "Pokracujem s vynechanim tohto znaku" << endl;
							break;
						}
						if (line[i+1] == '\\') {newrule.right.push_back("\\");validT.insert("\\");}
						if (line[i+1] == 'n')  {newrule.right.push_back(string()+char(10));validT.insert("\n");}
						if (line[i+1] == 'u')  {newrule.right.push_back("\"");validT.insert("\"");}
						i++;
					}
					i++;
				}
			}


			if (line[i] == '<')	//nacitanie jedneho neterminalneho symbolu
			{
				i++;
				string neterminalSymbol="_";
				while(i < line.size())
				{
					if (line[i] == '>')
					{
						newrule.right.push_back(neterminalSymbol);
						i++;
						break;
					}
					neterminalSymbol+=line[i];
					i++;
				}
			}
		}
		rulesin.push_back(newrule);
	}
	return 0;
}
