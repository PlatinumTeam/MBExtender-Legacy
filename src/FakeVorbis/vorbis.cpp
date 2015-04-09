// Windows-only: wrapper dll for vorbis.dll which provides an alternate method of launching the game with PluginLoader injected
// This method does not require administrator access, though using a launcher executable should be preferred on other platforms
// Generated using http://floodyberry.wordpress.com/2008/09/08/generating-dll-wrappers/

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include "vorbis.h"
#include "init.h"

#define REAL_DLL_NAME "vorbis-real.dll"

HINSTANCE mHinst, mHinstDLL;
FARPROC mProcs[35];

LPCSTR mImportNames[] = {
	"_floor_P", "_mapping_P", "_residue_P", "vorbis_analysis", 
	"vorbis_analysis_blockout", "vorbis_analysis_buffer", "vorbis_analysis_headerout", "vorbis_analysis_init", 
	"vorbis_analysis_wrote", "vorbis_bitrate_addblock", "vorbis_bitrate_flushpacket", "vorbis_block_clear", 
	"vorbis_block_init", "vorbis_comment_add", "vorbis_comment_add_tag", "vorbis_comment_clear", 
	"vorbis_comment_init", "vorbis_comment_query", "vorbis_comment_query_count", "vorbis_commentheader_out", 
	"vorbis_dsp_clear", "vorbis_encode_setup_init", "vorbis_encode_setup_managed", "vorbis_encode_setup_vbr", 
	"vorbis_info_blocksize", "vorbis_info_clear", "vorbis_info_init", "vorbis_packet_blocksize", 
	"vorbis_synthesis", "vorbis_synthesis_blockin", "vorbis_synthesis_headerin", "vorbis_synthesis_init", 
	"vorbis_synthesis_pcmout", "vorbis_synthesis_read", "vorbis_synthesis_trackonly", 
};

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved ) {
	mHinst = hinstDLL;
	if ( fdwReason == DLL_PROCESS_ATTACH ) {
		mHinstDLL = LoadLibrary( REAL_DLL_NAME );
		if ( !mHinstDLL ) {
			MessageBox( NULL, "Unable to load " REAL_DLL_NAME ".\r\nMake sure the file exists and then try again.", "MBExtender", MB_OK | MB_ICONERROR );
			return ( FALSE );
		}
		for ( int i = 0; i < 35; i++ )
			mProcs[ i ] = GetProcAddress( mHinstDLL, mImportNames[ i ] );

		if ( !injectPluginLoader() ) {
			FreeLibrary( mHinstDLL );
			return ( FALSE );
		}
	} else if ( fdwReason == DLL_PROCESS_DETACH ) {
		FreeLibrary( mHinstDLL );
	}
	return ( TRUE );
}

// _floor_P
int __stdcall __floor_P() {
	return call__floor_P();
}

// _mapping_P
int __stdcall __mapping_P() {
	return call__mapping_P();
}

// _residue_P
int __stdcall __residue_P() {
	return call__residue_P();
}

// vorbis_analysis
int __stdcall _vorbis_analysis() {
	return call_vorbis_analysis();
}

// vorbis_analysis_blockout
int __stdcall _vorbis_analysis_blockout() {
	return call_vorbis_analysis_blockout();
}

// vorbis_analysis_buffer
int __stdcall _vorbis_analysis_buffer() {
	return call_vorbis_analysis_buffer();
}

// vorbis_analysis_headerout
int __stdcall _vorbis_analysis_headerout() {
	return call_vorbis_analysis_headerout();
}

// vorbis_analysis_init
int __stdcall _vorbis_analysis_init() {
	return call_vorbis_analysis_init();
}

// vorbis_analysis_wrote
int __stdcall _vorbis_analysis_wrote() {
	return call_vorbis_analysis_wrote();
}

// vorbis_bitrate_addblock
int __stdcall _vorbis_bitrate_addblock() {
	return call_vorbis_bitrate_addblock();
}

// vorbis_bitrate_flushpacket
int __stdcall _vorbis_bitrate_flushpacket() {
	return call_vorbis_bitrate_flushpacket();
}

// vorbis_block_clear
int __stdcall _vorbis_block_clear() {
	return call_vorbis_block_clear();
}

// vorbis_block_init
int __stdcall _vorbis_block_init() {
	return call_vorbis_block_init();
}

// vorbis_comment_add
int __stdcall _vorbis_comment_add() {
	return call_vorbis_comment_add();
}

// vorbis_comment_add_tag
int __stdcall _vorbis_comment_add_tag() {
	return call_vorbis_comment_add_tag();
}

// vorbis_comment_clear
int __stdcall _vorbis_comment_clear() {
	return call_vorbis_comment_clear();
}

// vorbis_comment_init
int __stdcall _vorbis_comment_init() {
	return call_vorbis_comment_init();
}

// vorbis_comment_query
int __stdcall _vorbis_comment_query() {
	return call_vorbis_comment_query();
}

// vorbis_comment_query_count
int __stdcall _vorbis_comment_query_count() {
	return call_vorbis_comment_query_count();
}

// vorbis_commentheader_out
int __stdcall _vorbis_commentheader_out() {
	return call_vorbis_commentheader_out();
}

// vorbis_dsp_clear
int __stdcall _vorbis_dsp_clear() {
	return call_vorbis_dsp_clear();
}

// vorbis_encode_setup_init
int __stdcall _vorbis_encode_setup_init() {
	return call_vorbis_encode_setup_init();
}

// vorbis_encode_setup_managed
int __stdcall _vorbis_encode_setup_managed() {
	return call_vorbis_encode_setup_managed();
}

// vorbis_encode_setup_vbr
int __stdcall _vorbis_encode_setup_vbr() {
	return call_vorbis_encode_setup_vbr();
}

// vorbis_info_blocksize
int __stdcall _vorbis_info_blocksize() {
	return call_vorbis_info_blocksize();
}

// vorbis_info_clear
int __stdcall _vorbis_info_clear() {
	return call_vorbis_info_clear();
}

// vorbis_info_init
int __stdcall _vorbis_info_init() {
	return call_vorbis_info_init();
}

// vorbis_packet_blocksize
int __stdcall _vorbis_packet_blocksize() {
	return call_vorbis_packet_blocksize();
}

// vorbis_synthesis
int __stdcall _vorbis_synthesis() {
	return call_vorbis_synthesis();
}

// vorbis_synthesis_blockin
int __stdcall _vorbis_synthesis_blockin() {
	return call_vorbis_synthesis_blockin();
}

// vorbis_synthesis_headerin
int __stdcall _vorbis_synthesis_headerin() {
	return call_vorbis_synthesis_headerin();
}

// vorbis_synthesis_init
int __stdcall _vorbis_synthesis_init() {
	return call_vorbis_synthesis_init();
}

// vorbis_synthesis_pcmout
int __stdcall _vorbis_synthesis_pcmout() {
	return call_vorbis_synthesis_pcmout();
}

// vorbis_synthesis_read
int __stdcall _vorbis_synthesis_read() {
	return call_vorbis_synthesis_read();
}

// vorbis_synthesis_trackonly
int __stdcall _vorbis_synthesis_trackonly() {
	return call_vorbis_synthesis_trackonly();
}

