// IllegalWindowSpec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IllegalWindowSpec dialog
#include <string>

using namespace std;

class IllegalWindowSpec {
public:
	string m_name;
	string m_filename;
	string m_title;
	string m_winClass;
	bool m_caseSensitive;

	IllegalWindowSpec();

	string GetName() { return m_name; };
	string GetFilename() { return m_filename; };
	string GetTitle() { return m_title; };
	string GetClass() { return m_winClass; };
	bool GetCaseSensitive() { return m_caseSensitive; };

	void SetName(string n) {
		m_name = n;
	};

	void SetFilename(string f) {
		m_filename = f;
	};

	void SetTitle(string t) {
		m_title = t;
	};
	
	void SetClass(string c) {
		m_winClass = c;
	};

	void SetCaseSensitive(bool cs) {
		m_caseSensitive = cs;
	};

	BOOL match(HWND window);
};
