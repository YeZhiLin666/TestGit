 #include "includes.h"
//  #include "fuzzyPID.h"

//Table define: row is L M S Z of Error
//              column is L M S Z of Error Rate
//------------------------------------------------------------
const unsigned char kp_table[4][4]={{KP_M_CENT,KP_S_CENT,KP_M_CENT,KP_M_CENT},\
		                          {KP_L_CENT,KP_M_CENT,KP_L_CENT,KP_L_CENT},\
															{KP_L_CENT,KP_M_CENT,KP_L_CENT,KP_L_CENT},\
															{KP_L_CENT,KP_M_CENT,KP_L_CENT,KP_Z_CENT}};

const unsigned char ki_table[4][4]={{KI_Z_CENT,KI_S_CENT,KI_M_CENT,KI_L_CENT},\
															{KI_Z_CENT,KI_S_CENT,KI_L_CENT,KI_L_CENT},\
															{KI_Z_CENT,KI_Z_CENT,KI_L_CENT,KI_L_CENT},\
															{KI_Z_CENT,KI_Z_CENT,KI_L_CENT,KI_Z_CENT}};

const unsigned char kd_table[4][4]={{KD_S_CENT,KD_M_CENT,KD_Z_CENT,KD_Z_CENT},\
															{KD_M_CENT,KD_M_CENT,KD_S_CENT,KD_Z_CENT},\
															{KD_L_CENT,KD_L_CENT,KD_S_CENT,KD_S_CENT},\
															{KD_L_CENT,KD_L_CENT,KD_S_CENT,KD_Z_CENT}};
// float Fuzzy_PID::KP_H;
// float Fuzzy_PID::KI_H;
// float Fuzzy_PID::KD_H;
// float Fuzzy_PID::KP_L;
// float Fuzzy_PID::KI_L;
// float Fuzzy_PID::KD_L;
// float Fuzzy_PID::E_L;
// float Fuzzy_PID::E_H;
// float Fuzzy_PID::EC_L;
// float Fuzzy_PID::EC_H;
BASE_FUZZY_PARA Fuzzy_PID::Set_FuzzyPara;
float Fuzzy_PID::E_L_L;
float Fuzzy_PID::E_L_H;
float Fuzzy_PID::E_M_L;
float Fuzzy_PID::E_M_H;
float Fuzzy_PID::E_S_L;
float Fuzzy_PID::E_S_H;
float Fuzzy_PID::E_Z_L;
float Fuzzy_PID::E_Z_H;
float Fuzzy_PID::EC_L_L;
float Fuzzy_PID::EC_L_H;
float Fuzzy_PID::EC_M_L;
float Fuzzy_PID::EC_M_H;
float Fuzzy_PID::EC_S_L;
float Fuzzy_PID::EC_S_H;
float Fuzzy_PID::EC_Z_L;
float Fuzzy_PID::EC_Z_H;
float Fuzzy_PID::KP_F_WIDE;
float Fuzzy_PID::KI_F_WIDE;
float Fuzzy_PID::KD_F_WIDE;
float Fuzzy_PID::KP_L_L;
float Fuzzy_PID::KP_L_H;
float Fuzzy_PID::KP_M_L;
float Fuzzy_PID::KP_M_H;
float Fuzzy_PID::KP_S_L;
float Fuzzy_PID::KP_S_H;
float Fuzzy_PID::KP_Z_L;
float Fuzzy_PID::KP_Z_H;
		
float Fuzzy_PID::KI_L_L;
float Fuzzy_PID::KI_L_H;
float Fuzzy_PID::KI_M_L;
float Fuzzy_PID::KI_M_H;
float Fuzzy_PID::KI_S_L;
float Fuzzy_PID::KI_S_H;
float Fuzzy_PID::KI_Z_L;
float Fuzzy_PID::KI_Z_H;
		
