
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <sstream>
#include <algorithm>
using namespace std;


void split(string str, char delim, vector<string>& v);


/* Catalog Class*/
class catalog
{
public:
	string table_name;
	string table_columns;	
	string primary_key;
	int rec_size;
	int total_size;
	int records;
	vector<string> column_split ;
	vector<string> col_name;
	vector<string> col_type;
	catalog()
	{
	}
	void catalog_set_properties(string _table_name,string _table_columns,string _primary_key,int _rec_size,	int _total_size,int _records)
	{
		vector<string> temp;
		table_name=_table_name;
		table_columns=_table_columns;
		primary_key=_primary_key;
		rec_size=_rec_size;
		total_size =_total_size;
		records=_records;	
		split(_table_columns, ',', column_split);
		for (int j = 0; j < column_split.size(); j++)
		{
			temp.clear();
			split(column_split[j],':',temp);
			col_name.push_back(temp[0]);
			col_type.push_back(temp[1]);
		}
	}
};
/* Catalog Class*/
bool select_where_table (string command,catalog *catalog_object);
bool select_function(string command,catalog *catalog_object);

/* Global Variables */
vector<catalog> catalog_tbl;
vector<string> output;
int num_of_table_in_catalog=0;
vector<string> output_table;
vector<string> output_insert_into;
string output_file_name ="output.txt"; //By default output.txt file
/* Global Variables */

/* HELPER METHODS */

// To Convert the given string into lower case
string ToLowerCase(string str) {
	for (unsigned i=0;i<strlen(str.c_str());i++)		
			str[i] = tolower(str[i]);
	return str;
}

// Removes space in the string ( Leading / trailing / extra spaces in between )
string remove_spaces (string str)
{
	string return_str;
	int index=0;
	int space_count = 0;
	while(index < str.size())
	{
		if (str[index] != ' ')
			space_count = 0;
		if ( str[index] == ' ')
			space_count ++ ;		
		if(space_count == 1 || space_count == 0 )
		return_str = return_str + str[index];		
		index ++ ;
	}
	if(return_str[0] == ' ')
		return_str.erase(0,1);	
	if(return_str[return_str.length()-1] == ' ')
		return_str.erase(return_str.length()-1);
	return return_str;
}

//Gets the String , Char as input and spilts the contents and stores vector 
void split(string str, char delim,vector<string>& v)
 {
    size_t i;
	//Exit Condition
	if(str.find(delim) == string::npos)
	{
		v.push_back(str); 
		return;
	}
    i=str.find_first_of(delim);

	v.push_back(str.substr(0,i));    
    str.erase(0, i+1);
	//Recursion
	split(str,delim,v);
	
  }

// Spilt the catalof file contents and store in the class
void catalog_format(string Catalog_file_contents[], catalog * catalog_object)
{
	string catalog_file_table_content [1000][6];
	string table_name,columns,primary_key;
	string rec_size,total_size,records;
	int counter=0;
	for ( int i =0 ; i < num_of_table_in_catalog ; i ++)
	{
		catalog_file_table_content[i][0] = Catalog_file_contents[counter];
		counter++;
		catalog_file_table_content[i][1] = Catalog_file_contents[counter];
		counter++;
		catalog_file_table_content[i][2] = Catalog_file_contents[counter];
		counter++;
		catalog_file_table_content[i][3] = Catalog_file_contents[counter];
		counter++;
		catalog_file_table_content[i][4] = Catalog_file_contents[counter];
		counter++;
		catalog_file_table_content[i][5] = Catalog_file_contents[counter];
		counter++;
		
	}

	for ( int i =0 ; i < num_of_table_in_catalog ; i ++)
	{
		table_name = catalog_file_table_content[i][0].substr(catalog_file_table_content[i][0].find('=')+1,catalog_file_table_content[i][0].size());
		columns = catalog_file_table_content[i][1].substr(catalog_file_table_content[i][1].find('=')+1,catalog_file_table_content[i][1].size());
		primary_key =  catalog_file_table_content[i][2].substr(catalog_file_table_content[i][2].find('=')+1,catalog_file_table_content[i][2].size());
		rec_size = catalog_file_table_content[i][3].substr(catalog_file_table_content[i][3].find('=')+1,catalog_file_table_content[i][3].size());
		total_size = catalog_file_table_content[i][4].substr(catalog_file_table_content[i][4].find('=')+1,catalog_file_table_content[i][4].size());
		records =  catalog_file_table_content[i][5].substr(catalog_file_table_content[i][5].find('=')+1,catalog_file_table_content[i][5].size());
		catalog_object[i].catalog_set_properties(table_name,columns,primary_key,atoi(rec_size.c_str()),atoi(total_size.c_str()),atoi (records.c_str()));

	}
	
}

//write the contents of catalog object in the catlog.txt file
void write_into_catalog()
{
	ofstream catalog;
	catalog.open("catalog.txt");
	if (catalog.is_open())
	{
		for(unsigned i=0;i<catalog_tbl.size();i++)
		{
			if (catalog_tbl[i].table_name.substr(0,1) != "#")
			{
				catalog<<"tablename="<<catalog_tbl[i].table_name<<endl;
				catalog<<"columns="<<catalog_tbl[i].table_columns<<endl;
				catalog<<"primary key="<<catalog_tbl[i].primary_key<<endl;
				catalog<<"recordsize="<<catalog_tbl[i].rec_size<<endl;
				catalog<<"totalsize="<<catalog_tbl[i].total_size<<endl;
				catalog<<"records="<<catalog_tbl[i].records<<endl;
			}
			else if (catalog_tbl[i].table_name.substr(0,1) == "#")
			{
				string st = catalog_tbl[i].table_name + ".tbl";	
				const char * c = st.c_str();				
				remove(c);
			}
		}
	}
	catalog.close();
}

