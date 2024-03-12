#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>


using namespace std;

int main()
{
    cout << "|/////////////////////////////////////////////////////|" << endl;
    cout << "|//                                                 //|" << endl;
    cout << "|//                MS ACCESS TO MSQL                //|" << endl;
    cout << "|//              SOURCE FILE CONVERTER              //|" << endl;
    cout << "|//                                                 //|" << endl;
    cout << "|/////////////////////////////////////////////////////|" << endl;
    cout << endl;



    string databasename;

    string tablename;
    int tables=0;

    string exportname;
    string importname;

    string inextension;
    string ouextension;

    int separator;
    char character;

    int autod=0;

    std::cout << "Output file extension: ";
    cin >> ouextension;
    ouextension = "."+ouextension;

    cout << "Input file extension: ";
    cin >> inextension;
    inextension = "."+inextension;

    //BEMENETI FAJL ELVALASZTO KARAKTER
    cout << "|/////////////////////////|" << endl;
    cout << "|   Input file separator  |" << endl;
    cout << "| [1] (TAB) | [2] (SPACE) |" << endl;
    cout << "|/////////////////////////|" << endl;
    cout << "SEPARATOR NUMBER: ";
    do {
        cin >> separator;
    } while (separator < 1 || separator > 2);
    if (separator == 1) character = '\t';
    if (separator == 2) character = ' ';

    string s_separator = string(1, character);

    cout << endl;
    cout << "|/////////////////////////|" << endl;
    cout << "| SQL VARIABLE DECLARING  |" << endl;
    cout << "| [1] AUTO  | [2] MANUAL  |" << endl;
    cout << "|/////////////////////////|" << endl;
    cout << "METHOD NUMBER: ";
    do {
        cin >> autod;
    } while (autod < 1 || autod > 2);



    cout << endl;


    //ADATBAZIS NEVE
    cout << "DATABASE NAME: ";
    cin >> databasename;

    exportname = databasename + ouextension;
    ofstream oufile(exportname);

    oufile << "CREATE DATABASE IF NOT EXISTS " <<databasename<<" DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_hungarian_ci;" << endl;
    oufile << "USE "<<databasename<<";" << endl;
    oufile << endl;


    //TABLAK
    do {

        tables++;
        ifstream infile;
        cout << endl;
        cout << "TABLE NAME(" << tables << "): ";
        cin >> tablename;

        importname = tablename + inextension;
        infile.open(importname);

        //LETEZIK A TABLA?
        if (!infile.is_open() && tablename !="0" ) {
            do {
            cout << "WARNING: " << tablename << inextension <<" not found" << endl;
            cout << "TABLE NAME(" << tables << "): ";
            cin >> tablename;

            importname = tablename + inextension;
            infile.open(importname);
            } while (!infile.is_open() && tablename != "0");
        }

        //HA NEM 0, AZAZ NEM LEPUNK KI AKKOR
        if (tablename !="0") {


            //TABLAADATOK LETREHOZASA
            string column[1000];

            string variable;
            string variabletype[1000];
            string value[1000];
            bool nullvalue[1000] = {0};

            string null="0";
            int c=0;
            int primary=0;
            int pointer=0;

            string header;
            string word;

            getline(infile, header);
            istringstream headerstream(header);
            //OSZLOPOK DEFINIALASA, ELSODLEGES KULCS MEGADASA
            cout << "|/////////////////////////|" << endl;
            cout << "|  PIMARY KEY SELECTION   |" << endl;
            cout << "|/////////////////////////|" << endl;
            while (getline(headerstream, word, character)) {
                column[c] = word;
                cout << "[" << c+1 << "] " << column[c] << endl;
                pointer += word.length();
                c++;
            }

            cout << "[0] (none) " << endl;
            cout << "COLUMN NUMBER: ";
            cin >> primary;
            if (primary < 0 || primary > c) {
                do {
                    cout << "WARNING: Column does not exist" << endl;
                    cout << "COLUMN NUMBER: ";
                    cin >> primary;
                } while (primary < 0 || primary > c);
            }
            primary--;

            oufile << "CREATE TABLE "<<tablename<<" (" << endl;



            for (int i=0;i<c;i++) {

                //////////////////////////////////////////////
                //AUTOMATIKUS VATOZO DEKLARALAS
                ///////////////////////////////////////////////

                //INT //BOOLEAN //YEAR //DOUBLE //DATE //DATETIME //TIME //TEXT //VARCHAR

                if (autod == 1) {
                    infile.close();
                    infile.open(importname);
                    getline(infile, header);


                    int int_test;
                    double double_test;

                    int min=2147483647;
                    int max=-2147483648;
                    int dmax=0;
                    bool is_date=true;
                    bool is_datetime=true;
                    bool is_time=true;
                    bool is_string=false;
                    bool is_int=true;
                    bool is_double=false;
                    bool is_empty=true;


                    int x;


                    while(getline(infile, header)) {
                        x=0;
                        istringstream stream(header);
                        while (getline(stream, word, character)) {

                                if (x == i) {


                                    if (!word.empty()) {
                                        is_empty = false;

                                        ////ADAT HOSSZ
                                        if (word.length() > dmax) dmax = word.length();

                                        //DATUM?
                                        if (((word[4]!='-' || word[7]!='-' || word[13]!=':' || word[16]!=':' ) && (word[4]!='.' || word[7]!='.' || word[13]!=':' || word[16]!=':')) || word.length() !=19 ) {
                                            is_datetime = false;
                                            if (((word[4]!='-' || word[7]!='-') && (word[4]!='.' || word[7]!='.')) || word.length() !=10) is_date = false;
                                            if (word[2]!=':' || word[5]!=':' || word.length() != 8) is_time = false;
                                        }

                                        //SZAM?
                                        try {
                                            int_test = stoi(word);
                                        } catch (invalid_argument) {
                                            is_int=false;
                                            is_string=true;
                                        }
                                        if (is_int==true) {
                                            try {
                                                    double_test = stod(word);
                                                    if (double_test != int_test) {
                                                        is_double = true;
                                                    }
                                                } catch (invalid_argument) {
                                                    is_double=false;
                                                }
                                        }
                                        if (double_test != int_test) {
                                            is_double = true;
                                        }
                                        if (is_string == false && is_double == false ) {
                                            if (min > int_test) min = int_test;
                                            if (max < int_test) max = int_test;
                                        }


                                        //=STRING


                                    } else {
                                        nullvalue[x] = 1;
                                    }


                                }

                            x++;
                            }

                            while (x<c) {
                                nullvalue[x] = 1;
                                x++;

                            }
                        }



                    if (is_string==false) {
                        if (is_datetime == true) {
                            variabletype[i]="datetime";
                        } else if (is_date == true) {
                            variabletype[i]="date";
                        } else if (is_time == true) {
                            variabletype[i]="time";
                        } else if (is_double==true) {
                            variabletype[i]="double";
                        } else if (is_double == false && min >= 1900 && max<=3000) {
                            variabletype[i]="year";
                        } else if ((min==0 && max==1) || (min==-1 && max==0)) {
                            variabletype[i]="boolean";
                            null = "0";
                        } else {
                            variabletype[i]="int";
                            value[i]="(" + to_string(dmax+1) + ")";
                        }
                    } else {
                        if (dmax > 90) {
                            variabletype[i]="text";
                        } else {
                            variabletype[i]="varchar";
                            value[i]="(" + to_string(dmax+10) + ")";
                        }
                    }
                    if (is_empty) {
                        variabletype[i] = "varchar";
                        value[i] = "(20)";
                    }


                infile.close();
                infile.open(importname);
                getline(infile, header);
                istringstream headerstream(header);
                while (getline(headerstream, word, character)){}
                }
                ///////////////////////////////////////////////
                ///////////////////////////////////////////////
                ///////////////////////////////////////////////





                //////////////////////////////////////////////
                //MANUALIS VALTOZO DEKLARALAS
                ///////////////////////////////////////////////
                if (autod == 2) {

                    cout << "COLUMN("<<column[i]<<") VARIABLE TYPE: ";

                    bool p=false;
                    int pos=0;
                    do {
                        if (p == true) {
                            cout << "WARNING: Non-SQL variable type" << endl;
                            cout << "COLUMN("<<column[i]<<") VARIABLE TYPE: ";
                        }
                        cin >> variable;
                        p = true;
                        pos = variable.find("(");

                    } while (pos <= 1 && variable != "date" && variable != "text" && variable != "binary" && variable != "boolean" && variable != "year" && variable != "real" && variable != "double" && variable != "time" && variable != "datetime");

                    if (variable != "date" && variable != "text" && variable != "boolean" && variable != "binary" && variable != "year" && variable != "real" && variable != "double" && variable != "time" && variable != "datetime") {
                        variabletype[i] = variable.substr(0, pos);
                        value[i] = variable.substr(pos);
                    } else {
                        variabletype[i] = variable;
                    }
                    if (variable != "boolean") {
                        cout << "ENABLE NULL VALUES: ";
                        cin >> null;
                    }
                    if (null == "1" || null == "yes" || null == "YES" || null == "y" || null == "Y") {
                        nullvalue[i] = 1;
                    }

                }
                ///////////////////////////////////////////////
                ///////////////////////////////////////////////
                ///////////////////////////////////////////////


                //OSZLOPOK VALTOZOJANAK VEGLEGESITESE
                oufile << "  ";
                oufile <<column[i]<<" ";
                if (variabletype[i] == "varchar") {
                    oufile <<variabletype[i]<<value[i]<<" COLLATE utf8mb4_hungarian_ci";
                }
                else if (variabletype[i] == "text") {
                    oufile <<variabletype[i]<<" COLLATE utf8mb4_hungarian_ci";
                }
                else if (variabletype[i] == "date" || variabletype[i] == "boolean" || variabletype[i] == "year" || variabletype[i] == "binary" || variabletype[i] == "real" || variabletype[i] == "double"|| variabletype[i] == "time" || variabletype[i] == "datetime") {
                    oufile <<variabletype[i];
                } else {
                    oufile <<variabletype[i]<<value[i];
                }
                if (i == primary) {
                    oufile << " PRIMARY KEY";
                }

                if (nullvalue[i] == 0) {
                    oufile << " NOT NULL";
                }

                if (i < c-1) {
                    oufile << ",";
                }
                oufile << endl;


            }





            //OSZLOPOK ELOKESZITESE AZ ERTEKEK MEGADASARA
            oufile << ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_hungarian_ci;" << endl;
            oufile << endl;
            oufile << "INSERT INTO "<<tablename<<" (";

            for (int i=0;i<c;i++) {
                oufile << column[i];
                if (c-i != 1) {
                    oufile <<", ";
                } else {
                    oufile << ") VALUES" << endl;
                }
            }




            //ERTEKEK BEIMPORTALASA
            int z=0;
            bool start = false;

            while(getline(infile, header)) {
                if (start == true) {
                    oufile << ",\n";
                } else {
                    start = true;
                }
                oufile <<"(";
                istringstream headerstream(header);
                z=0;
                while (getline(headerstream, word, character)) {
                    z++;
                    if (!word.empty()) {
                        if (z>1) oufile <<", ";
                        if (variabletype[z-1] == "text" || variabletype[z-1] == "varchar" || variabletype[z-1] == "varbinary" || variabletype[z-1] == "binary" || variabletype[z-1] == "date" || variabletype[z-1] == "time" || variabletype[z-1] == "datetime") {
                                oufile << "'" << word << "'";
                        } else {
                            oufile << word;
                        }
                    } else {
                        if (z>1) oufile <<", ";
                        oufile << "NULL";
                    }

                }
                while (z<c) {
                    z++;
                    oufile << ", NULL";
                }
                oufile <<")";

            }
            oufile <<";\n\n";
        }

        infile.close();




    } while (tablename != "0");


    oufile.close();

}

//MADY BY DENCS BULCSÚ