float Fuzzy_PID::KD_L_L;
float Fuzzy_PID::KD_L_H;
float Fuzzy_PID::KD_M_L;
float Fuzzy_PID::KD_M_H;
float Fuzzy_PID::KD_S_L;
float Fuzzy_PID::KD_S_H;
float Fuzzy_PID::KD_Z_L;
float Fuzzy_PID::KD_Z_H;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//--------------------------------------------------------------------------------------//
void Fuzzy_PID::InitFuzzy(BASE_FUZZY_PARA m_fuzzy)
{
	
	
	//////////////////////////////
//  E_L= 0;
//  E_H= 1;

//  EC_L= 0;
//  EC_H= 1; //EC
/////////////////////////////////////////////
 E_L_L= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*2);
 E_L_H=(m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*3);

 E_M_L= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*1);
 E_M_H= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*3);

 E_S_L= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*0);
 E_S_H= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*2);

 E_Z_L= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*0);
 E_Z_H= (m_fuzzy.E_L + ((m_fuzzy.E_H - m_fuzzy.E_L)/3)*1);
/////////////////////////////////////////////
 EC_L_L= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*2);
 EC_L_H= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*3);

 EC_M_L= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*1);
 EC_M_H= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*3);

 EC_S_L= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*0);
 EC_S_H= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*2);

 EC_Z_L= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*0);
 EC_Z_H= (m_fuzzy.EC_L + ((m_fuzzy.EC_H - m_fuzzy.EC_L)/3)*1);
/////////////////////////////////////////////

 KP_F_WIDE= (m_fuzzy.KP_H - m_fuzzy.KP_L)/3;


 KI_F_WIDE= (m_fuzzy.KI_H - m_fuzzy.KI_L)/3;


 KD_F_WIDE= (m_fuzzy.KD_H - m_fuzzy.KD_L)/3;
/////////////////////////////////////
 KP_L_L= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*2);
 KP_L_H= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*3);
 KP_M_L= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*1);
 KP_M_H= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*3);
 KP_S_L= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*0);
 KP_S_H= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*2);
 KP_Z_L= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*0);
 KP_Z_H= (m_fuzzy.KP_L + ((m_fuzzy.KP_H - m_fuzzy.KP_L)/3)*1);


 KI_L_L= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*2);
 KI_L_H= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*3);
 KI_M_L= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*1);
 KI_M_H= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*3);
 KI_S_L= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*0);
 KI_S_H= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*2);
 KI_Z_L= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*0);
 KI_Z_H= (m_fuzzy.KI_L + ((m_fuzzy.KI_H - m_fuzzy.KI_L)/3)*1);


 KD_L_L= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*2);
 KD_L_H= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*3);
 KD_M_L= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*1);
 KD_M_H= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*3);
 KD_S_L= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*0);
 KD_S_H= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*2);
 KD_Z_L= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*0);
 KD_Z_H= (m_fuzzy.KD_L + ((m_fuzzy.KD_H - m_fuzzy.KD_L)/3)*1);

}
// float Fuzzy_PID::function_out_kp(float kph, float kpl, unsigned char func_name, float degree_of_membership, char defuzzy_type)
// {
// 	float result;
// 	float result1;
// 	float result2;