//Create table function
bool create_table (string command, catalog *catalog_object)
{
	string temp_cmd = command;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	if (temp_cmd.substr(0, temp_cmd.find(' ')) != "table")
	{
		cout<<"Syntax error:The keyword table in create statement is missing\n";
		output.push_back("Syntax error:The keyword table in create statement is missing\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	string temp_cmd1 = temp_cmd.substr(0,temp_cmd.find('('));
	temp_cmd1 = remove_spaces ( temp_cmd1);
	if(temp_cmd1.size() == 0)
	{
		cout<<"Syntax error:Table name should be followed after table keyword\n";
		output.push_back("Syntax error:Table name should be followed after table keyword\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find('('));	
	if(temp_cmd.substr(0,1) != "(")
	{
		cout<<"Syntax error:Table name should be following by open parenthesis\n";
		output.push_back("Syntax error:Table name should be following by open parenthesis\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find('(')+1);	
	if(temp_cmd.find_last_of(')') == string::npos)
	{
		cout<<"Syntax error:Missing close parenthesis\n";
		output.push_back("Syntax error:Missing close parenthesis\n");
		return 0;
	}	
	/*string temp_cmd2 = temp_cmd.substr(temp_cmd.find_last_of(')')+1,temp_cmd.size());
	if(temp_cmd2 != ";" || temp_cmd2 != " ;" )
	{
		cout<<"Syntax error:Text after closing parenthesis\n";
		output.push_back("Syntax error:Text after closing parenthesis\n");
		return 0;
	}*/
	temp_cmd=temp_cmd.substr(0,temp_cmd.find_last_of(')'));
	vector<string> columns;
	vector<string> temp;
	vector<string> column_name;
	vector<string> column_data_type;
	bool primary_key_flag =false;
	unsigned int primary_key_location;
	string temp_primary_key="";
	split(temp_cmd,',', columns);	
	for (int j = 0; j < columns.size(); j++)
		{
			temp.clear();
			columns[j]=remove_spaces(columns[j]);
			split(columns[j],' ',temp);
			column_name.push_back(temp[0]);
			if ( temp.size() == 3)
				column_data_type.push_back(temp[1]+temp[2]);
			else
				column_data_type.push_back(temp[1]);
		}
	//For Primary Key

	for (int k = 0; k < columns.size(); k++)
	{
		
			if ( column_name[k] == "primary" )
				if(column_data_type[k].substr(0,3) == "key" )
				{
					if(primary_key_flag == false)
					{
						
						column_data_type[k].erase(0,3);							
						if(column_data_type[k].substr(0,1) != "(")
						{
							cout<<"Syntax error:Primary key syntax should be following by open parenthesis\n";
							output.push_back("Syntax error:Primary key syntax should be following by open parenthesis\n");
							return 0;
						}
						column_data_type[k].erase(0, column_data_type[k].find('(')+1);	
						if(column_data_type[k].find_last_of(')') == string::npos)
						{
							cout<<"Syntax error:Missing close parenthesis in primary key syntax\n";
							output.push_back("Syntax error:Missing close parenthesis in primary key syntax\n");
							return 0;
						}	
						temp_primary_key=column_data_type[k].substr(0,column_data_type[k].find_last_of(')'));
						for (int x = 0; x < column_name.size(); x++)
						{
							if(temp_primary_key == column_name[x])
							{
								primary_key_flag = true ;	
								primary_key_location =k;
							}
						}
						if(primary_key_flag == false)
						{
							cout<<"Syntax error: The column name given in primary key constraint is not a valid column name of the table "<<endl;
							output.push_back("Syntax error: The column name given in primary key constraint is not a valid column name of the table\n");
							return 0;
						}						
						
					}
					else
					{
						cout<<"Syntax Error: Cannot add multiple PRIMARY KEY constraints to table\n";
						output.push_back("Syntax Error: Cannot add multiple PRIMARY KEY constraints to table\n");
						return 0;
					}

				}
		}
	if(primary_key_flag == true )
	{
	column_data_type.erase(column_data_type.begin() + primary_key_location);
	column_name.erase(column_name.begin() + primary_key_location );
	}
	//For Record Size
	int col_size = 0;
	for (int x =0 ;  x < column_name.size(); x++)
	{
		if(column_data_type[x] == "int" )
			col_size += 4;
		if(column_data_type[x].substr(0,5) =="char(")
		{
			if(column_data_type[x].find_last_of(')') == string::npos)
						{
							cout<<"Syntax error:Missing close parenthesis in char syntax\n";
							output.push_back("Syntax error:Missing close parenthesis in char syntax\n");
							return 0;
						}
			col_size += atoi(column_data_type[x].substr(5,column_data_type[x].find_last_of(')')).c_str());
			 
		}

	}
	// additional syntax - table name and column name should start with letter
	for(int y=0 ;y<column_name.size(); y++)
	{
		if(!isalpha(column_name[y][0]))
		{
			cout<<"Syntax Error:Column name should begin only with an alphabet value"<<endl;
			output.push_back("Syntax Error:Column name should begin only with an alphabet value\n");
			return 0;
		}
		if(column_data_type[y] != "int" && column_data_type[y].substr(0,5) != "char(")
		{
			cout<<"Syntax Error:Column type should be only int or char"<<endl;
			output.push_back("Syntax Error:Column type should be only int or char\n");
			return 0;
		}
	}
	if(isdigit(temp_cmd1[0]))
	{
		cout<<"syntax error:Table name cannot start with a numbers"<<endl;
		output.push_back("syntax error:Table name cannot start with a numbers\n");
		return 0;
	}
	
	// check if the table name is already present in datastructure. Cannot have two tables with same name
	for (int i=0;i<num_of_table_in_catalog;i++)
	{
		if ( catalog_object[i].table_name == temp_cmd1)
		{
			cout<<"There is already a table named "<< temp_cmd1 <<" in the database."<<endl;
			output.push_back("There is already a table named " + temp_cmd1 + " in the database.\n");
			return 0;
		}
	}
	// Store the contents in catalog table
	catalog_object[num_of_table_in_catalog].table_name = temp_cmd1;	
	catalog_object[num_of_table_in_catalog].total_size = 0;
	catalog_object[num_of_table_in_catalog].records = 0;
	catalog_object[num_of_table_in_catalog].rec_size=col_size;	
	catalog_object[num_of_table_in_catalog].primary_key = temp_primary_key;
	catalog_object[num_of_table_in_catalog].col_name = column_name;
	catalog_object[num_of_table_in_catalog].col_type = column_data_type;
	for(int i = 0 ; i < column_name.size() ; i ++ )
	{
		catalog_object[num_of_table_in_catalog].column_split.push_back( column_name[i] + ":" + column_data_type[i]);
	}
	if(catalog_object[num_of_table_in_catalog].column_split.size() == 0 )
	{
		catalog_object[num_of_table_in_catalog].table_columns += catalog_object[num_of_table_in_catalog].column_split[0];
	}
	else
	{
		for ( int i=0; i<catalog_object[num_of_table_in_catalog].column_split.size() ; i++ )
		{
			if( catalog_object[num_of_table_in_catalog].column_split.size()-1 == i)
				catalog_object[num_of_table_in_catalog].table_columns += catalog_object[num_of_table_in_catalog].column_split[i];
			else
			catalog_object[num_of_table_in_catalog].table_columns += catalog_object[num_of_table_in_catalog].column_split[i] + ",";
		}
	}

	catalog_tbl.push_back(catalog_object[num_of_table_in_catalog]);
	num_of_table_in_catalog ++ ;
	return true;

}

//Check whether a number or not
bool is_number(const string& s)
{
	string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

//Check primary key constraint and data type constraint in the insert query
bool check_primary_key(vector<string> columns,int table_position)
{
	int primary_key_postion=-1;
	string primary_key= catalog_tbl[table_position].primary_key;
	string char_size;
	int char_size1;
	char char_value[100];
	int col_int;
	for (unsigned j = 0;j<catalog_tbl[table_position].col_name.size();j++)
		{
			if(catalog_tbl[table_position].col_name[j] == primary_key)
			{
				primary_key_postion = j;
			}
		}
		ifstream key;
		string table_name =catalog_tbl[table_position].table_name;
		string table = table_name.append(".tbl");
	    const char * c = table.c_str();
		key.open (c,ios::in|ios::binary|ios::ate);
	if(primary_key_postion != -1)
	{

	if (catalog_tbl[table_position].col_type[primary_key_postion] == "int")
	{		
		if(key.is_open())
		{
		key.seekg (0, ios::end);		
		key.seekg (0, ios::beg);
		if (catalog_tbl[table_position].total_size == 0) return 0;
		for (int l=0;l<catalog_tbl[table_position].total_size;l=l+catalog_tbl[table_position].rec_size)
		{
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if (catalog_tbl[table_position].col_type[i] == "int")
				{
					key.read((char *)(&col_int),4);
					cout.width(15);
					cout.setf ( ios_base::left);
					if (( catalog_tbl[table_position].col_name[i] == catalog_tbl[table_position].primary_key) && (col_int == atoi(columns[primary_key_postion].c_str())))
						return 1;
				}
			}
		}
		

	}
	
	}
	if (catalog_tbl[table_position].col_type[primary_key_postion].substr(0,4) == "char")
	{
		if(key.is_open())
		{
		key.seekg (0, ios::end);		
		key.seekg (0, ios::beg);
		if (catalog_tbl[table_position].total_size == 0) return 0;
		for (int l=0;l<catalog_tbl[table_position].total_size;l=l+catalog_tbl[table_position].rec_size)
		{
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
				{
					char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
					char_size.erase(char_size.size()-1);
					char_size1 = atoi(char_size.c_str());
					key.read((char *)(&char_value),char_size1);
					cout.width(15);
					cout.setf ( ios_base::left);
					if ((catalog_tbl[table_position].col_name[i] == primary_key) && char_value == columns[primary_key_postion].substr(1,columns[primary_key_postion].size()-2))
						return 1;
					if ((catalog_tbl[table_position].col_name[i] == primary_key) && char_value == columns[primary_key_postion].substr(0,columns[primary_key_postion].size()-1))
						return 1;
				}
			}
		}
		}
	}
	key.close();	
	return 0;
	}
	return 0;
}

//Insert Table function
bool insert_table(string command , catalog *catalog_object)
{
	bool table_exists_flag=false;
	int table_position;
	int char_size1_in_constraint;
	string char_size_in_constraint,char_value_in_constraint;
	string temp_cmd = command;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	string temp_cmd1 = temp_cmd.substr(0,temp_cmd.find(' '));
	temp_cmd1 = remove_spaces ( temp_cmd1);
	if(temp_cmd1.size() == 0)
	{
		cout<<"Syntax error:Table name should be followed after into keyword\n";
		output.push_back("Syntax error:Table name should be followed after into keyword\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	temp_cmd1=temp_cmd.substr(0,temp_cmd.find(' '));
	temp_cmd1 = remove_spaces(temp_cmd1);
	// To check whether table name exists in the database
	for (unsigned i=0;i<catalog_tbl.size();i++)
	{
		if(catalog_tbl[i].table_name == temp_cmd1 )
		{
			table_exists_flag = true;
			table_position = i;
		}
	}
	if( table_exists_flag == false)
	{
		cout<<"Table Name doesn't exists in the database\n";
		output.push_back("Table Name doesn't exists in the database\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	string temp_cmd2 = temp_cmd.substr(0,6);
	temp_cmd2= remove_spaces ( temp_cmd2);
	if( temp_cmd2 == "values")
	{
		//Insert values block
		temp_cmd.erase(0, temp_cmd.find('('));
		if(temp_cmd.substr(0,1) != "(")
		{
		cout<<"Syntax error:values should be following by open parenthesis\n";
		output.push_back("Syntax error:values should be following by open parenthesis\n");
		return 0;
		}
		temp_cmd.erase(0, temp_cmd.find('(')+1);	
		if(temp_cmd.find_last_of(')') == string::npos)
		{
			cout<<"Syntax error:Missing close parenthesis\n";
			output.push_back("Syntax error:Missing close parenthesis\n");
			return 0;
		}	
		/*string temp_cmd2 = temp_cmd.substr(temp_cmd.find_last_of(')')+1,temp_cmd.size());
		if(temp_cmd2 != ";"  )
		{
			cout<<"Syntax error:Text after closing parenthesis\n";
			output.push_back("Syntax error:Text after closing parenthesis\n");
			return 0;
		}*/
		temp_cmd=temp_cmd.substr(0,temp_cmd.find_last_of(')'));
		vector<string> columns;
		split(temp_cmd,',', columns);
		for (unsigned j = 0;j<columns.size();j++)
		{
			columns[j] = remove_spaces(columns[j]);
		}
		if(columns.size() != catalog_tbl[table_position].col_name.size())
		{
			cout<<"Number of insert values is not equal to the number of fields in the table\n"<<endl;
			output.push_back("Number of insert values is not equal to the numbern of fields in the table\n");
			return 0;
		}
		//Check primary key constraint data type constraint
		if(check_primary_key(columns,table_position))
		{
			cout<<"Primary Key Constraint or data type constraint in insert statement\n"<<endl;
			output.push_back("Primary Key Constraint or data type constraint in insert statement\n");
			return 0;
		}
		//data type constraint
		
		for (unsigned i = 0;i<columns.size();i++)
		{
			if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char" && (columns[i].substr(0,1) == "'" || columns[i].substr(columns[i].size()-1,1) == "'"))
			{
				char_size_in_constraint = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
				char_size_in_constraint.erase(char_size_in_constraint.size()-1);
				char_size1_in_constraint = atoi(char_size_in_constraint.c_str());
				char_value_in_constraint = columns[i].substr(1,columns[i].size()-1);
				char_value_in_constraint.erase(char_value_in_constraint.size()-1);
				if(char_value_in_constraint.size() >(unsigned) char_size1_in_constraint )
				{
					cout<<"Error:Char entered is greater than char length of the assigned char field"<<endl;
					output.push_back("Error:Char entered is greater than char length of the assigned char field\n");
					return 0;
				}
			}
		}

		//write into binary file
		string char_size, char_value;
		int char_size1;
		fstream file;
		string table_name = catalog_tbl[table_position].table_name;
		table_name.append(".tbl");
		const char * c = table_name.c_str();
		file.open (c,ios::out|ios::binary|ios::app);
		for (unsigned i = 0;i<columns.size();i++)
		{
			if (catalog_tbl[table_position].col_type[i] == "int" && !is_number(columns[i]))
			{
				cout<<"Data Incompatible:Non numeric values are entered in a int field"<<endl;	
				output.push_back("Data Incompatible:Non numeric values are entered in a int field\n");
				return 0;
			}
			else if (catalog_tbl[table_position].col_type[i] == "int" && is_number(columns[i]))
			{	
				int int_value = atoi(columns[i].c_str());
				file.write((char *)(&int_value),4);
			}
			else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char" && (columns[i].substr(0,1) != "'" || columns[i].substr(columns[i].size()-1,1) != "'"))
			{
				cout<<"Character values should be enclosed within two single quotes"<<endl;
				output.push_back("Character values should be enclosed within two single quotes\n");
				return 0;
			}
			else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char" && (columns[i].substr(0,1) == "'" || columns[i].substr(columns[i].size()-1,1) == "'"))
			{
				char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
				char_size.erase(char_size.size()-1);
				char_size1 = atoi(char_size.c_str());
				char_value = columns[i].substr(1,columns[i].size()-1);
				char_value.erase(char_value.size()-1);
				const char * c = char_value.c_str();
				file.write((char *)(c),char_size1);
			}
			else
			{
				cout<<"Syntax Error"<<endl;
				output.push_back("Syntax Error Insert stmt\n");
				return 0;
			}
		}
		catalog_tbl[table_position].records = catalog_tbl[table_position].records+1;
		catalog_tbl[table_position].total_size = catalog_tbl[table_position].records * catalog_tbl[table_position].rec_size;
		file.close();
		
	}
	else if ( temp_cmd2 == "select")
	{
		//insert select block
		int t_pos;
		vector<string> columns;
		string select_query = temp_cmd;	
		if ( select_query.find("where") == string::npos)
		{
		bool isSuccess=select_function(remove_spaces(select_query),catalog_object);	
		}
		else
		{
			bool isSuccess=select_where_table(remove_spaces(select_query),catalog_object);
		}
		
		string char_size, char_value;
		int char_size1;
		fstream file;
		string table_name = catalog_tbl[table_position].table_name;
		table_name.append(".tbl");
		const char * c = table_name.c_str();
		file.open (c,ios::out|ios::binary|ios::app);
		for(unsigned p=0;  p<output_insert_into.size();p++)
		{
			
			columns.push_back(output_insert_into[p]);
			if( p ==  catalog_tbl[table_position].col_name.size()-1)
			{
				output_insert_into.erase(output_insert_into.begin() , output_insert_into.begin() + catalog_tbl[table_position].col_name.size());
		//Check primary key constraint data type constraint
		if(check_primary_key(columns,table_position))
		{
			cout<<"Primary Key Constraint or data type constraint in insert statement\n"<<endl;
			output.push_back("Primary Key Constraint or data type constraint in insert statement\n");
			return 0;
		}
		//data type constraint
		
		for (unsigned i = 0;i<columns.size();i++)
		{
			if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
			{
				char_size_in_constraint = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
				char_size_in_constraint.erase(char_size_in_constraint.size()-1);
				char_size1_in_constraint = atoi(char_size_in_constraint.c_str());
				char_value_in_constraint = columns[i].substr(0,columns[i].size());
				
				if(char_value_in_constraint.size() >(unsigned) char_size1_in_constraint )
				{
					cout<<"Error:Char entered is greater than char length of the assigned char field"<<endl;
					output.push_back("Error:Char entered is greater than char length of the assigned char field\n");
					return 0;
				}
			}
		}
				for (unsigned i = 0;i<columns.size();i++)
				{
					if (catalog_tbl[table_position].col_type[i] == "int" && !is_number(columns[i]))
					{
						cout<<"Data Incompatible:Non numeric values are entered in a int field"<<endl;	
						output.push_back("Data Incompatible:Non numeric values are entered in a int field\n");
						return 0;
					}
					else if (catalog_tbl[table_position].col_type[i] == "int" && is_number(columns[i]))
					{	
						int int_value = atoi(columns[i].c_str());
						file.write((char *)(&int_value),4);
					}
					
					else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char" )
					{
						char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
						char_size.erase(char_size.size()-1);
						char_size1 = atoi(char_size.c_str());
						char_value = columns[i].substr(0,columns[i].size());						
						const char * c = char_value.c_str();
						file.write((char *)(c),char_size1);
					}
					else
					{
						cout<<"Syntax Error"<<endl;
						output.push_back("Syntax Error Insert stmt\n");
						return 0;
					}
				}
		catalog_tbl[table_position].records = catalog_tbl[table_position].records+1;
		catalog_tbl[table_position].total_size = catalog_tbl[table_position].records * catalog_tbl[table_position].rec_size;
		columns.erase(columns.begin(),columns.end());
		p=-1;
		}
		
		}
		
		file.close();


	}
	else
	{
		cout<<"Syntax error:The keyword 'values' or 'select' is not specified in the insert statement"<<endl;
		output.push_back("Syntax error:The keyword 'values' or 'select' is not specified in the insert statement\n");
		return 0;
	}
	return 1;
	


}

//Select Table Function
bool select_table(string command , catalog *catalog_object)
{
	output_table.push_back(command);
	cout<<command<<endl;
	string columns,table_name,char_size;
	string temp_cmd = command;
	bool isTableFound=false;
	char char_value[100];
	int table_position,char_size1,col_int;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	if (temp_cmd.find('from') == string::npos)
	{
		cout<<"Syntax error:The keyword from in select statement is missing\n";
		output.push_back("Syntax error:The keyword from in select statement is missing\n");
		return 0;
	}	
	columns = temp_cmd.substr(0,temp_cmd.find("from"));
	columns = remove_spaces(columns);
	temp_cmd.erase(0, temp_cmd.find("from")+5);	
	temp_cmd.erase(temp_cmd.find(";"));
		
	table_name = temp_cmd.substr(0,temp_cmd.find(' '));
	for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_name == catalog_tbl[i].table_name)
		{
			isTableFound = true;
			table_position = i;
		}

	}
	if (!isTableFound)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
	
	if(columns.substr(0,1) == "*" && columns.size() == 1)
	{
		output_table.push_back(catalog_tbl[table_position].table_name+"\n");
		for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
		{
			cout.width(15);
			cout.setf ( ios_base::left);
			cout<<catalog_tbl[table_position].col_name[i];
			output_table.push_back(catalog_tbl[table_position].col_name[i] );
			output_table.push_back("\t");
		}
		cout<<endl;		
		output_table.push_back("\n");
		ifstream select;
		string table_name1 = table_name.append(".tbl");
		const char * c = table_name1.c_str();		
		select.open (c,ios::in|ios::binary|ios::ate);
		if(select.is_open())
		{
			select.seekg (0, ios::end);
			unsigned size = select.tellg();
			select.seekg (0, ios::beg);
			for (unsigned j=0;j<size;j=j+catalog_tbl[table_position].rec_size)
					{
						for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
						{
							if (catalog_tbl[table_position].col_type[i] == "int")
							{
								select.read((char *)(&col_int),4);
								cout.width(15);
								cout.setf ( ios_base::left);
								cout<<col_int;
								stringstream convert; 
								convert << col_int;
								string col_int = convert.str();
								output_table.push_back(col_int);
								output_insert_into.push_back(col_int);
								output_table.push_back("\t");
							}
							else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
							{
								char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
								char_size.erase(char_size.size()-1);
								char_size1 = atoi(char_size.c_str());								
								select.read((char *)(&char_value),char_size1);
								cout.width(15);
								cout.setf ( ios_base::left);
								char_value[char_size1] = '\0';
								cout<<char_value;
								output_table.push_back(char_value);
								output_insert_into.push_back(char_value);
								output_table.push_back("\t");
							}
						}
						cout<<endl;
						output_table.push_back("\n");
		}
		select.close();
		output_table.push_back("\n");
	}
	}
	else 
	{
		//select stmt with column names
		vector<string> column_spilt;
		bool isValidColumn =false;
		split(columns,',',column_spilt);
		for(unsigned k=0;k < column_spilt.size();k++)
		{
			column_spilt[k]=remove_spaces(column_spilt[k]);
		}
		for(unsigned x=0;x < column_spilt.size(); x++)		
		{
			isValidColumn =false;
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
			if ( column_spilt[x] == catalog_tbl[table_position].col_name[i] )
			isValidColumn = true;
			}
			if ( isValidColumn == false)
					{
						cout<<"Column name given in select statement is not present in table"<<endl;
						output.push_back("Column name given in select statement is not present in table");
						return 0;
					}
		}
		for(unsigned y=0;y < column_spilt.size(); y++)		
		{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<column_spilt[y];
					output_table.push_back(column_spilt[y]);
					output_table.push_back("\t");
		}
		cout<<endl;		
		output_table.push_back("\n");
		ifstream select;
		string table_name1 = table_name.append(".tbl");
		const char * c = table_name1.c_str();		
		select.open (c,ios::in|ios::binary|ios::ate);
		if(select.is_open())
		{
			select.seekg (0, ios::end);
			unsigned size = select.tellg();
			select.seekg (0, ios::beg);
			for (unsigned j=0;j<size;j=j+catalog_tbl[table_position].rec_size)
					{
						for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
						{
							if (catalog_tbl[table_position].col_type[i] == "int")
							{
								select.read((char *)(&col_int),4);
								cout.width(15);
								cout.setf ( ios_base::left);
								for(unsigned m = 0;m<column_spilt.size();m++)
								{
									if (catalog_tbl[table_position].col_name[i] == column_spilt[m]) 
									{
									cout<<col_int;
									stringstream convert; 
									convert << col_int;
									string col_int = convert.str();
									output_table.push_back(col_int);
									output_insert_into.push_back(col_int);
									output_table.push_back("\t");
									}
								}
							}
							else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
							{
								char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
								char_size.erase(char_size.size()-1);
								char_size1 = atoi(char_size.c_str());								
								select.read((char *)(&char_value),char_size1);
								cout.width(15);
								cout.setf ( ios_base::left);
								char_value[char_size1] = '\0';
								for(unsigned m = 0;m<column_spilt.size();m++)
								{
									if (catalog_tbl[table_position].col_name[i] == column_spilt[m]) 
									{
									cout<<char_value;
									output_table.push_back(char_value);
									output_insert_into.push_back(char_value);
									output_table.push_back("\t");
									}
								}
							}
						}
						cout<<endl;
						output_table.push_back("\n");
		}
		select.close();
		output_table.push_back("\n");
	}


	}
return 1;
}

//Select Table Join Function
bool select_join_table(string command , catalog *catalog_object)
{
	output_table.push_back(command);
	string columns,table_name1,table_name2,char_size;
	string temp_cmd = command;
	bool isTableFound1=false,isTableFound2=false;
	char char_value[100];
	int table_position1,table_position2,char_size1,col_int;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	if (temp_cmd.find('from') == string::npos)
	{
		cout<<"Syntax error:The keyword from in select statement is missing\n";
		output.push_back("Syntax error:The keyword from in select statement is missing\n");
		return 0;
	}	
	columns = temp_cmd.substr(0,temp_cmd.find("from"));
	columns = remove_spaces(columns);
	temp_cmd.erase(0, temp_cmd.find("from")+5);	
	temp_cmd.erase(temp_cmd.find(";"));
		
	table_name1 = temp_cmd.substr(0,temp_cmd.find(' '));
	for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_name1 == catalog_tbl[i].table_name)
		{
			isTableFound1 = true;
			table_position1 = i;
		}

	}
	if (!isTableFound1)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);	
	string join;
	join =  temp_cmd.substr(0,temp_cmd.find(' '));
	if ( join != "join")
	{
		cout<<"Syntax Error: Join keyword not in proper format"<<endl;		
		output.push_back("Join keyword not in proper format\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	
	table_name2 =  temp_cmd.substr(0,temp_cmd.find(' '));
	for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_name2 == catalog_tbl[i].table_name)
		{
			isTableFound2 = true;
			table_position2 = i;
		}

	}
	if (!isTableFound2)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	string on;
	on = temp_cmd.substr(0,temp_cmd.find(' '));
	if ( on != "on")
	{
		cout<<"Syntax Error: on keyword in select Join stmt not in proper format"<<endl;		
		output.push_back("on keyword in select Join stmt  not in proper format\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	string column1 = temp_cmd.substr(0,temp_cmd.find('='));
	column1 = remove_spaces(column1);
	temp_cmd.erase(0,temp_cmd.find('=')+1);
	temp_cmd = remove_spaces(temp_cmd);
	string column2 =  temp_cmd.substr(0,temp_cmd.find(' '));
	column2 = remove_spaces(column2);
	bool isValidcolumn1_2 = false,isValidcolumn1_1 =false;
	int column1_position, column2_position;
	int column1_table_position,column2_table_position;
	//Checking whether the column1 is present
	for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
	{
		if(catalog_tbl[table_position1].col_name[i] == column1)
			{
				column1 = table_name1+"."+column1;
				isValidcolumn1_1 = true;
				column1_position = i;
				column1_table_position = table_position1;
			}
	}
	for (unsigned i = 0;i<catalog_tbl[table_position2].col_name.size();i++)
	{
		if(catalog_tbl[table_position2].col_name[i] == column1)
			{
				column1 = table_name2+"."+column1;
				isValidcolumn1_2 = true;
				column1_position = i;
				column1_table_position = table_position2;
			}
	}
	if ((!isValidcolumn1_1) && (!isValidcolumn1_2))
	{
			cout<<"Column name given in ON condition doesnt exist in the tables specified"<<endl;
			output.push_back("Column name given in ON condition doesnt exist in the tables specified");
			return 0;
	}
	//Checking Ambigious Column
	if ((isValidcolumn1_1) && (isValidcolumn1_2))
	{
		cout<<"Column name given in ON condition is ambiguous"<<endl;
		output.push_back("Column name given in ON condition is ambiguous");
		return 0;
	}
	bool isValidcolumn2_1=false,isValidcolumn2_2 =false;
	//Checking whether the column2 is present
	for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
	{
		if(catalog_tbl[table_position1].col_name[i] == column2)
			{
				column2 = table_name1+"."+column2;
				isValidcolumn2_1 = true;
				column2_position = i;
				column2_table_position = table_position1;
			}
	}
	for (unsigned i = 0;i<catalog_tbl[table_position2].col_name.size();i++)
	{
		if(catalog_tbl[table_position2].col_name[i] == column2)
			{
				column2 = table_name2+"."+column2;
				isValidcolumn2_2 = true;
				column2_position = i;
				column2_table_position = table_position2;
			}
	}
	if ((!isValidcolumn2_1) && (!isValidcolumn2_2))
	{
			cout<<"Column name given in ON condition doesnt exist in the tables specified"<<endl;
			output.push_back("Column name given in ON condition doesnt exist in the tables specified");
			return 0;
	}
	//Checking Ambigious Column
	if ((isValidcolumn2_1) && (isValidcolumn2_2))
	{
		cout<<"Column name given in ON condition is ambiguous"<<endl;
		output.push_back("Column name given in ON condition is ambiguous");
		return 0;
	}
	
	string column1_tablename,column2_tablename;	
	column1_tablename = column1.substr(0,column1.find('.'));
	column2_tablename = column2.substr(0,column2.find('.'));

	if (catalog_tbl[column1_table_position].col_type[column1_position] != catalog_tbl[column2_table_position].col_type[column2_position])
	{
		cout<<"Error:The Join condition columns are of different datatypes"<<endl;
		output.push_back("Error:The Join condition columns are of different datatypes");
		return 0;
	}
	ifstream table1;
	ifstream table2;
	string temp_table1=table_name1;
	temp_table1.append(".tbl");
	string temp_table2 =table_name2;
	temp_table2.append(".tbl");
	const char * c1 = temp_table1.c_str();
	const char * c2 = temp_table2.c_str();
	if(columns.substr(0,1) == "*" && columns.size() == 1)
	{
		// Select * Block
		for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
			{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<catalog_tbl[table_position1].col_name[i];
					output_table.push_back(catalog_tbl[table_position1].col_name[i]);
					output_table.push_back("\t");
			}
		for (unsigned i = 0;i<catalog_tbl[table_position2].col_name.size();i++)
			{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<catalog_tbl[table_position2].col_name[i];
					output_table.push_back(catalog_tbl[table_position2].col_name[i]);
					output_table.push_back("\t");
			}
		cout<<endl;		
		output_table.push_back("\n");
	table1.open (c1,ios::in|ios::binary|ios::ate);
	if(table1.is_open())
	{
		table1.seekg (0, ios::end);
		unsigned size1 = table1.tellg();
		table1.seekg (0, ios::beg);
		vector<string> table1_row;
		vector<string> table2_row;
		for (int j=0;j<size1;j=j+catalog_tbl[table_position1].rec_size)
		{
			for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
			{
				if (catalog_tbl[table_position1].col_type[i] == "int")
				{
					table1.read((char *)(&col_int),4);
					stringstream convert; 
					convert << col_int;
					string col_int = convert.str();
					table1_row.push_back(col_int);					
				}
				else if (catalog_tbl[table_position1].col_type[i].substr(0,4) == "char")
				{
					char_size = catalog_tbl[table_position1].col_type[i].substr(5,catalog_tbl[table_position1].col_type[i].size()-1);
					char_size.erase(char_size.size()-1);
					char_size1 = atoi(char_size.c_str());
					table1.read((char *)(&char_value),char_size1);
					char_value[char_size1] = '\0';
					table1_row.push_back(char_value);					
				}
			}
			table2.open (c2,ios::in|ios::binary|ios::ate);
			int size2 = (int)table2.tellg();			
			table2.seekg (0, ios::beg);
			for (int k=0;k<size2;k=k+catalog_tbl[table_position2].rec_size)
			{
				for (unsigned l = 0;l<catalog_tbl[table_position2].col_name.size();l++)
				{
					if (catalog_tbl[table_position2].col_type[l] == "int")
					{
						table2.read((char *)(&col_int),4);
						stringstream convert; 
						convert << col_int;
						string col_int = convert.str();
						table2_row.push_back(col_int);		
					}
					else if (catalog_tbl[table_position2].col_type[l].substr(0,4) == "char")
					{
						char_size = catalog_tbl[table_position2].col_type[l].substr(5,catalog_tbl[table_position2].col_type[l].size()-1);
						char_size.erase(char_size.size()-1);
						char_size1 = atoi(char_size.c_str());
						table2.read((char *)(&char_value),char_size1);
						char_value[char_size1] = '\0';
						table2_row.push_back(char_value);
					}
				}
				if(table1_row[column1_position] == table2_row[column2_position])
				{
					for(unsigned n =0;n<table2_row.size();n++)
					{
						table1_row.push_back(table2_row[n]);
					}		
					for(unsigned f=0;f< table1_row.size(); f++)
					{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<table1_row[f];
					output_table.push_back(table1_row[f]);
					output_insert_into.push_back(table1_row[f]);
					output_table.push_back("\t");
					}
					output_table.push_back("\n");
					cout<<endl;
				}
				table2_row.clear();
			}
			table2.close();
			table1_row.clear();
		}
	}
	output_table.push_back("\n");
	cout<<endl;
	table1.close();
	}
	else
	{
		// select with column name block
		vector<string> column_spilt;
		bool isValidColumn =false;
		split(columns,',',column_spilt);
		for(unsigned k=0;k < column_spilt.size();k++)
		{
			column_spilt[k]=remove_spaces(column_spilt[k]);
		}
		for(unsigned x=0;x < column_spilt.size(); x++)		
		{
			isValidColumn =false;
			for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
			{
			if ( column_spilt[x] == catalog_tbl[table_position1].col_name[i] )
			isValidColumn = true;
			}
			for (unsigned i = 0;i<catalog_tbl[table_position2].col_name.size();i++)
			{
			if ( column_spilt[x] == catalog_tbl[table_position2].col_name[i] )
			isValidColumn = true;
			}
			if ( isValidColumn == false)
					{
						cout<<"Column name given in select statement is not present in table"<<endl;
						output.push_back("Column name given in select statement is not present in table");
						return 0;
					}
		}
		for(unsigned y=0;y < column_spilt.size(); y++)		
		{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<column_spilt[y];
					output_table.push_back(column_spilt[y]);
					output_table.push_back("\t");
		}
		cout<<endl;		
		output_table.push_back("\n");
		table1.open (c1,ios::in|ios::binary|ios::ate);
	if(table1.is_open())
	{
		table1.seekg (0, ios::end);
		unsigned size1 = table1.tellg();
		table1.seekg (0, ios::beg);
		vector<string> table1_row;
		vector<string> table2_row;
		for (int j=0;j<size1;j=j+catalog_tbl[table_position1].rec_size)
		{
			for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
			{
				if (catalog_tbl[table_position1].col_type[i] == "int")
				{
					table1.read((char *)(&col_int),4);
					stringstream convert; 
					convert << col_int;
					string col_int = convert.str();
					table1_row.push_back(col_int);					
				}
				else if (catalog_tbl[table_position1].col_type[i].substr(0,4) == "char")
				{
					char_size = catalog_tbl[table_position1].col_type[i].substr(5,catalog_tbl[table_position1].col_type[i].size()-1);
					char_size.erase(char_size.size()-1);
					char_size1 = atoi(char_size.c_str());
					table1.read((char *)(&char_value),char_size1);
					char_value[char_size1] = '\0';
					table1_row.push_back(char_value);					
				}
			}
			table2.open (c2,ios::in|ios::binary|ios::ate);
			int size2 = (int)table2.tellg();			
			table2.seekg (0, ios::beg);
			for (int k=0;k<size2;k=k+catalog_tbl[table_position2].rec_size)
			{
				for (unsigned l = 0;l<catalog_tbl[table_position2].col_name.size();l++)
				{
					if (catalog_tbl[table_position2].col_type[l] == "int")
					{
						table2.read((char *)(&col_int),4);
						stringstream convert; 
						convert << col_int;
						string col_int = convert.str();
						table2_row.push_back(col_int);		
					}
					else if (catalog_tbl[table_position2].col_type[l].substr(0,4) == "char")
					{
						char_size = catalog_tbl[table_position2].col_type[l].substr(5,catalog_tbl[table_position2].col_type[l].size()-1);
						char_size.erase(char_size.size()-1);
						char_size1 = atoi(char_size.c_str());
						table2.read((char *)(&char_value),char_size1);
						char_value[char_size1] = '\0';
						table2_row.push_back(char_value);
					}
				}
				if(table1_row[column1_position] == table2_row[column2_position])
				{
					for(unsigned n =0;n<table2_row.size();n++)
					{
						table1_row.push_back(table2_row[n]);
					}	
					vector<string> column_spilt1 = column_spilt;
					for(unsigned f=0;f< table1_row.size(); f++)
					{						
						
						for(unsigned m = 0;m<column_spilt1.size();m++)
								{
									bool column_output =false;
									for (unsigned i = 0;i<catalog_tbl[table_position1].col_name.size();i++)
									{
									if (catalog_tbl[table_position1].col_name[i] == column_spilt1[m]) 
									{
									cout.width(15);
									cout.setf ( ios_base::left);
									cout<<table1_row[i];
									
									output_table.push_back(table1_row[i]);
									output_insert_into.push_back(table1_row[i]);
									column_output = true;
									}
									}
									for (unsigned i = 0;i<catalog_tbl[table_position2].col_name.size();i++)
									{
									if (catalog_tbl[table_position2].col_name[i] == column_spilt1[m]) 
									{
									cout.width(15);
									cout.setf ( ios_base::left);
									int no_of_columns_in_1_table = i + (catalog_tbl[table_position1].col_name.size());
									cout<<table1_row[no_of_columns_in_1_table];
									output_table.push_back(table1_row[no_of_columns_in_1_table]);
									output_insert_into.push_back(table1_row[no_of_columns_in_1_table]);
									output_table.push_back("\t");
									column_output = true;
									}
									}
									column_spilt1.erase(column_spilt1.begin());
									if ( column_output == true)
									{
										break;
									}									
									
						        }
					}
					output_table.push_back("\n");
					cout<<endl;
				}
				table2_row.clear();
			}
			table2.close();
			table1_row.clear();
		}
	}
	output_table.push_back("\n");
	cout<<endl;
	table1.close();
	}
 return 1;
}

//Select where Block
bool select_where_table(string command , catalog *catalog_object)
{
	output_table.push_back(command);
	string columns,table_name,char_size,cond1,cond2,c1,c2,col1,col2,val1,val2,column1,column2,symbol1,symbol2,conj;
	string temp_cmd = command;
	bool isTableFound=false;
	char char_value[100];
	int table_position,char_size1,col_int,col1_pos,col2_pos;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	if (temp_cmd.find('from') == string::npos)
	{
		cout<<"Syntax error:The keyword from in select statement is missing\n";
		output.push_back("Syntax error:The keyword from in select statement is missing\n");
		return 0;
	}	
	columns = temp_cmd.substr(0,temp_cmd.find("from"));
	columns = remove_spaces(columns);
	temp_cmd.erase(0, temp_cmd.find("from")+5);	
	temp_cmd.erase(temp_cmd.find(";"));
		
	table_name = temp_cmd.substr(0,temp_cmd.find(' '));
	for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_name == catalog_tbl[i].table_name)
		{
			isTableFound = true;
			table_position = i;
		}

	}
	if (!isTableFound)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
	
	temp_cmd.erase(0, temp_cmd.find("where")+6);
	
	//check if 'and' or 'or' is present
	if(temp_cmd.find("and") != string::npos || temp_cmd.find("or") != string::npos)
	{
		if(temp_cmd.find("and") != string::npos)
		{
			conj = "and";
			cond1 = temp_cmd.substr(0,temp_cmd.find("and"));
			c1 = remove_spaces(cond1);
			if(cond1.find(">") != string::npos)
			{
				col1 = cond1.substr(0, cond1.find(">"));
				symbol1 = ">";
			}
			if(cond1.find("=") != string::npos)
			{
				col1 = cond1.substr(0, cond1.find("="));
				symbol1 = "=";
			}
			if(cond1.find("<") != string::npos)
			{
				col1 = cond1.substr(0, cond1.find("<"));
				symbol1 = "<";
			}
			col1 = remove_spaces(col1);
			//Checking whether the column in condition1 is present
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if(catalog_tbl[table_position].col_name[i] == col1)
				{
					col1_pos = i;
					column1 = table_name+"."+col1;
					//isValidcolumn1_1 = true;
					//column1_position = i;
					//column1_table_position = table_position1;
					cond1.erase(0, cond1.find(symbol1));
					val1 = cond1.substr(0, temp_cmd.find("and"));
					//val1 = cond1.substr(symbol1+1, temp_cmd.find("or"));
				}
				else
				{
					cout << "Error: column1 specified in where condition is not present in the table" << endl;
				}
			}
			temp_cmd.erase(0, temp_cmd.find("and")+4);
			cond2 = temp_cmd.substr(0, temp_cmd.find(' '));
			c2 = remove_spaces(cond2);
			if(cond2.find(">"))
			{
				col2 = cond2.substr(0, cond2.find(">"));
				symbol2 = ">";
			}
			if(cond2.find("="))
			{
				col2 = cond2.substr(0, cond2.find("="));
				symbol2 = "=";
			}
			if(cond2.find("<"))
			{
				col2 = cond2.substr(0, cond2.find("<"));
				symbol2 = "<";
			}
			col2 = remove_spaces(col2);
			//Checking whether the column in condition2 is present
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if(catalog_tbl[table_position].col_name[i] == col2)
				{
					col2_pos = i;
					column2 = table_name+"."+col2;
					//isValidcolumn1_1 = true;
					//column1_position = i;
					//column1_table_position = table_position1;
					cond2.erase(0,cond2.find(symbol2));
					val2 = cond2.substr(0, cond2.find(' '));
				}
				else
				{
					cout << "Error: column2 specified in where condition is not present in the table" << endl;
				}
			}
		}
		else
		{
			conj = "or";
			cond1 = temp_cmd.substr(0,temp_cmd.find("or"));
			c1 = remove_spaces(cond1);
			if(cond1.find(">"))
			{
				col1 = cond1.substr(0, cond1.find(">"));
				symbol1 = ">";
			}
			if(cond1.find("="))
			{
				col1 = cond1.substr(0, cond1.find("="));
				symbol1 = "=";
			}
			if(cond1.find("<"))
			{
				col1 = cond1.substr(0, cond1.find("<"));
				symbol1 = "<";
			}
			col1 = remove_spaces(col1);
			//Checking whether the column in condition1 is present
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if(catalog_tbl[table_position].col_name[i] == col1)
				{
					col1_pos = i;
					column1 = table_name+"."+col1;
					//isValidcolumn1_1 = true;
					//column1_position = i;
					//column1_table_position = table_position1;
					cond1.erase(0, cond1.find(symbol1));
					//val1 = cond1.substr(symbol1+1, temp_cmd.find("and"));
					val1 = cond1.substr(0, temp_cmd.find("or"));
				}
				else
				{
					cout << "Error: column1 specified in where condition is not present in the table" << endl;
				}
			}
			temp_cmd.erase(0, temp_cmd.find("or")+3);
			cond2 = temp_cmd.substr(0, temp_cmd.find(' '));
			c2 = remove_spaces(cond2);
			if(cond2.find(">"))
			{
				col2 = cond2.substr(0, cond2.find(">"));
				symbol2 = ">";
			}
			if(cond2.find("="))
			{
				col2 = cond2.substr(0, cond2.find("="));
				symbol2 = "=";
			}
			if(cond2.find("<"))
			{
				col2 = cond2.substr(0, cond2.find("<"));
				symbol2 = "<";
			}
			col2 = remove_spaces(col2);
			//Checking whether the column in condition2 is present
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
				if(catalog_tbl[table_position].col_name[i] == col2)
				{
					col2_pos = i;
					column2 = table_name+"."+col2;
					//isValidcolumn1_1 = true;
					//column1_position = i;
					//column1_table_position = table_position1;
					cond2.erase(0,cond2.find(symbol2));
					val2 = cond2.substr(0, cond2.find(' '));
				}
				else
				{
					cout << "Error: column2 specified in where condition is not present in the table" << endl;
				}
			}
		}				
	}
	else
	{
		//when there is only one conditional statement after 'where' clause
		
		cond1 = temp_cmd;
		c1 = remove_spaces(cond1);
		if(cond1.find(">") != string::npos  )
			{
				col1 = cond1.substr(0, cond1.find(">"));
				symbol1 = ">";
			}
			if(cond1.find("=") != string::npos  )
			{
				col1 = cond1.substr(0, cond1.find("="));
				symbol1 = "=";
			}
			if(cond1.find("<") != string::npos )
			{
				col1 = cond1.substr(0, cond1.find("<"));
				symbol1 = "<";
			}
		col1 = remove_spaces(col1);
		//Checking whether the column in given condition is present
		bool isValidcolumn1_1 = false;
		int column1_position;
		for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
		{
			if(catalog_tbl[table_position].col_name[i] == col1)
				{
					col1_pos = i;
					column1 = table_name+"."+col1;
					isValidcolumn1_1 = true;
					column1_position = i;
					//column1_table_position = table_position1;
					cond1.erase(0,cond1.find(symbol1)+1);
					val1 = cond1.substr(0, cond1.size());
					val1 = remove_spaces(val1);
				}
			
		}
		if( ! isValidcolumn1_1 )
		{
			 cout<< " Column Name in where condition is not present \n " ;
			 output_table.push_back("Column Name in where condition is not present \n");
			 return 0;
		}
	}
	
	
	vector<string> col_values;
	//select * table name
	if(columns.substr(0,1) == "*" && columns.size() == 1)
	{
		output_table.push_back(catalog_tbl[table_position].table_name+"\n");
		for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
		{
			cout.width(15);
			cout.setf ( ios_base::left);
			cout<<catalog_tbl[table_position].col_name[i];
			output_table.push_back(catalog_tbl[table_position].col_name[i] );
			output_table.push_back("\t");
		}
		cout<<endl;		
		output_table.push_back("\n");
		ifstream select;
		bool is_clause= false;
		string table_name1 = table_name.append(".tbl");
		const char * c = table_name1.c_str();		
		select.open (c,ios::in|ios::binary|ios::ate);
		if(select.is_open())
		{
			select.seekg (0, ios::end);
			unsigned size = select.tellg();
			select.seekg (0, ios::beg);
			for (unsigned j=0;j<size;j=j+catalog_tbl[table_position].rec_size)
					{
						 is_clause= false;
						for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
						{
							if (catalog_tbl[table_position].col_type[i] == "int")
							{
								select.read((char *)(&col_int),4);
								cout.width(15);
								cout.setf ( ios_base::left);
								if ( conj != "and" || conj != "or" )
								{
								// Single condition Block
								if( symbol1 == "=" )
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int == atoi(val1.c_str()))
									is_clause = true;
								}
								if( symbol1 == ">")
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int > atoi(val1.c_str()))
									is_clause = true;
								}
								if( symbol1 == "<")
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int < atoi(val1.c_str()))
									is_clause = true;
								}
								}
								//cout<<col_int;
								stringstream convert; 
								convert << col_int;
								string col_int = convert.str();
								/*output_table.push_back(col_int);
								output_insert_into.push_back(col_int);*/
								col_values.push_back(col_int);
								/*output_table.push_back("\t");*/
							}
							else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
							{
								char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
								char_size.erase(char_size.size()-1);
								char_size1 = atoi(char_size.c_str());								
								select.read((char *)(&char_value),char_size1);
								cout.width(15);
								cout.setf ( ios_base::left);
								char_value[char_size1] = '\0';
								
								if ( conj != "and" || conj != "or" )
								{
								// Single condition Block
								if( symbol1 == "=" )
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && char_value == val1.substr(1,val1.size()-1))
									is_clause = true;
								}
								}
								//cout<<char_value;
								//output_table.push_back(char_value);
								//output_insert_into.push_back(char_value);
								col_values.push_back(char_value);
								//output_table.push_back("\t");
							}
						}
						if (is_clause)
						{
							
							for( unsigned kl = 0 ;kl < col_values.size() ; kl++)
							{
								cout<<col_values[kl]<<"\t";
								output_table.push_back(col_values[kl]);
								output_insert_into.push_back(col_values[kl]);
								output_table.push_back("\t");
							}
						}
						col_values.erase(col_values.begin(),col_values.end());
						cout<<endl;
						output_table.push_back("\n");
		}
		select.close();
		output_table.push_back("\n");
	}
	}
	else 
	{
		//select stmt with column names
		vector<string> column_spilt;
		bool isValidColumn =false;
		split(columns,',',column_spilt);
		for(unsigned k=0;k < column_spilt.size();k++)
		{
			column_spilt[k]=remove_spaces(column_spilt[k]);
		}
		for(unsigned x=0;x < column_spilt.size(); x++)		
		{
			isValidColumn =false;
			for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
			{
			if ( column_spilt[x] == catalog_tbl[table_position].col_name[i] )
			isValidColumn = true;
			}
			if ( isValidColumn == false)
					{
						cout<<"Column name given in select statement is not present in table"<<endl;
						output.push_back("Column name given in select statement is not present in table");
						return 0;
					}
		}
		for(unsigned y=0;y < column_spilt.size(); y++)		
		{
					cout.width(15);
					cout.setf ( ios_base::left);
					cout<<column_spilt[y];
					output_table.push_back(column_spilt[y]);
					output_table.push_back("\t");
		}
		cout<<endl;		
		output_table.push_back("\n");
		ifstream select;
		string table_name1 = table_name.append(".tbl");
		bool is_clause = false;
		const char * c = table_name1.c_str();		
		select.open (c,ios::in|ios::binary|ios::ate);
		if(select.is_open())
		{
			select.seekg (0, ios::end);
			unsigned size = select.tellg();
			select.seekg (0, ios::beg);
			for (unsigned j=0;j<size;j=j+catalog_tbl[table_position].rec_size)
					{
						is_clause = false;
						for (unsigned i = 0;i<catalog_tbl[table_position].col_name.size();i++)
						{
							if (catalog_tbl[table_position].col_type[i] == "int")
							{
								select.read((char *)(&col_int),4);
								cout.width(15);
								cout.setf ( ios_base::left);
								for(unsigned m = 0;m<column_spilt.size();m++)
								{
									if (catalog_tbl[table_position].col_name[i] == column_spilt[m]) 
									{

									//cout<<col_int;
								if ( conj != "and" || conj != "or" )
								
								{
								// Single condition Block
								if( symbol1 == "=" )
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int == atoi(val1.c_str()))
									is_clause = true;
								}
								if( symbol1 == ">")
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int > atoi(val1.c_str()))
									is_clause = true;
								}
								if( symbol1 == "<")
								{
									if(catalog_tbl[table_position].col_name[i] == col1 && col_int < atoi(val1.c_str()))
									is_clause = true;
								}
								}
									stringstream convert; 
									convert << col_int;
									string col_int = convert.str();
									//output_table.push_back(col_int);
									//output_insert_into.push_back(col_int);
									//output_table.push_back("\t");
									col_values.push_back(col_int);
									}
								}
							}
							else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
							{
								char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
								char_size.erase(char_size.size()-1);
								char_size1 = atoi(char_size.c_str());								
								select.read((char *)(&char_value),char_size1);
								cout.width(15);
								cout.setf ( ios_base::left);
								char_value[char_size1] = '\0';
								for(unsigned m = 0;m<column_spilt.size();m++)
								{
									if (catalog_tbl[table_position].col_name[i] == column_spilt[m]) 
									{
										if ( conj != "and" || conj != "or" )
											{
											// Single condition Block
											if( symbol1 == "=" )
											{
												if(catalog_tbl[table_position].col_name[i] == col1 && char_value == val1.substr(1,val1.size()-1))
												is_clause = true;
											}
											}
									/*cout<<char_value;
									output_table.push_back(char_value);
									output_insert_into.push_back(char_value);
									output_table.push_back("\t");*/
										col_values.push_back(char_value);
									}
								}
							}
						}
						if (is_clause)
						{
							
							for( unsigned kl = 0 ;kl < col_values.size() ; kl++)
							{
								cout<<col_values[kl]<<"\t";
								output_table.push_back(col_values[kl]);
								output_insert_into.push_back(col_values[kl]);
								output_table.push_back("\t");
							}
						}
						col_values.erase(col_values.begin(),col_values.end());
						cout<<endl;
						output_table.push_back("\n");
		}
		select.close();
		output_table.push_back("\n");
	}


	}
	return 1;
}

