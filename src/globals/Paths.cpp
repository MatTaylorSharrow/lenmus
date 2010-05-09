//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Paths.h"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if defined(_LM_LINUX_) && !defined(_LM_DEBUG_) && !defined(_LM_CODEBLOCKS_)
    #include "../../config.h"
#elif defined(_LM_CODEBLOCKS_)
    #define PACKAGE_PREFIX "/usr/local/"
#endif


#include "Paths.h"
#include "../app/TheApp.h"

// the config object
extern wxConfigBase *g_pPrefs;


//-------------------------------------------------------------------------------------------
// lmPaths implementation
//-------------------------------------------------------------------------------------------

lmPaths::lmPaths(wxString sBinPath)
{
    //Receives the full path to the LenMus executable folder (/bin) and
    //extracts the root path
    m_sBin = sBinPath;
    //wxLogMessage(_T("[lmPaths::lmPaths] sBinPath='%s'"), sBinPath.c_str());
     m_root.Assign(sBinPath, _T(""), wxPATH_NATIVE);
    m_root.RemoveLastDir();

    // Folders are organized into four groups
    //		1. Software and essentials
    //		2. Logs and temporal files
    //		3. Configuration files, user dependent
    //		4. User scores and samples
	//
	// Only files in the four group can be configured by the user
	//
	// For Windows all folders follow the working copy structure, but for Linux
	// there is a root path for each group. See next table:
    //
    // ------------------------------------------------------------------------------
    //      Linux                       Windows
    //    Default <prefix> = /usr/local
    //
    // 0. The lenmus program
    // ------------------------------------------------------------------------------
    //      <prefix>                    lenmus
    //          + /bin                      + \bin
    //
    // 1. Software and essentials (RootG1):
    // ------------------------------------------------------------------------------
    //      <prefix>/share/lenmus       lenmus
    //          + /xrc                      + \xrc
    //          + /res                      + \res
    //          + /locale                   + \locale
    //          + /books                    + \books
    //          + /templates                + \templates
    //          + /test-scores              + \test-scores
    //
    // 2. Logs and temporal files (RootG2):
    // ------------------------------------------------------------------------------
    //                                  lenmus
    // logs:    /var/log/lenmus             + \logs
    // temp:    /tmp/lenmus                 + \temp
    //
    // 3. Configuration files, user dependent (Root3):
    // ------------------------------------------------------------------------------
    //      ~/.lenmus                    lenmus\bin
    //
    // 4. User scores and samples (RootG4):
    // ------------------------------------------------------------------------------
    //      ~/lenmus                    lenmus\scores
    //          + /scores
	//


	wxFileName path;

#if defined(_LM_WINDOWS_) || defined(_LM_DEBUG_)
    wxFileName oRootG1 = m_root;
    wxFileName oRootG2 = m_root;
    wxFileName oRootG3 = m_root;
    wxFileName oRootG4 = m_root;
    #if defined(_LM_WINDOWS_)
        #if defined(_LM_DEBUG_)
            oRootG3.AppendDir(_T("z_bin"));
        #else
            oRootG3.AppendDir(_T("bin"));
        #endif
    #endif
#elif defined(_LM_LINUX_)
	#if defined PACKAGE_PREFIX
		wxFileName oRootG1( _T(PACKAGE_PREFIX) );        //<prefix>
	#else
    	wxFileName oRootG1 = m_root;        //<prefix>
	#endif
    oRootG1.AppendDir(_T("share"));
    oRootG1.AppendDir(_T("lenmus"));
    wxFileName oRootG3(wxFileName::GetHomeDir() + _T("/.lenmus/"));
    wxFileName oRootG4(wxFileName::GetHomeDir() + _T("/lenmus/"));

#endif

    // Group 1. Software and essentials

    path = oRootG1;
    path.AppendDir(_T("xrc"));
    m_sXrc = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("icons"));
    m_sImages = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("cursors"));
    m_sCursors = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("sounds"));
    m_sSounds = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("locale"));
    m_sLocaleRoot = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("templates"));
    m_sTemplates = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("test-scores"));
    m_sTestScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 2. Logs and temporal files

#if defined(_LM_WINDOWS_) || defined(_LM_DEBUG_)
    path = oRootG2;
    path.AppendDir(_T("temp"));
    m_sTemp = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG2;
    path.AppendDir(_T("logs"));
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

