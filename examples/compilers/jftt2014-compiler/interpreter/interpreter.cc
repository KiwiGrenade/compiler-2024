/*
 * Kod interpretera maszyny rejestrowej do projektu z JFTT2014
 *
 * Autor: Maciek Gębala
 * http://mgc.im.pwr.wroc.pl/
 * 2014-04-14
*/

#include<iostream>
#include<fstream>
#include<vector>
#include<map>

#include<stdlib.h> 	// rand()
#include<time.h>

using namespace std;

const int SCAN = 0;
const int PRINT = 1;
const int LOAD = 2;
const int STORE = 3;
const int ADD = 4;
const int SUB = 5;
const int SHR = 6;
const int SHL = 7;
const int INC = 8;
const int DEC = 9;
const int ZERO = 10;
const int JUMP = 11;
const int JZ = 12;
const int JG = 13;
const int JODD = 14;
const int HALT = 15;

int main(int argc, char* argv[])
{
    vector< pair<int,int> > program;
    map<int,long long> pam;

    long long a;
    int lr;

    int k=0;
    long long i;
    int i1, i2;
    string com;

    if( argc!=2 )
    {
        cout << "Sposób użycia programu: interpreter kod" << endl;
        return -1;
    }

    cout << "Czytanie programu." << endl;
    ifstream plik( argv[1] );
    if( !plik )
    {
        cout << "Błąd: Nie można otworzyć pliku " << argv[1] << endl;
        return -1;
    }
    while( !plik.eof() )
    {
        plik >> com;

        i1 = -1;
        if( com=="SCAN"  ) { i1 = SCAN; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                }
        if( com=="PRINT" ) { i1 = PRINT; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                }

        if( com=="LOAD"  ) { i1 = LOAD; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }
        if( com=="STORE" ) { i1 = STORE; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                }

        if( com=="ADD"   ) { i1 = ADD; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }
        if( com=="SUB"   ) { i1 = SUB; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }

        if( com=="SHR"   ) { i1 = SHR; i2 = 0;
                            }
        if( com=="SHL"   ) { i1 = SHL; i2 = 0;
                            }
        if( com=="INC"   ) { i1 = INC; i2 = 0;
                            }
        if( com=="DEC"   ) { i1 = DEC; i2 = 0;
                            }
        if( com=="ZERO"  ) { i1 = ZERO; i2 = 0;
                            }

        if( com=="JUMP"  ) { i1 = JUMP; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }
        if( com=="JZ"    ) { i1 = JZ; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }
        if( com=="JG"    ) { i1 = JG; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }
        if( com=="JODD"  ) { i1 = JODD; plik >> i2;
                            if(i2<0) { cout << "Błąd: zły adress w instrukcji " << k << endl; return -1; }
                            }

        if( com=="HALT"  ) { i1 = HALT; i2 = 0;
                            }
        if( i1==-1 )
        {
            cout << "Błąd: Nieznana instrukcja w linii " << program.size()+1 << "." << endl;
            return -1;
        }

        if( plik.good() )
        {
            pair<int,int> temp(i1,i2);
            program.push_back( temp );
        }
        k++;
    }
    plik.close();
    cout << "Skończono czytanie programu (linii: " << program.size() << ")." << endl;

    cout << "Uruchamianie programu." << endl;
    lr = 0;
    srand(time(NULL));
    a = rand();
    i = 0;
    while( program[lr].first!=HALT )	// HALT
    {
	switch( program[lr].first )
	{
	case SCAN:	cout << "? "; cin >> pam[program[lr].second]; i+=100; lr++; break;	// SCAN
	case PRINT:	cout << "> " << pam[program[lr].second] << endl; i+=100; lr++; break;	// PRINT

	case LOAD:	a=pam[program[lr].second]; if(program[lr].second<3) i+=10; else i+=100; lr++; cout<<"LOAD " << a << "\n"; break;	// LOAD
	case STORE:	pam[program[lr].second]=a; if(program[lr].second<3) i+=10; else i+=100; lr++; cout<<"STORE " << a << "\n"; break;	// STORE

	case ADD:	a+=pam[program[lr].second]; if(program[lr].second<3) i+=10; else i+=100; lr++; cout<<"ADD " << a << "\n"; break;	// ADD
	case SUB:	a-=pam[program[lr].second]; if( a<0 ) a=0; if(program[lr].second<3) i+=10; else i+=100; lr++; cout<<"SUB " << a << "\n"; break;	// SUB

        case SHR:       a>>=1; i++; lr++; cout<<"SHR " << a << "\n"; break;    // SHR
        case SHL:       a<<=1; i++; lr++; cout<<"SHL " << a << "\n"; break;    // SHL
	case INC:	a++; i++; lr++; cout<<"INC " << a << "\n"; break;	// INC
	case DEC:	a--; if( a<0 ) a=0; i++; lr++; cout<<"DEC " << a << "\n"; break;	// DEC
	case ZERO:	a=0; i++; lr++; cout<<"ZERO\n"; break;	// ZERO

	case JUMP: 	lr = program[lr].second; i++; cout<<"JUMP\n";break;		// JUMP
	case JZ:	if( a==0 ) lr = program[lr].second; else lr++; i++; cout<<"JZ " << a << "\n"; break; // JZ
	case JG:	if( a>0 ) lr = program[lr].second; else lr++; i++; cout<<"JG " << a << "\n"; break; // JG
	case JODD:	if( a%2!=0 ) lr = program[lr].second; else lr++; i++; cout<<"JODD " << a << "\n"; break; // JODD
	}
	if( lr<0 || lr>=(int)program.size() )
	{
	    cout << "Błąd: Wywołanie nieistniejącej instrukcji nr " << lr << "." << endl;
	    return -1;
	}

    }
    cout << "Skończono program (wykonano kroków: " << i << ")." << endl;

    return 0;
}

