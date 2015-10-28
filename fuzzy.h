
#define SUBSETS_LIMIT 5
#define INPUTS_LIMIT 10
#define OUTPUTS_LIMIT 5
#define RULES_LIMIT 20

#define RULE_LENGTH 150
#define NAME_LENGTH 30

#define DEBUGGING 0

#define TRAPEZOID 0
#define TRAPEZOID_LEFT_UNBOUNDED 1
#define TRAPEZOID_RIGHT_UNBOUNDED 2
/*
				fuzzy
	 |					|			|
	input[]			  output[]	  rule[]
	|					|
	subset[]		 subset[]

*/
class subset
{
public:
	int center,upper_width,lower_width,shape;
	char name[NAME_LENGTH];
	int get_mean()
	{
		return center;
	}
	float DOM(int input_value)
	{
		int upper_left = center - upper_width/2;
		int upper_right = center + upper_width/2;
		int lower_left = center - lower_width/2;
		int lower_right = center + lower_width/2;

		
		if(input_value >= upper_left && input_value <= upper_right)
			return 1;
		else if(input_value <= lower_left || input_value >= lower_right)
			return 0;
		else if(shape == TRAPEZOID_LEFT_UNBOUNDED && input_value < upper_left)
			return 1;
		else if(shape == TRAPEZOID_RIGHT_UNBOUNDED && input_value > upper_right)
			return 1;
		else if(input_value > lower_left && input_value < upper_right)
			return ((float)(input_value - lower_left)/(float)(upper_left - lower_left));
		else if(input_value > upper_right && input_value < lower_right)
			return ((float)(input_value - lower_right)/(float)(upper_right - lower_right));
		else
		{
			if(DEBUGGING)cout<<"\n\nERROR CLASSIFYING INPUT VALUE OF : "<<name;
			return -1;
		}
	}
};
class input
{
public:
	int sub_cnt,value;
	subset sub[SUBSETS_LIMIT];
	char name[NAME_LENGTH];
	
	input()
	{
		sub_cnt = 0;value = 0;
	}
	int get_subset_index(char *sub_name)
	{
		int i;
		for(i=0;i<sub_cnt;i++)
			if(strcmp(sub_name,sub[i].name)==0)
				break;
		return i;
	}
};
class output
{
public:
	int sub_cnt;
	subset sub[SUBSETS_LIMIT];
	char name[NAME_LENGTH];
	
	output()
	{
		sub_cnt = 0;
	}
	int get_subset_index(char *sub_name)
	{
		int i;
		for(i=0;i<sub_cnt;i++)
			if(strcmp(sub_name,sub[i].name)==0)
				break;
		return i;
	}
};
class rules
{
public:
	char str[RULE_LENGTH],token[NAME_LENGTH];
	
	int str_ptr;
	bool end_of_rule;
	rules()
	{
		str_ptr = 0;
		end_of_rule = false;
		strcpy(str,"\0");
	}
	char* get_next_token()
	{
		int i;
		for(i=0;str[str_ptr] != ' ' && str[str_ptr] != '\0';i++,str_ptr++)
			token[i] = str[str_ptr];
		token[i] = '\0';
		
		if(str[str_ptr] == '\0')
			end_of_rule = true;
		else
			str_ptr++;

		return token;
	}
};
class fuzzy
{
	int in_cnt,out_cnt,rule_cnt;
	input in[INPUTS_LIMIT];
	output out[OUTPUTS_LIMIT];
	rules rule[RULES_LIMIT];

	
public:
	fuzzy()
	{
		in_cnt = out_cnt = rule_cnt = 0;
	}
	int get_input_index(char *in_name)
	{
		int i;
		for(i=0;i<in_cnt;i++){
			if(strcmp(in_name,in[i].name)==0)
				break;
		}
		if(i == in_cnt)
			if(DEBUGGING)cout<<"\nERROR : INPUT STRING '"<<in_name<<"' NOT FOUND";

		return i;
	}
	int get_output_index(char *out_name)
	{
		int i;
		for(i=0;i<out_cnt;i++){
			if(strcmp(out_name,out[i].name)==0)
				break;
		}
		if(i == out_cnt)
			if(DEBUGGING)cout<<"\nERROR : OUTPUT STRING '"<<out_name<<"' NOT FOUND";
		return i;
	}

	void init_rule_token_ptr()
	{
		for(int i=0;i<rule_cnt;i++)
			rule[i].str_ptr = 0;
	}
	
	void add_input(char *name_in)
	{
		strcpy(in[in_cnt++].name,name_in);
	}
	void add_input_subset(char *name_in,char *name_sub,int c,int uw,int lw,int shape)
	{
		int in_ptr;
		for(in_ptr = 0 ; in_ptr < in_cnt ; in_ptr++)
			if(strcmp(in[in_ptr].name,name_in)==0)
				break;
		
		if(in_ptr == in_cnt && DEBUGGING)
			cout<<"\nERROR : INPUT '"<<name_in<<"' NOT FOUND";

		int sub_ptr = in[in_ptr].sub_cnt;
		strcpy(in[in_ptr].sub[sub_ptr].name,name_sub);
		in[in_ptr].sub[sub_ptr].center = c;
		in[in_ptr].sub[sub_ptr].upper_width = uw;
		in[in_ptr].sub[sub_ptr].lower_width = lw;
		in[in_ptr].sub[sub_ptr].shape = shape;
		in[in_ptr].sub_cnt++;
	}
	
	void add_output(char *name_out)
	{
		strcpy(out[out_cnt++].name,name_out);
	}
	void add_output_subset(char *name_out,char *name_sub,int c,int uw,int lw,int shape)
	{
		int out_ptr;
		for(out_ptr=0;out_ptr<out_cnt;out_ptr++)
			if(strcmp(out[out_ptr].name,name_out)==0)
				break;
		
		if(out_ptr == in_cnt && DEBUGGING)
			cout<<"\nERROR : OUTPUT '"<<name_out<<"' NOT FOUND";

		int sub_ptr = out[out_ptr].sub_cnt;
		
		strcpy(out[out_ptr].sub[sub_ptr].name,name_sub);
		out[out_ptr].sub[sub_ptr].center = c;
		out[out_ptr].sub[sub_ptr].upper_width = uw;
		out[out_ptr].sub[sub_ptr].lower_width = lw;
		out[out_ptr].sub[sub_ptr].shape = shape;
		out[out_ptr].sub_cnt++;
	}
	
	void define_rule(char *rule_str)
	{
		strcpy(rule[rule_cnt++].str,rule_str);
	}
	void assign_input(char* in_name,int num)
	{
		int in_ptr = get_input_index(in_name);
		in[in_ptr].value = num;
	}

	
	int defuzzify(char *out_name)
	{
		float DOF,sum_DOF=0,sum_DOM=0,min_DOM = 65534;
		char token[NAME_LENGTH];

		//INITIALIZE TOKEN POINTER OF RULES
		init_rule_token_ptr();

		// TRAVERSE ALL RULES
		for(int rule_ptr=0;rule_ptr < rule_cnt;rule_ptr++)
		{
			//GET FIRST TOKEN
			strcpy(token,rule[rule_ptr].get_next_token());
			if(strcmp(token,"IF")!=0 && DEBUGGING)
			{cout<<"IF TOKEN MISSING";while(1);}


			//GET NEXT TOKEN
			strcpy(token,rule[rule_ptr].get_next_token());
			
			min_DOM = 65535;

			//SEARCH WHILE INPUT RULES DEFINED
			while(strcmp(token,"THEN")!=0)
			{
				// SEARCH INPUTS
				int in_ptr = get_input_index(token);
				
				//GET NEXT TOKEN
				strcpy(token,rule[rule_ptr].get_next_token());
				
				if(strcmp(token,"IS")!=0)
				{cout<<"IS TOKEN MISSING";while(1);}

				//GET INPUT SUBSET VALUE TOKEN
				strcpy(token,rule[rule_ptr].get_next_token());
				
				//SEARCH INPUT SUBSET
				int sub_ptr = in[in_ptr].get_subset_index(token);
				
				//GET DEGREE OF MEMBERSHIP FOR SUBSET
				float DOM = in[in_ptr].sub[sub_ptr].DOM(in[in_ptr].value);

				if(DOM < min_DOM)
					min_DOM = DOM;
				
				

				//GET NEXT TOKEN
				strcpy(token,rule[rule_ptr].get_next_token());

				//IF TOKEN IS AND => SKIP
				if(strcmp(token,"AND")==0)
					strcpy(token,rule[rule_ptr].get_next_token());
			}
			if(DEBUGGING)cout<<"\nRULE "<<rule_ptr+1<<" : "<<min_DOM;

			
			//_____________  SEARCH FOR OUTPUT AND ITS SUBSET   ________//
			
			do{
				strcpy(token,rule[rule_ptr].get_next_token());
			}while(strcmp(token,out_name) != 0 && strcmp(token,"\0") != 0);
			
			// IF OUTPUT FOUND IN RULE GET SUBSET
			if(strcmp(token,out_name) != 0)
				DOF = 0;
			else
			{
				int out_ptr = get_output_index(out_name);
				
				//GET NEXT TOKEN => IS
				strcpy(token,rule[rule_ptr].get_next_token());
				if(strcmp(token,"IS") != 0)
				{cout<<"IS TOKEN MISSING";while(1);}

				//GET NEXT TOKEN => OUTPUT SUBSET
				strcpy(token,rule[rule_ptr].get_next_token());
				int sub_ptr = out[out_ptr].get_subset_index(token);
				int avg_sub_value  = out[out_ptr].sub[sub_ptr].center;

				DOF = min_DOM * avg_sub_value;
			}
			
			sum_DOF += DOF;
			sum_DOM += min_DOM;

			
		}
		//IF NO RULE SATISFIED RETURN NON WEIGHTED AVERAGE
		if(sum_DOM == 0)
		{
			int out_ptr = get_output_index(out_name);
			int i,avg = 0;
			for(i=0;i<out[out_ptr].sub_cnt;i++)
				avg += out[out_ptr].sub[i].center;
			
			return (avg/i);
		}
		//ELSE RETURN WEIGHTED AVERAGE

		return (int)(sum_DOF/sum_DOM);
	}
};