//select into table function
bool select_into_table(string command , catalog *catalog_object)
{
	cout<<command<<endl;
  string temp_cmd1 = command;
  string temp_cmd2 = command;
  int char_size1_in_constraint;
  string char_size_in_constraint,char_value_in_constraint;
  temp_cmd2.erase(0,temp_cmd2.find("#"));
  string new_table_name =  temp_cmd2.substr(0,temp_cmd2.find("from"));
  int found_into= temp_cmd1.find("into") ; 
  temp_cmd1.erase(found_into, 5 );  
   
  new_table_name = remove_spaces(new_table_name);
  if ( new_table_name.substr(0,1) !="#")
  {
	  cout<<"Select Into can be done in temporary table , # symbol missing"<<endl;
	  output.push_back("Select Into can be done in temporary table , # symbol missing");
	  return 0;
  }
  temp_cmd1.erase(found_into,new_table_name.size());
  string temp_cmd = remove_spaces(temp_cmd1);
  string   create_cmd = "create table " + new_table_name + "(" ;
  int table_position123;
  bool isTableFound123;
  string temp_cmd3 =temp_cmd1;
  temp_cmd3.erase(0, temp_cmd3.find(' ')+1);
  temp_cmd3.erase(0, temp_cmd3.find("from")+5);	
  temp_cmd3.erase(temp_cmd3.find(";"));
  string table_select = temp_cmd3.substr(0,temp_cmd3.find(' '));
  for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_select == catalog_tbl[i].table_name)
		{
			isTableFound123 = true;
			table_position123 = i;
		}

	}
	if (!isTableFound123)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
  for(unsigned i=0;i< catalog_tbl[table_position123].col_name.size() ; i++)
  {	  
	  if(i == catalog_tbl[table_position123].col_name.size()-1)
	   create_cmd +=  catalog_tbl[table_position123].col_name[i] + " " +  catalog_tbl[table_position123].col_type[i];
	  else
	  create_cmd +=  catalog_tbl[table_position123].col_name[i] + " " +  catalog_tbl[table_position123].col_type[i] + ",";
  }
  create_cmd += ");";
  create_table(create_cmd, catalog_object);
		int t_pos;
		vector<string> columns;
		string select_query = temp_cmd;	
		if ( select_query.find("where") == string::npos)
		{
		bool isSuccess=select_function(remove_spaces(select_query),catalog_object);	
		}
		else
		{
			bool isSuccess=select_where_table(remove_spaces(select_query),catalog_object);
		}
		string char_size, char_value;
		int char_size1;
		fstream file;
		int table_position;
		bool isTableFound = false;
		for (unsigned i = 0;i<catalog_tbl.size();i++)
		{
		if(new_table_name == catalog_tbl[i].table_name)
		{
			isTableFound = true;
			table_position = i;
		}
	
		}
		if (!isTableFound)
		{
			cout<<"Table not found"<<endl;		
			output.push_back("Table not found\n");
			return 0;
		}
		string table_name = catalog_tbl[table_position].table_name;
		table_name.append(".tbl");
		const char * c = table_name.c_str();
		file.open (c,ios::out|ios::binary|ios::app);
		for(unsigned p=0;  p<output_insert_into.size();p++)
		{
			
			columns.push_back(output_insert_into[p]);
			if( p ==  catalog_tbl[table_position].col_name.size()-1)
			{
				output_insert_into.erase(output_insert_into.begin() , output_insert_into.begin() + catalog_tbl[table_position].col_name.size());
		//Check primary key constraint data type constraint
		if(check_primary_key(columns,table_position))
		{
			cout<<"Primary Key Constraint or data type constraint in insert statement\n"<<endl;
			output.push_back("Primary Key Constraint or data type constraint in insert statement\n");
			return 0;
		}
		//data type constraint
		
		for (unsigned i = 0;i<columns.size();i++)
		{
			if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char")
			{
				char_size_in_constraint = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
				char_size_in_constraint.erase(char_size_in_constraint.size()-1);
				char_size1_in_constraint = atoi(char_size_in_constraint.c_str());
				char_value_in_constraint = columns[i].substr(0,columns[i].size());
				
				if(char_value_in_constraint.size() >(unsigned) char_size1_in_constraint )
				{
					cout<<"Error:Char entered is greater than char length of the assigned char field"<<endl;
					output.push_back("Error:Char entered is greater than char length of the assigned char field\n");
					return 0;
				}
			}
		}
				for (unsigned i = 0;i<columns.size();i++)
				{
					if (catalog_tbl[table_position].col_type[i] == "int" && !is_number(columns[i]))
					{
						cout<<"Data Incompatible:Non numeric values are entered in a int field"<<endl;	
						output.push_back("Data Incompatible:Non numeric values are entered in a int field\n");
						return 0;
					}
					else if (catalog_tbl[table_position].col_type[i] == "int" && is_number(columns[i]))
					{	
						int int_value = atoi(columns[i].c_str());
						file.write((char *)(&int_value),4);
					}
					
					else if (catalog_tbl[table_position].col_type[i].substr(0,4) == "char" )
					{
						char_size = catalog_tbl[table_position].col_type[i].substr(5,catalog_tbl[table_position].col_type[i].size()-1);
						char_size.erase(char_size.size()-1);
						char_size1 = atoi(char_size.c_str());
						char_value = columns[i].substr(0,columns[i].size());						
						const char * c = char_value.c_str();
						file.write((char *)(c),char_size1);
					}
					else
					{
						cout<<"Syntax Error"<<endl;
						output.push_back("Syntax Error Insert stmt\n");
						return 0;
					}
				}
		catalog_tbl[table_position].records = catalog_tbl[table_position].records+1;
		catalog_tbl[table_position].total_size = catalog_tbl[table_position].records * catalog_tbl[table_position].rec_size;
		columns.erase(columns.begin(),columns.end());
		p=-1;
		}
		
		}
		
		file.close();

  return 1;
}