#else
    m_sTemp = _T("/tmp/lenmus/");
    m_sLogs = _T("/var/log/lenmus/");

#endif

    // Group 3. Configuration files, user dependent

    path = oRootG3;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 4. User scores and samples

    path = oRootG4;
    path.AppendDir(_T("scores"));
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    path.AppendDir(_T("samples"));
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


	//create temp folder if it does not exist. Otherwise the program will
    //fail when the user tries to open an eMusicBook
    if (!::wxDirExists(m_sTemp))
	{
		//bypass for bug in unicode build (GTK) for wxMkdir
        //::wxMkDir(m_sTemp.c_str());
		wxFileName oFN(m_sTemp);
		oFN.Mkdir(777);
    }

#if defined(_LM_LINUX_)
    //create folders if they don't exist
    if (!::wxDirExists(m_sLogs))
	{
		wxFileName oFN(m_sLogs);
		oFN.Mkdir(777);
    }
    if (!::wxDirExists(m_sConfig))
	{
		wxFileName oFN(m_sConfig);
		oFN.Mkdir(777);
    }
    if (!::wxDirExists(m_sConfig))
	{
		wxFileName oFN(m_sConfig);
		oFN.Mkdir(777);
    }
#endif

}

lmPaths::~lmPaths()
{
    SaveUserPreferences();
}


void lmPaths::SetLanguageCode(wxString sLangCode)
{
    //
    // Lang code has changed. It is necessary to rebuild paths depending on language
    //
    // IMPORTANT: When this method is invoked wxLocale object is not
    //            yet initialized. DO NOT USE LANGUAGE DEPENDENT STRINGS HERE
    //
    m_sLangCode = sLangCode;
    wxFileName oLocalePath(m_sLocaleRoot, _T(""), wxPATH_NATIVE);
    oLocalePath.AppendDir(m_sLangCode);
    m_sLocale = oLocalePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

#if 0
    wxFileName oBooksPath = m_root;
    oBooksPath.AppendDir(_T("books"));
    oBooksPath.AppendDir(m_sLangCode);
    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
#else
    wxFileName oBooksPath = oLocalePath;
    oBooksPath.AppendDir(_T("books"));
    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
#endif

    wxFileName oHelpPath = oLocalePath;
    oHelpPath.AppendDir(_T("help"));
    m_sHelp = oHelpPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Check this.
	ClearTempFiles();

}


void lmPaths::LoadUserPreferences()
{
    //
    // load settings form user congiguration data or default values
    //

	// Only the path for group 4 files can be selected by the user
    g_pPrefs->Read(_T("/Paths/Scores"), &m_sScores);

}

//! save path settings in user configuration data
void lmPaths::SaveUserPreferences()
{
    //g_pPrefs->Write(_T("/Paths/Locale"), m_sLocaleRoot);
    g_pPrefs->Write(_T("/Paths/Scores"), m_sScores);
    //g_pPrefs->Write(_T("/Paths/Temp"), m_sTemp);
    //g_pPrefs->Write(_T("/Paths/Xrc"), m_sXrc);
    //g_pPrefs->Write(_T("/Paths/Images"), m_sImages);
    //g_pPrefs->Write(_T("/Paths/Sounds"), m_sSounds);
    //g_pPrefs->Write(_T("/Paths/Config"), m_sConfig);
    //g_pPrefs->Write(_T("/Paths/Logs"), m_sLogs);

    // bin path is not user configurable
    //g_pPrefs->Write(_T("/Paths/Bin"), m_sBin);

}

void lmPaths::ClearTempFiles()
{
    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Otherwise, as books have the same names in all languages,
    // in Spanish, the new language .hcc and hhk files will not be properly loaded.
    // Here I test this flag and if true, remove all files in temp folder
    bool fClearTemp;
    g_pPrefs->Read(_T("/Locale/LanguageChanged"), &fClearTemp, false );
    if (fClearTemp) {
        wxString sFile = wxFindFirstFile(m_sTemp);
        while ( !sFile.empty() ) {
            if (!::wxRemoveFile(sFile)) {
                wxLogMessage(_T("[lmPaths::LoadUserPreferences] Error deleting %s"),
                    sFile.c_str() );
            }
            sFile = wxFindNextFile();
        }
        //reset flag
        fClearTemp = false;
        g_pPrefs->Write(_T("/Locale/LanguageChanged"), fClearTemp);
    }

}

