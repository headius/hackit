// AutoFunction.h: interface for the AutoFunction class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUTOFUNCTION_H__996597A1_30DE_11D1_A56F_0040053958BF__INCLUDED_)
#define AFX_AUTOFUNCTION_H__996597A1_30DE_11D1_A56F_0040053958BF__INCLUDED_
#pragma	warning(disable: 4786)
#include <string>

using namespace std;

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WINDOW_ACTION_TEXT {"No Change", "Restore", "Minimize", "Maximize"}

#include <set>
using namespace std;

class HackItDlg ;

class AutoFunction  
{
private:
	set<HWND> m_LastWindows;

	string filename;
	string title;
	string winClass;

	int windowAction;
	int topmostAction;
	int visibilityAction;
	int priorityAction;
	int destructionAction;
	int transparency;
	int resize;
	int resizeW;
	int resizeH;
	int move;
	int moveX;
	int moveY;
	int specialAction;
	string specialData;
	int alternateTarget;
	string alternateData;
	int trayIcon;
	int topOnly;
	int caseSensitive;

public:
	AutoFunction();
	AutoFunction(string filename, string title, string winclass);
	
	string getWinClass() {return winClass;}
	string getTitle() {return title;}
	string getFilename() {return filename;}

	void setWinClass(string w) {winClass = w;}
	void setTitle(string t) {title = t;}
	void setFilename(string f) {filename = f;}

	int getWindowAction() {return windowAction;}
	int getTopmostAction() {return topmostAction;}
	int getVisibilityAction() {return visibilityAction;}
	int getTransparency() {return transparency;}
	int getPriorityAction() {return priorityAction;}
	int getDestructionAction() {return destructionAction;}
	int getResize() {return resize;}
	int getResizeW() {return resizeW;}
	int getResizeH() {return resizeH;}
	int getMove() {return move;}
	int getMoveX() {return moveX;}
	int getMoveY() {return moveY;}
	int getSpecialAction() {return specialAction;}
	string getSpecialData() {return specialData;}
	int getAlternateTarget() {return alternateTarget;}
	string getAlternateData() {return alternateData;}
	int getTrayIcon() {return trayIcon;}
	int getTopOnly() {return topOnly;}
	int getCaseSensitive() {return caseSensitive;}

	void setWindowAction(int action) {windowAction = action;}
	void setTopmostAction(int action) {topmostAction = action;}
	void setVisibilityAction(int action) {visibilityAction = action;}
	void setTransparency(int transparency) {this->transparency = transparency;}
	void setPriorityAction(int action) {priorityAction = action;}
	void setDestructionAction(int action) {destructionAction = action;}
	void setResize(int r) {resize = r;}
	void setResizeW(int w) {resizeW = w;}
	void setResizeH(int h) {resizeH = h;}
	void setMove(int m) {move = m;}
	void setMoveX(int x) {moveX = x;}
	void setMoveY(int y) {moveY = y;}
	void setSpecialAction(int action) {specialAction = action;}
	void setSpecialData(string data) {specialData = data;}
	void setAlternateTarget(int target) {alternateTarget = target;}
	void setAlternateData(string data) {alternateData = data;}
	void setTrayIcon(int tray) {trayIcon = tray;}
	void setTopOnly(int top) {topOnly = top;}
	void setCaseSensitive(int case1) {caseSensitive = case1;}

	BOOL match(HWND window);

	static void execute(HackItDlg *dlg, AutoFunction *af, HWND window);
};	
#endif // !defined(AFX_AUTOFUNCTION_H__996597A1_30DE_11D1_A56F_0040053958BF__INCLUDED_)