//select main function
bool select_function(string command,catalog *catalog_object)
{
	size_t found_join=command.find("join");
	size_t found_where=command.find("where");
	size_t found_into= command.find("into");
	if(found_into != string::npos)
	{
		bool isSuccess = select_into_table ( command , catalog_object);
		if(isSuccess)		
			return 1;
		else
			return 0;
	}
	if(found_join != string::npos)
	{
		bool isSuccess = select_join_table ( command , catalog_object);
		if(isSuccess)		
			return 1;
		else
			return 0;
	}
	if(found_where != string::npos)
	{
		bool isSuccess = select_where_table( command , catalog_object);
		if(isSuccess)		
			return 1;		
		else
			return 0;
	}
	bool isSuccess = select_table(command , catalog_object);
		if(isSuccess)		
			return 1;		
		else
			return 0;
	

}

//Show table function with table name
bool show_table (string command , catalog *catalog_object)
{
	string table_name;
	bool isTableFound =false;
	int table_position;
	string temp_cmd = command;
	temp_cmd.erase(0, temp_cmd.find(' ')+1);	
	if(temp_cmd.substr(0,5) != "table")
	{
		cout<<"Syntax error:Table keyword is missing in Show Statement\n";
		output.push_back("Syntax error:Table keyword is missing in Show Statement\n");
		return 0;
	}
	temp_cmd.erase(0, temp_cmd.find(' ')+1);
	table_name = temp_cmd.substr(0,temp_cmd.size()-1);	
	for (unsigned i = 0;i<catalog_tbl.size();i++)
	{
		if(table_name == catalog_tbl[i].table_name)
		{
			isTableFound = true;
			table_position = i;
		}

	}
	if (!isTableFound)
	{
		cout<<"Table not found"<<endl;		
		output.push_back("Table not found\n");
		return 0;
	}
	
		cout<<"Table Name:"<<catalog_tbl[table_position].table_name<<endl;
		cout<<"Columns:"<<catalog_tbl[table_position].table_columns<<endl;
		cout<<"Primary Key:"<<catalog_tbl[table_position].primary_key<<endl;
		cout<<"Record Size:"<<catalog_tbl[table_position].rec_size<<endl;
		cout<<"Total Size:"<<catalog_tbl[table_position].total_size<<endl;
		cout<<"Records:"<<catalog_tbl[table_position].records<<endl;
		output.push_back("\nTable Name:" + catalog_tbl[table_position].table_name);
		output.push_back("\nColumns:" + catalog_tbl[table_position].table_columns);	
        output.push_back("\nPrimary Key:" + catalog_tbl[table_position].primary_key);
		output.push_back("\nRecord Size:" + catalog_tbl[table_position].rec_size);
		output.push_back("\nTotal Size:" + catalog_tbl[table_position].total_size);
		output.push_back("\nRecords:" +catalog_tbl[table_position].records);
	return 1;

}

