#include <windows.h>
#include <string>
#include "IllegalWindowSpec.h"
#include "wildcard.h"

void GetWindowFileName(HWND window, char* file, int size);

IllegalWindowSpec::IllegalWindowSpec() {
};

BOOL IllegalWindowSpec::match(HWND window)
{
//## begin AutoFunction::Match%-160473114.body preserve=yes
	
	if ( !::IsWindow( window ) ) {
		return FALSE ;
	} else {
		BOOL match = TRUE ;
		BOOL checkedSomething = FALSE ;

		if (m_title.length()) {
			checkedSomething = TRUE ;
			char title[256] ;
			::GetWindowText(window, title, 256) ;
			string Title( title ) ;
			if (!(wildcard(m_title, Title, (m_caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if (m_winClass.length()) {
			checkedSomething = TRUE ;
			char winclass[256] ;
			::GetClassName(window, winclass, 256 ) ;
			string Class(winclass) ;
			if (!(wildcard(m_winClass, Class, (m_caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if (m_filename.length()) {
			checkedSomething = TRUE ;
			char file[256] ;

			GetWindowFileName(window, file, 256);

			string File(file) ;
			if (!(wildcard(m_filename, File, (m_caseSensitive ? FNM_BASHSTYLE | FNM_NOESCAPE : FNM_BASHSTYLE | FNM_CASEFOLD | FNM_NOESCAPE) ))) {
				match = FALSE ;
			}
		}

		if ( !checkedSomething ) {
			match = FALSE ;
		}
		
		return match ;
	}
}
