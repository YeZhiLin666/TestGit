
#define FUZZY_Z 3
#define FUZZY_S 2
#define FUZZY_M 1
#define FUZZY_L 0

#define SOM 0
#define LOM 1
#define MOM 2

#define DEFUZZY MOM

//////////////////////////////
// #define E_L -0.0005
// #define E_H 0.0005

// #define EC_L -0.0001
// #define EC_H 0.0001    //EC
// /////////////////////////////////////////////
// #define E_L_L (E_L + ((E_H - E_L)/3)*2)
// #define E_L_H (E_L + ((E_H - E_L)/3)*3)

// #define E_M_L (E_L + ((E_H - E_L)/3)*1)
// #define E_M_H (E_L + ((E_H - E_L)/3)*3)

// #define E_S_L (E_L + ((E_H - E_L)/3)*0)
// #define E_S_H (E_L + ((E_H - E_L)/3)*2)

// #define E_Z_L (E_L + ((E_H - E_L)/3)*0)
// #define E_Z_H (E_L + ((E_H - E_L)/3)*1)
// /////////////////////////////////////////////
// #define EC_L_L (EC_L + ((EC_H - EC_L)/3)*2)
// #define EC_L_H (EC_L + ((EC_H - EC_L)/3)*3)

// #define EC_M_L (EC_L + ((EC_H - EC_L)/3)*1)
// #define EC_M_H (EC_L + ((EC_H - EC_L)/3)*3)

// #define EC_S_L (EC_L + ((EC_H - EC_L)/3)*0)
// #define EC_S_H (EC_L + ((EC_H - EC_L)/3)*2)

// #define EC_Z_L (EC_L + ((EC_H - EC_L)/3)*0)
// #define EC_Z_H (EC_L + ((EC_H - EC_L)/3)*1)
// /////////////////////////////////////////////
// #define KP_L 0
// //#define KP_H 10
// #define KP_F_WIDE (KP_H - KP_L)/3

// #define KI_L 0
// //#define KI_H 300

// #define KI_F_WIDE (KI_H - KI_L)/3

// #define KD_L 0
// //#define KD_H 1
// #define KD_F_WIDE (KD_H - KD_L)/3
// /////////////////////////////////////
// #define KP_L_L (KP_L + ((KP_H - KP_L)/3)*2)
// #define KP_L_H (KP_L + ((KP_H - KP_L)/3)*3)
// #define KP_M_L (KP_L + ((KP_H - KP_L)/3)*1)
// #define KP_M_H (KP_L + ((KP_H - KP_L)/3)*3)
// #define KP_S_L (KP_L + ((KP_H - KP_L)/3)*0)
// #define KP_S_H (KP_L + ((KP_H - KP_L)/3)*2)
// #define KP_Z_L (KP_L + ((KP_H - KP_L)/3)*0)
// #define KP_Z_H (KP_L + ((KP_H - KP_L)/3)*1)

#define KP_L_CENT 0
#define KP_M_CENT 1
#define KP_S_CENT 2
#define KP_Z_CENT 3

// #define KI_L_L (KI_L + ((KI_H - KI_L)/3)*2)
// #define KI_L_H (KI_L + ((KI_H - KI_L)/3)*3)
// #define KI_M_L (KI_L + ((KI_H - KI_L)/3)*1)
// #define KI_M_H (KI_L + ((KI_H - KI_L)/3)*3)
// #define KI_S_L (KI_L + ((KI_H - KI_L)/3)*0)
// #define KI_S_H (KI_L + ((KI_H - KI_L)/3)*2)
// #define KI_Z_L (KI_L + ((KI_H - KI_L)/3)*0)
// #define KI_Z_H (KI_L + ((KI_H - KI_L)/3)*1)

#define KI_L_CENT 0
#define KI_M_CENT 1
#define KI_S_CENT 2
#define KI_Z_CENT 3

// #define KD_L_L (KD_L + ((KD_H - KD_L)/3)*2)
// #define KD_L_H (KD_L + ((KD_H - KD_L)/3)*3)
// #define KD_M_L (KD_L + ((KD_H - KD_L)/3)*1)
// #define KD_M_H (KD_L + ((KD_H - KD_L)/3)*3)
// #define KD_S_L (KD_L + ((KD_H - KD_L)/3)*0)
// #define KD_S_H (KD_L + ((KD_H - KD_L)/3)*2)
// #define KD_Z_L (KD_L + ((KD_H - KD_L)/3)*0)
// #define KD_Z_H (KD_L + ((KD_H - KD_L)/3)*1)

#define KD_L_CENT 0
#define KD_M_CENT 1
#define KD_S_CENT 2
#define KD_Z_CENT 3
typedef struct
{
	float Delta_Kp;
	float Delta_Ki;
	float Delta_Kd;
}GET_FUZZY_PID;

typedef struct 
{
	float KP_H;
	float KI_H;
	float KD_H;
	float KP_L;
	float KI_L;
	float KD_L;
	float E_L;
	float E_H;
	float EC_L;
	float EC_H;
}BASE_FUZZY_PARA;
											
class Fuzzy_PID
{
  public:
		static BASE_FUZZY_PARA Set_FuzzyPara;
	  static float E_L_L;
	  static float E_L_H;
		static float E_M_L;
	  static float E_M_H;
		static float E_S_L;
	  static float E_S_H;
	  static float E_Z_L;
	  static float E_Z_H;
	  static float EC_L_L;
		static float EC_L_H;
		static float EC_M_L;
		static float EC_M_H;
		static float EC_S_L;
		static float EC_S_H;
		static float EC_Z_L;
		static float EC_Z_H;
		static float KP_F_WIDE;
		static float KI_F_WIDE;
		static float KD_F_WIDE;
		static float KP_L_L;
		static float KP_L_H;
		static float KP_M_L;
		static float KP_M_H;
		static float KP_S_L;
		static float KP_S_H;
		static float KP_Z_L;
		static float KP_Z_H;
		
		static float KI_L_L;
		static float KI_L_H;
		static float KI_M_L;
		static float KI_M_H;
		static float KI_S_L;
		static float KI_S_H;
		static float KI_Z_L;
		static float KI_Z_H;
		
		static float KD_L_L;
		static float KD_L_H;
		static float KD_M_L;
		static float KD_M_H;
		static float KD_S_L;
		static float KD_S_H;
		static float KD_Z_L;
		static float KD_Z_H;
		
    static GET_FUZZY_PID fuzzy_pid(float error, float error_rate,BASE_FUZZY_PARA get_Fuzzy);
	  static void InitFuzzy(BASE_FUZZY_PARA m_fuzzy);

	public:

    static float function_out_k(float kh, float kl, unsigned char func_name, float degree_of_membership, char defuzzy_type);
//     static float function_out_ki(float kih, float kil, unsigned char func_name, float degree_of_membership, char defuzzy_type);
//     static float function_out_kd(float kdh, float kdl, unsigned char func_name, float degree_of_membership, char defuzzy_type);
    static float function_of_er(float eh, float el, float x, unsigned char type);
//		static float function_of_ec(float ech, float ecl, float x, unsigned char type);
    static GET_FUZZY_PID defuzzy(char e_belong1, char e_belong2, char ec_belong1, char ec_belong2, \
                          float degree_of_membership_e1, float degree_of_membership_e2, \
                          float degree_of_membership_ec1, float degree_of_membership_ec2,BASE_FUZZY_PARA m_fuzzy );
    
};