// 	switch (func_name)
// 		{
// 			case KP_L_CENT: result1 = (degree_of_membership+2)*(kph-kpl)/3+kpl;
// 											result2 = result1;
// 				              break;
// 			case KP_M_CENT: result1 = (degree_of_membership+1)*(kph-kpl)/3+kpl;
// 		                  result2 = (3-degree_of_membership)*(kph-kpl)/3+kpl;
// 				              break;
// 		  case KP_S_CENT: result1 = degree_of_membership*(kph-kpl)/3+kpl;
// 		                  result2 = (2-degree_of_membership)*(kph-kpl)/3+kpl;
// 				              break;
// 			case KP_Z_CENT: result1 = (1-degree_of_membership)*(kph-kpl)/3+kpl;
// 											result2 = result1;
// 				              break;
// 		  default:
// 				              break;	
// 		}
// 	
// 	if(result1 > kph)
// 		{
// 			result1 = kph;
// 		}
// 	if(result1 < kpl)
// 	  {
// 			result1 = kpl;
// 		}
// 	if(result2 > kph)
// 		{
// 			result2 = kph;
// 		}
// 	if(result2 < kpl)
// 	  {
// 			result2 = kpl;
// 		}
// 	if(defuzzy_type == SOM)   //using the smaller one
// 		{
// 			if(result1 <= result2)
// 				{
// 					result = result1;
// 				}
// 			else
// 				{
// 					result = result2;
// 				}
// 		}
// 	if(defuzzy_type == LOM)   //using the bigger one
// 		{
// 			if(result1 >= result2)
// 				{
// 					result = result1;
// 				}
// 			else
// 				{
// 					result = result2;
// 				}
// 		}
// 	if(defuzzy_type == MOM)   //using the middle value
// 		{
// 				result = (result1 + result2)/2;
// 		}
// 	
// 	return result;
// }
//--------------------------------------------------------------------------------------//
float Fuzzy_PID::function_out_k(float kh, float kl, unsigned char func_name, float degree_of_membership, char defuzzy_type)
{
	float result;
	float result1;
	float result2;
	
	switch (func_name)
		{
			case KI_L_CENT: result1 = (degree_of_membership+2)*(kh-kl)/3+kl;
				              result2 = result1;
											break;
			case KI_M_CENT: result1 = (degree_of_membership+1)*(kh-kl)/3+kl;
		                  result2 = (3-degree_of_membership)*(kh-kl)/3+kl;
				              break;
		  case KI_S_CENT: result1 = degree_of_membership*(kh-kl)/3+kl;
		                  result2 = (2-degree_of_membership)*(kh-kl)/3+kl;
				              break;
			case KI_Z_CENT: result1 = (1-degree_of_membership)*(kh-kl)/3+kl;
											result2 = result1;
				              break;
		  default:
				              break;	
		}
	if(result1 > kh)
		{
			result1 = kh;
		}
	if(result1 < kl)
	  {
			result1 = kl;
		}
	if(result2 > kh)
		{
			result2 = kh;
		}
	if(result2 < kl)
	  {
			result2 = kl;
		}
	if(defuzzy_type == SOM)   //using the smaller one
		{
			if(result1 <= result2)
				{
					result = result1;
				}
			else
				{
					result = result2;
				}
		}
	if(defuzzy_type == LOM)   //using the bigger one
		{
			if(result1 >= result2)
				{
					result = result1;
				}
			else
				{
					result = result2;
				}
		}
	if(defuzzy_type == MOM)   //using the middle value
		{
				result = (result1 + result2)/2;
		}
	
	return result;
}
//--------------------------------------------------------------------------------------//
// float Fuzzy_PID::function_out_kd(float kdh, float kdl, unsigned char func_name, float degree_of_membership, char defuzzy_type)
// {
// 	float result;
// 	float result1;
// 	float result2;
// 	
// 	switch (func_name)
// 		{
// 			case KD_L_CENT: result1 = (degree_of_membership+2)*(kdh-kdl)/3+kdl;
// 											result2 = result1;
// 				              break;
// 			case KD_M_CENT: result1 = (degree_of_membership+1)*(kdh-kdl)/3+kdl;
// 		                  result2 = (3-degree_of_membership)*(kdh-kdl)/3+kdl;
// 				              break;
// 		  case KD_S_CENT: result1 = degree_of_membership*(kdh-kdl)/3+kdl;
// 		                  result2 = (2-degree_of_membership)*(kdh-kdl)/3+kdl;
// 				              break;
// 			case KD_Z_CENT: result1 = (1-degree_of_membership)*(kdh-kdl)/3+kdl;
// 											result2 = result1;
// 				              break;
// 		  default:
// 				              break;	
// 		}
// 	if(result1 > kdh)
// 		{
// 			result1 = kdh;
// 		}
// 	if(result1 < kdl)
// 	  {
// 			result1 = kdl;
// 		}
// 	if(result2 > kdh)
// 		{
// 			result2 = kdh;
// 		}
// 	if(result2 < kdl)
// 	  {
// 			result2 = kdl;
// 		}
// 	if(defuzzy_type == SOM)   //using the smaller one
// 		{
// 			if(result1 <= result2)
// 				{
// 					result = result1;
// 				}
// 			else
// 				{
// 					result = result2;
// 				}
// 		}
// 	if(defuzzy_type == LOM)   //using the bigger one
// 		{
// 			if(result1 >= result2)
// 				{
// 					result = result1;
// 				}
// 			else
// 				{
// 					result = result2;
// 				}
// 		}
// 	if(defuzzy_type == MOM)   //using the middle value
// 		{
// 				result = (result1 + result2)/2;
// 		}
// 	
// 	return result;
// }
//--------------------------------------------------------------------------------------//
GET_FUZZY_PID Fuzzy_PID::defuzzy(char e_belong1, char e_belong2, char ec_belong1, char ec_belong2, \
                          float degree_of_membership_e1, float degree_of_membership_e2, \
                          float degree_of_membership_ec1, float degree_of_membership_ec2 ,BASE_FUZZY_PARA m_fuzzy)
{
	GET_FUZZY_PID m_FuzzyPID;
	unsigned char m_func_Kp=1,m_func_Ki=1,m_func_Kd=1;
	char table_A_ec = 0x25;
	char table_B_e = 0x25;
	char table_C_membership_out = 0x25;
	float degree_of_membership_out[4];
	degree_of_membership_out[0] = degree_of_membership_e1 >= degree_of_membership_ec1 ? degree_of_membership_ec1 : degree_of_membership_e1;  //ȡС
	degree_of_membership_out[1] = degree_of_membership_e1 >= degree_of_membership_ec2 ? degree_of_membership_ec2 : degree_of_membership_e1;  //ȡС
	degree_of_membership_out[2] = degree_of_membership_e2 >= degree_of_membership_ec1 ? degree_of_membership_ec1 : degree_of_membership_e2;  //ȡС
	degree_of_membership_out[3] = degree_of_membership_e2 >= degree_of_membership_ec2 ? degree_of_membership_ec2 : degree_of_membership_e2;  //ȡС
	
	
	if(degree_of_membership_out[0] >= degree_of_membership_out[1] && \
		 degree_of_membership_out[0] >= degree_of_membership_out[2] && \
	   degree_of_membership_out[0] >= degree_of_membership_out[3])
	  {
		
		    table_A_ec = ec_belong1;
		    table_B_e = e_belong1;
		    table_C_membership_out = 0;
// 			m_func_Kp=kp_table[ec_belong1][e_belong1];
// 			m_func_Ki=ki_table[ec_belong1][e_belong1];
// 			m_func_Kd=kd_table[ec_belong1][e_belong1];
// 			m_FuzzyPID.Delta_Kp = function_out_k(m_fuzzy.KP_H, m_fuzzy.KP_L, m_func_Kp, degree_of_membership_out[0], DEFUZZY);
// 			m_FuzzyPID.Delta_Ki = function_out_k(m_fuzzy.KI_H, m_fuzzy.KI_L, m_func_Ki, degree_of_membership_out[0], DEFUZZY);
// 			m_FuzzyPID.Delta_Kd = function_out_k(m_fuzzy.KD_H, m_fuzzy.KD_L, m_func_Kd, degree_of_membership_out[0], DEFUZZY);
		}
	if(degree_of_membership_out[1] >= degree_of_membership_out[0] && \
		 degree_of_membership_out[1] >= degree_of_membership_out[2] && \
	   degree_of_membership_out[1] >= degree_of_membership_out[3])
	  {
		  	table_A_ec = ec_belong2;
		    table_B_e = e_belong1;
		    table_C_membership_out = 1;
// 			m_func_Kp=kp_table[ec_belong2][e_belong1];
// 			m_func_Ki=ki_table[ec_belong2][e_belong1];
// 			m_func_Kd=kd_table[ec_belong2][e_belong1];
// 			m_FuzzyPID.Delta_Kp = function_out_k(m_fuzzy.KP_H, m_fuzzy.KP_L, m_func_Kp, degree_of_membership_out[1], DEFUZZY);
// 			m_FuzzyPID.Delta_Ki = function_out_k(m_fuzzy.KI_H, m_fuzzy.KI_L, m_func_Ki, degree_of_membership_out[1], DEFUZZY);
// 			m_FuzzyPID.Delta_Kd = function_out_k(m_fuzzy.KD_H, m_fuzzy.KD_L, m_func_Kd, degree_of_membership_out[1], DEFUZZY);
		}
	if(degree_of_membership_out[2] >= degree_of_membership_out[0] && \
		 degree_of_membership_out[2] >= degree_of_membership_out[1] && \
	   degree_of_membership_out[2] >= degree_of_membership_out[3])
	  {   
		  	table_A_ec = ec_belong1;
		    table_B_e = e_belong2;
		    table_C_membership_out = 2;
// 			m_func_Kp=kp_table[ec_belong1][e_belong2];
// 			m_func_Ki=ki_table[ec_belong1][e_belong2];
// 			m_func_Kd=kd_table[ec_belong1][e_belong2];
// 			m_FuzzyPID.Delta_Kp = function_out_k(m_fuzzy.KP_H, m_fuzzy.KP_L, m_func_Kp, degree_of_membership_out[2], DEFUZZY);
// 			m_FuzzyPID.Delta_Ki = function_out_k(m_fuzzy.KI_H, m_fuzzy.KI_L, m_func_Ki, degree_of_membership_out[2], DEFUZZY);
// 		    m_FuzzyPID.Delta_Kd = function_out_k(m_fuzzy.KD_H, m_fuzzy.KD_L, m_func_Kd, degree_of_membership_out[2], DEFUZZY);
		}
	if(degree_of_membership_out[3] >= degree_of_membership_out[0] && \
		 degree_of_membership_out[3] >= degree_of_membership_out[1] && \
	   degree_of_membership_out[3] >= degree_of_membership_out[2])
	  {   
		  	table_A_ec = ec_belong2;
		    table_B_e = e_belong2;
		    table_C_membership_out = 3;
// 			m_func_Kp=kp_table[ec_belong2][e_belong2];
// 			m_func_Ki=ki_table[ec_belong2][e_belong2];
// 			m_func_Kd=kd_table[ec_belong2][e_belong2];
// 			m_FuzzyPID.Delta_Kp = function_out_k(m_fuzzy.KP_H, m_fuzzy.KP_L, m_func_Kp, degree_of_membership_out[3], DEFUZZY);
// 			m_FuzzyPID.Delta_Ki = function_out_k(m_fuzzy.KI_H, m_fuzzy.KI_L, m_func_Ki, degree_of_membership_out[3], DEFUZZY);
// 			m_FuzzyPID.Delta_Kd = function_out_k(m_fuzzy.KD_H, m_fuzzy.KD_L, m_func_Kd, degree_of_membership_out[3], DEFUZZY);
	  }
	
	  if(table_C_membership_out != 0x25)
	  {
		    m_func_Kp=kp_table[table_A_ec][table_B_e];
			m_func_Ki=ki_table[table_A_ec][table_B_e];
			m_func_Kd=kd_table[table_A_ec][table_B_e];
			m_FuzzyPID.Delta_Kp = function_out_k(m_fuzzy.KP_H, m_fuzzy.KP_L, m_func_Kp, degree_of_membership_out[table_C_membership_out], DEFUZZY);
			m_FuzzyPID.Delta_Ki = function_out_k(m_fuzzy.KI_H, m_fuzzy.KI_L, m_func_Ki, degree_of_membership_out[table_C_membership_out], DEFUZZY);
			m_FuzzyPID.Delta_Kd = function_out_k(m_fuzzy.KD_H, m_fuzzy.KD_L, m_func_Kd, degree_of_membership_out[table_C_membership_out], DEFUZZY);
	  }
	return m_FuzzyPID;
}
//--------------------------------------------------------------------------------------//
float Fuzzy_PID::function_of_er(float eh, float el, float x, unsigned char type)
{
	float result;
	
	if( el >= eh)   //parameter error, return 0
	{
		return 0;
	}	
// 	if((x < el) || (x > eh))
// 	{
// 		return 0;
// 	}
	if(x<el)
		x=el;
	if(x>eh)
		x=eh;
	
	switch(type)
		{
			case 1: result = ((eh - el)/3 - (x - el)) / ((eh - el)/3);  //function of ez
							break;
			case 2: result = (x - el) / ((eh - el)/3);  //function of es1
							break;
			case 3: result = ((eh - el)*2/3 - (x - el)) / ((eh - el)/3);  //function of es2
							break;
			case 4: result = (x - el) / ((eh - el)/3) - 1;  //function of em1
							break;
			case 5: result = ((eh - el) - (x - el)) / ((eh - el)/3);  //function of em2
							break;
			case 6: result = (x - el) / ((eh - el)/3) - 2;   //fnction of el
							break;
			default:
							break;
		}
	if(result > 1)
		{
			result = 1;
		}
	if(result < 0)
		{
			result = 0;
		}
	return result;
}
//--------------------------------------------------------------------------------------//
// float Fuzzy_PID::function_of_ec(float ech, float ecl, float x, unsigned char type)
// {
// 	float result;
// 	
// 	if( ecl >= ech)   //parameter error, return 0
// 	{
// 		return 0;
// 	}	
// // 	if((x < ecl) || (x > ech))
// // 	{
// // 		return 0;
// // 	}
// 	if(x<ecl)
// 		x=ecl;
// 	if(x>ech)
// 		x=ech;
//  
// 	
// 	switch(type)
// 		{
// 			case 1: result = ((ech - ecl)/3 - (x - ecl)) / ((ech - ecl)/3);  //function of ez
// 							break;
// 			case 2: result = (x - ecl) / ((ech - ecl)/3);  //function of es1
// 							break;
// 			case 3: result = ((ech - ecl)*2/3 - (x - ecl)) / ((ech - ecl)/3);  //function of es2
// 							break;
// 			case 4: result = (x - ecl) / ((ech - ecl)/3) - 1;  //function of em1
// 							break;
// 			case 5: result = ((ech - ecl) - (x - ecl)) / ((ech - ecl)/3);  //function of em2
// 							break;
// 			case 6: result = (x - ecl) / ((ech - ecl)/3) - 2;   //fnction of el
// 							break;
// 			default:
// 							break;
// 		}
// 	if(result > 1)
// 		{
// 			result = 1;
// 		}
// 	if(result < 0)
// 		{
// 			result = 0;
// 		}
// 	return result;
// }
//--------------------------------------------------------------------------------------//
GET_FUZZY_PID Fuzzy_PID::fuzzy_pid(float error, float error_rate,BASE_FUZZY_PARA get_Fuzzy)
{
	GET_FUZZY_PID r_FuzzyPID;
	float degree_of_membership_e1=0.0f;
	float degree_of_membership_e2=0.0f;
	float degree_of_membership_ec1=0.0f;
	float degree_of_membership_ec2=0.0f;
	
	unsigned char e_belong1=0;
	unsigned char e_belong2=0;
	unsigned char ec_belong1=0;
	unsigned char ec_belong2=0;
	unsigned char table_A_er_E=0x25;
	unsigned char table_B_er_EC=0x25;
	//Given the range
	if(error < 0)    //error rate should be an absolute value
	{
		error *= -1; 
	}
	if(error > E_L_H)
	{
		error = E_L_H;
	}
	if(error < E_Z_L)
	{
		error = E_Z_L;
	}
	//////////////////////////////////////////////////
	if(error_rate < 0)    //error rate should be an absolute value
	{
		error_rate *= -1; 
	}
	if(error_rate > EC_L_H)
	{
		error_rate = EC_L_H;
	}
	if(error_rate < EC_Z_L)
	{
		error_rate = EC_Z_L;
	}
	/////////////////////////////////////////////
	if(error >= E_Z_L && error < E_Z_H)  //EC maybe belongs to Z or S
	{
		table_A_er_E=1;
// 		degree_of_membership_e1 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 1);
// 		degree_of_membership_e2 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 2);
		e_belong1 = FUZZY_Z;
		e_belong2 = FUZZY_S;
	}	
	/////////////////////////////////////////////
	if(error >= E_Z_H && error < E_S_H)  //EC maybe belongs to S or M
	{
		table_A_er_E=3;
// 		degree_of_membership_e1 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 3);
// 		degree_of_membership_e2 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 4);
		e_belong1 = FUZZY_S;
		e_belong2 = FUZZY_M;
	}		
	/////////////////////////////////////////////
	if(error >= E_S_H && error <=E_M_H)  //EC maybe belongs to M or L
	{
		table_A_er_E=5;
// 		degree_of_membership_e1 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 5);
// 		degree_of_membership_e2 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, 6);
		e_belong1 = FUZZY_M;
		e_belong2 = FUZZY_L;
	}		
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	/////////////////////////////////////////////
	if(error_rate >= EC_Z_L && error_rate < EC_Z_H)  //EC maybe belongs to Z or S
	{
		table_B_er_EC=1;
// 		degree_of_membership_ec1 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 1);
// 		degree_of_membership_ec2 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 2);
		ec_belong1 = FUZZY_Z;
		ec_belong2 = FUZZY_S;
	}	
	/////////////////////////////////////////////
	if(error_rate >= EC_Z_H && error_rate < EC_S_H)  //EC maybe belongs to S or M
	{
		table_B_er_EC=3;
// 		degree_of_membership_ec1 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 3);
// 		degree_of_membership_ec2 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 4);
		ec_belong1 = FUZZY_S;
		ec_belong2 = FUZZY_M;
	}		
	/////////////////////////////////////////////
	if(error_rate >= EC_S_H && error_rate <=EC_M_H)  //EC maybe belongs to M or L
	{
		table_B_er_EC=5;
// 		degree_of_membership_ec1 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 5);
// 		degree_of_membership_ec2 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, 6);
		ec_belong1 = FUZZY_M;
		ec_belong2 = FUZZY_L;
	}		
	
	if(table_B_er_EC!=0x25)
	{
		degree_of_membership_ec1 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, table_B_er_EC);
		degree_of_membership_ec2 = function_of_er(get_Fuzzy.EC_H, get_Fuzzy.EC_L, error_rate, (table_B_er_EC+1));
	}
	if(table_A_er_E!=0x25)
	{
		degree_of_membership_e1 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, table_A_er_E);
		degree_of_membership_e2 = function_of_er(get_Fuzzy.E_H, get_Fuzzy.E_L, error, (table_A_er_E+1));
	}
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX	
	 r_FuzzyPID=Fuzzy_PID::defuzzy(e_belong1, e_belong2, ec_belong1, ec_belong2, \
	                          degree_of_membership_e1, degree_of_membership_e2, \
	                          degree_of_membership_ec1, degree_of_membership_ec2,get_Fuzzy);
	return r_FuzzyPID;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx