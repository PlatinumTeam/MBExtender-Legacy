#ifndef __vorbis_h__
#define __vorbis_h__

	#define naked __declspec(naked)
	#define inline __forceinline


	extern FARPROC mProcs[];
	inline naked int call__floor_P() { __asm { jmp dword ptr [ mProcs + 0 * 4 ] } }
	inline naked int call__mapping_P() { __asm { jmp dword ptr [ mProcs + 1 * 4 ] } }
	inline naked int call__residue_P() { __asm { jmp dword ptr [ mProcs + 2 * 4 ] } }
	inline naked int call_vorbis_analysis() { __asm { jmp dword ptr [ mProcs + 3 * 4 ] } }
	inline naked int call_vorbis_analysis_blockout() { __asm { jmp dword ptr [ mProcs + 4 * 4 ] } }
	inline naked int call_vorbis_analysis_buffer() { __asm { jmp dword ptr [ mProcs + 5 * 4 ] } }
	inline naked int call_vorbis_analysis_headerout() { __asm { jmp dword ptr [ mProcs + 6 * 4 ] } }
	inline naked int call_vorbis_analysis_init() { __asm { jmp dword ptr [ mProcs + 7 * 4 ] } }
	inline naked int call_vorbis_analysis_wrote() { __asm { jmp dword ptr [ mProcs + 8 * 4 ] } }
	inline naked int call_vorbis_bitrate_addblock() { __asm { jmp dword ptr [ mProcs + 9 * 4 ] } }
	inline naked int call_vorbis_bitrate_flushpacket() { __asm { jmp dword ptr [ mProcs + 10 * 4 ] } }
	inline naked int call_vorbis_block_clear() { __asm { jmp dword ptr [ mProcs + 11 * 4 ] } }
	inline naked int call_vorbis_block_init() { __asm { jmp dword ptr [ mProcs + 12 * 4 ] } }
	inline naked int call_vorbis_comment_add() { __asm { jmp dword ptr [ mProcs + 13 * 4 ] } }
	inline naked int call_vorbis_comment_add_tag() { __asm { jmp dword ptr [ mProcs + 14 * 4 ] } }
	inline naked int call_vorbis_comment_clear() { __asm { jmp dword ptr [ mProcs + 15 * 4 ] } }
	inline naked int call_vorbis_comment_init() { __asm { jmp dword ptr [ mProcs + 16 * 4 ] } }
	inline naked int call_vorbis_comment_query() { __asm { jmp dword ptr [ mProcs + 17 * 4 ] } }
	inline naked int call_vorbis_comment_query_count() { __asm { jmp dword ptr [ mProcs + 18 * 4 ] } }
	inline naked int call_vorbis_commentheader_out() { __asm { jmp dword ptr [ mProcs + 19 * 4 ] } }
	inline naked int call_vorbis_dsp_clear() { __asm { jmp dword ptr [ mProcs + 20 * 4 ] } }
	inline naked int call_vorbis_encode_setup_init() { __asm { jmp dword ptr [ mProcs + 21 * 4 ] } }
	inline naked int call_vorbis_encode_setup_managed() { __asm { jmp dword ptr [ mProcs + 22 * 4 ] } }
	inline naked int call_vorbis_encode_setup_vbr() { __asm { jmp dword ptr [ mProcs + 23 * 4 ] } }
	inline naked int call_vorbis_info_blocksize() { __asm { jmp dword ptr [ mProcs + 24 * 4 ] } }
	inline naked int call_vorbis_info_clear() { __asm { jmp dword ptr [ mProcs + 25 * 4 ] } }
	inline naked int call_vorbis_info_init() { __asm { jmp dword ptr [ mProcs + 26 * 4 ] } }
	inline naked int call_vorbis_packet_blocksize() { __asm { jmp dword ptr [ mProcs + 27 * 4 ] } }
	inline naked int call_vorbis_synthesis() { __asm { jmp dword ptr [ mProcs + 28 * 4 ] } }
	inline naked int call_vorbis_synthesis_blockin() { __asm { jmp dword ptr [ mProcs + 29 * 4 ] } }
	inline naked int call_vorbis_synthesis_headerin() { __asm { jmp dword ptr [ mProcs + 30 * 4 ] } }
	inline naked int call_vorbis_synthesis_init() { __asm { jmp dword ptr [ mProcs + 31 * 4 ] } }
	inline naked int call_vorbis_synthesis_pcmout() { __asm { jmp dword ptr [ mProcs + 32 * 4 ] } }
	inline naked int call_vorbis_synthesis_read() { __asm { jmp dword ptr [ mProcs + 33 * 4 ] } }
	inline naked int call_vorbis_synthesis_trackonly() { __asm { jmp dword ptr [ mProcs + 34 * 4 ] } }

#endif // __vorbis_h__