//Show table -- General function
bool show_table_general (string command , catalog *catalog_object)
{	
	
	for(unsigned i = 0 ; i < catalog_tbl.size() ; i++)
	{
		cout<<"Table Name:"<<catalog_tbl[i].table_name<<endl;
		cout<<"Columns:"<<catalog_tbl[i].table_columns<<endl;
		cout<<"Primary Key:"<<catalog_tbl[i].primary_key<<endl;
		cout<<"Record Size:"<<catalog_tbl[i].rec_size<<endl;
		cout<<"Total Size:"<<catalog_tbl[i].total_size<<endl;
		cout<<"Records:"<<catalog_tbl[i].records<<endl;
		output.push_back("\nTable Name:" + catalog_tbl[i].table_name);
		output.push_back("\nColumns:" + catalog_tbl[i].table_columns);	
        output.push_back("\nPrimary Key:" + catalog_tbl[i].primary_key);
		output.push_back("\nRecord Size:" + catalog_tbl[i].rec_size);
		output.push_back("\nTotal Size:" + catalog_tbl[i].total_size);
		output.push_back("\nRecords:" +catalog_tbl[i].records);
	}

	return 1;

}

//If Sql statements given as inputs
void SQL_Query_Block (string command_line_argument,catalog * catalog_object)
{
	if (command_line_argument[command_line_argument.size()-1] != ';')
	{
		command_line_argument = command_line_argument + ";";
	}
	command_line_argument = remove_spaces(command_line_argument);
		//CREATE
		if (command_line_argument.substr(0, command_line_argument.find(' ')) == "create")
		{
			bool isSuccess = create_table(command_line_argument,catalog_object);
			if(isSuccess)
			{
				cout<<"Table created Successfully \n" ;
				output.push_back("Table created Successfully \n" );
			}
		}
		//INSERT
		else if (command_line_argument.substr(0, command_line_argument.find(' '))== "insert")
		{
		 bool isSuccess =insert_table(command_line_argument,catalog_object);
		 if(isSuccess)
			{
				cout<<"Insert Statement executed Successfully \n" ;
				output.push_back("Insert Statement executed Successfully \n");
			}
		}
		//SELECT
		else if (command_line_argument.substr(0, command_line_argument.find(' ')) == "select")
		{
		bool isSuccess =select_function(command_line_argument,catalog_object);
		 if(isSuccess)
			{
				cout<<"select Statement executed Successfully \n" ;
				output.push_back("select Statement executed Successfully \n");
			}
		}
		//SHOW TABLE
		else if ( command_line_argument == "show table;" )
		{
			bool isSuccess =show_table_general(command_line_argument,catalog_object);
			 if(isSuccess)
			{
				cout<<"Show Statement executed Successfully \n" ;
				output.push_back("Show Statement executed Successfully \n");
			}
		}
		//SHOW TABLE TABLE NAME
		else if (command_line_argument.substr(0, command_line_argument.find(' ')) == "show")
		{
		 bool isSuccess =show_table(command_line_argument,catalog_object);
		 if(isSuccess)
			{
				cout<<"Show Statement executed Successfully \n" ;
				output.push_back("Show Statement executed Successfully \n");
			}
		}
		//QUIT
		else if (command_line_argument == "quit;")
		{
			write_into_catalog();
			cout<<"Exit\n";			
			output.push_back("Exit\n");
			ofstream out1;
			const char * c = output_file_name.c_str();
			out1.open(c);
			if (out1.is_open())
			{
			for(unsigned i =0;i<output.size();i++)
			{				
					out1<<output[i]<<endl;
			}
			for(unsigned j=0;j<output_table.size();j++)
			{
				out1<<output_table[j];
			}
			}
			out1.close();
			exit(0);
		}
		else
		{
		cout<<"Enter a valid SQL query"<<endl;		
		output.push_back("Enter a valid SQL query\n");
		}
}

