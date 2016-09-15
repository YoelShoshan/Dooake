#ifndef YOEL_PARSER
#define YOEL_PARSER


class COldParser
{
public:

	COldParser();
	~COldParser();

	/// on self

	void SetAll(char* pSet);
		
	char* NextWord(void);

	char* NextLine(void);


	char* NextWordIgnoreComments(void);

	char* GoTo(char* find);

	int GetInt(void);
	float GetFloat(void);

	void GetString(char*& get);   // NO alloc

	void GetStringIgnoreSpace(char*& get);
	
	void GetStringAndAlloc(char*& get); // alloc as required
	
	bool GetNextCurlyBracket(char*& get);

	


	// on "outsider"

	void RemoveParenthesis(char*& rem);

	//
	char* m_pTemp;
	//

char* m_pTravel;
char* m_pStart;

int m_iSize;
};

















#endif //YOEL_PARSER