/* HELPER METHODS */

void main(int argc, char* argv[])
{
	string command_line_argument,input;
	string Catalog_file_contents[10000],line;
	int line_counter = 0;
	ifstream file_Catalog;
	file_Catalog.open("catalog.txt");
	if (file_Catalog.is_open())	
	{		
		while(!file_Catalog.eof( ))
		{
			getline (file_Catalog,line);
			Catalog_file_contents[line_counter] = line;			
			line_counter++;
		}	
	}
	file_Catalog.close();
	num_of_table_in_catalog = line_counter/6;
	catalog *catalog_object = new catalog[1000];
	catalog_format(Catalog_file_contents,catalog_object);
	for (int i=0;i<num_of_table_in_catalog;i++)
	{
		catalog_tbl.push_back(catalog_object[i]);
	}
	
	if (argc == 1)
	{
		argv[1] = "select";
	}

	
    string command_line_argument_1 = ToLowerCase(argv[1]);
	//string command_line_argument_1 = "select";
	//command_line_argument ="Select * from Class where st_status >0 and st_status < 0";
	//command_line_argument = ToLowerCase ("script=xyz.txt > output.txt"); 
	if (command_line_argument_1.substr(0,6) == "script")
	{
		// SCRIPT BLOCK
		
		for(int i = 1 ;i < argc; i++)
		{
			command_line_argument += " ";
			command_line_argument += argv[i] ;
		}
		
		string temp_cmd_line_arg = ToLowerCase(remove_spaces(command_line_argument));
		
		int pos1=temp_cmd_line_arg.find('=');
		int pos2 = temp_cmd_line_arg.find('>');
		int pos3 = pos2-pos1;
		
		string filename = temp_cmd_line_arg.substr(pos1+1,pos3-1);
		filename=remove_spaces(filename);
		
		temp_cmd_line_arg.erase(0,temp_cmd_line_arg.find('>')+1);
		vector<string> query;
		ifstream in;
		const char * c = filename.c_str();
		in.open(c);
		if(!in.is_open())
		{
			cout<< "Error: Script file doesnt exist"<<endl;			
		}
		else
		{
		string line,line1;
		while(in.good())
		{
		getline(in, line1);
		if(!line1.empty())
		{
		line +=ToLowerCase(remove_spaces(line1));
		line +=" ";// Add the line to the end
		}
		}
		split(line,';',query);
		if(query[query.size()-1] == " " )
		query.pop_back();
		query.push_back("quit");

		// Add line numbers:
		temp_cmd_line_arg=remove_spaces(temp_cmd_line_arg);
		 output_file_name = temp_cmd_line_arg.substr(0,temp_cmd_line_arg.size());
		 output_file_name = remove_spaces(output_file_name);
		 
		 for(int i = 0; i < query.size(); i++)
			{	
			 SQL_Query_Block(query[i],catalog_object);
			}
		 

		}
		
	}
	else
	
	{

		for(int i = 1 ;i < argc; i++)
		{
			command_line_argument += " ";
			command_line_argument += argv[i] ;
		}
		command_line_argument =ToLowerCase(remove_spaces(command_line_argument));
		cout<<command_line_argument<<endl;
		// SQL QUERY BLOCK
		SQL_Query_Block(command_line_argument,catalog_object);
		while (true)
		{
			cout<<"SQL > ";
			getline (cin, input);
			input = ToLowerCase(remove_spaces(input));
			SQL_Query_Block(input,catalog_object);
		}
	}	
}