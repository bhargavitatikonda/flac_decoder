/*
 * FLAC Decoder
 *
 * This program has modified by Renesas Electronics Corporation.
 * The original source code is open source software under the new BSD
 * License.
 *
 * File Name    : stream_decoder.h
 * $Rev: 2083 $
 * $Date:: 2014-05-09 08:52:44 +0900#$
 */

/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000-2009  Josh Coalson
 * Copyright (C) 2011-2013  Xiph.Org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAC__STREAM_DECODER_H
#define FLAC__STREAM_DECODER_H

#include "export.h"
#include "format.h"

#include "flacd_Lib.h"

#ifdef __cplusplus
extern "C" {
#endif


/** \file include/FLAC/stream_decoder.h
 *
 *  \brief
 *  This module contains the functions which implement the stream
 *  decoder.
 *
 *  See the detailed documentation in the
 *  \link flac_stream_decoder stream decoder \endlink module.
 */

/** \defgroup flac_decoder FLAC/ \*_decoder.h: decoder interfaces
 *  \ingroup flac
 *
 *  \brief
 *  This module describes the decoder layers provided by libFLAC.
 *
 * The stream decoder can be used to decode complete streams either from
 * the client via callbacks, or directly from a file, depending on how
 * it is initialized.  When decoding via callbacks, the client provides
 * callbacks for reading FLAC data and writing decoded samples, and
 * handling metadata and errors.  If the client also supplies seek-related
 * callback, the decoder function for sample-accurate seeking within the
 * FLAC input is also available.  When decoding from a file, the client
 * needs only supply a filename or open \c FILE* and write/metadata/error
 * callbacks; the rest of the callbacks are supplied internally.  For more
 * info see the \link flac_stream_decoder stream decoder \endlink module.
 */

/** \defgroup flac_stream_decoder FLAC/stream_decoder.h: stream decoder interface
 *  \ingroup flac_decoder
 *
 *  \brief
 *  This module contains the functions which implement the stream
 *  decoder.
 *
 * The stream decoder can decode native FLAC, and optionally Ogg FLAC
 * (check FLAC_API_SUPPORTS_OGG_FLAC) streams and files.
 *
 * The basic usage of this decoder is as follows:
 * - The program creates an instance of a decoder using
 *   FLAC__stream_decoder_new().
 * - The program overrides the default settings using
 *   FLAC__stream_decoder_set_*() functions.
 * - The program initializes the instance to validate the settings and
 *   prepare for decoding using
 *   - FLAC__stream_decoder_init_stream() or FLAC__stream_decoder_init_FILE()
 *     or FLAC__stream_decoder_init_file() for native FLAC,
 *   - FLAC__stream_decoder_init_ogg_stream() or FLAC__stream_decoder_init_ogg_FILE()
 *     or FLAC__stream_decoder_init_ogg_file() for Ogg FLAC
 * - The program calls the FLAC__stream_decoder_process_*() functions
 *   to decode data, which subsequently calls the callbacks.
 * - The program finishes the decoding with FLAC__stream_decoder_finish(),
 *   which flushes the input and output and resets the decoder to the
 *   uninitialized state.
 * - The instance may be used again or deleted with
 *   FLAC__stream_decoder_delete().
 *
 * In more detail, the program will create a new instance by calling
 * FLAC__stream_decoder_new(), then call FLAC__stream_decoder_set_*()
 * functions to override the default decoder options, and call
 * one of the FLAC__stream_decoder_init_*() functions.
 *
 * There are three initialization functions for native FLAC, one for
 * setting up the decoder to decode FLAC data from the client via
 * callbacks, and two for decoding directly from a FLAC file.
 *
 * For decoding via callbacks, use FLAC__stream_decoder_init_stream().
 * You must also supply several callbacks for handling I/O.  Some (like
 * seeking) are optional, depending on the capabilities of the input.
 *
 * For decoding directly from a file, use FLAC__stream_decoder_init_FILE()
 * or FLAC__stream_decoder_init_file().  Then you must only supply an open
 * \c FILE* or filename and fewer callbacks; the decoder will handle
 * the other callbacks internally.
 *
 * There are three similarly-named init functions for decoding from Ogg
 * FLAC streams.  Check \c FLAC_API_SUPPORTS_OGG_FLAC to find out if the
 * library has been built with Ogg support.
 *
 * Once the decoder is initialized, your program will call one of several
 * functions to start the decoding process:
 *
 * - FLAC__stream_decoder_process_single() - Tells the decoder to process at
 *   most one metadata block or audio frame and return, calling either the
 *   metadata callback or write callback, respectively, once.  If the decoder
 *   loses sync it will return with only the error callback being called.
 * - FLAC__stream_decoder_process_until_end_of_metadata() - Tells the decoder
 *   to process the stream from the current location and stop upon reaching
 *   the first audio frame.  The client will get one metadata, write, or error
 *   callback per metadata block, audio frame, or sync error, respectively.
 * - FLAC__stream_decoder_process_until_end_of_stream() - Tells the decoder
 *   to process the stream from the current location until the read callback
 *   returns FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM or
 *   FLAC__STREAM_DECODER_READ_STATUS_ABORT.  The client will get one metadata,
 *   write, or error callback per metadata block, audio frame, or sync error,
 *   respectively.
 *
 * When the decoder has finished decoding (normally or through an abort),
 * the instance is finished by calling FLAC__stream_decoder_finish(), which
 * ensures the decoder is in the correct state and frees memory.  Then the
 * instance may be deleted with FLAC__stream_decoder_delete() or initialized
 * again to decode another stream.
 *
 * Seeking is exposed through the FLAC__stream_decoder_seek_absolute() method.
 * At any point after the stream decoder has been initialized, the client can
 * call this function to seek to an exact sample within the stream.
 * Subsequently, the first time the write callback is called it will be
 * passed a (possibly partial) block starting at that sample.
 *
 * If the client cannot seek via the callback interface provided, but still
 * has another way of seeking, it can flush the decoder using
 * FLAC__stream_decoder_flush() and start feeding data from the new position
 * through the read callback.
 *
 * The stream decoder also provides MD5 signature checking.  If this is
 * turned on before initialization, FLAC__stream_decoder_finish() will
 * report when the decoded MD5 signature does not match the one stored
 * in the STREAMINFO block.  MD5 checking is automatically turned off
 * (until the next FLAC__stream_decoder_reset()) if there is no signature
 * in the STREAMINFO block or when a seek is attempted.
 *
 * The FLAC__stream_decoder_set_metadata_*() functions deserve special
 * attention.  By default, the decoder only calls the metadata_callback for
 * the STREAMINFO block.  These functions allow you to tell the decoder
 * explicitly which blocks to parse and return via the metadata_callback
 * and/or which to skip.  Use a FLAC__stream_decoder_set_metadata_respond_all(),
 * FLAC__stream_decoder_set_metadata_ignore() ... or FLAC__stream_decoder_set_metadata_ignore_all(),
 * FLAC__stream_decoder_set_metadata_respond() ... sequence to exactly specify
 * which blocks to return.  Remember that metadata blocks can potentially
 * be big (for example, cover art) so filtering out the ones you don't
 * use can reduce the memory requirements of the decoder.  Also note the
 * special forms FLAC__stream_decoder_set_metadata_respond_application(id)
 * and FLAC__stream_decoder_set_metadata_ignore_application(id) for
 * filtering APPLICATION blocks based on the application ID.
 *
 * STREAMINFO and SEEKTABLE blocks are always parsed and used internally, but
 * they still can legally be filtered from the metadata_callback.
 *
 * \note
 * The "set" functions may only be called when the decoder is in the
 * state FLAC__STREAM_DECODER_UNINITIALIZED, i.e. after
 * FLAC__stream_decoder_new() or FLAC__stream_decoder_finish(), but
 * before FLAC__stream_decoder_init_*().  If this is the case they will
 * return \c true, otherwise \c false.
 *
 * \note
 * FLAC__stream_decoder_finish() resets all settings to the constructor
 * defaults, including the callbacks.
 *
 * \{
 */


/** State values for a FLAC__StreamDecoder
 *
 * The decoder's state can be obtained by calling FLAC__stream_decoder_get_state().
 */
typedef enum {

	FLAC__STREAM_DECODER_SEARCH_FOR_METADATA = 0,
	/**< The decoder is ready to search for metadata. */

	FLAC__STREAM_DECODER_READ_METADATA,
	/**< The decoder is ready to or is in the process of reading metadata. */

	FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC,
	/**< The decoder is ready to or is in the process of searching for the
	 * frame sync code.
	 */

	FLAC__STREAM_DECODER_READ_FRAME,
	/**< The decoder is ready to or is in the process of reading a frame. */

	FLAC__STREAM_DECODER_END_OF_STREAM,
	/**< The decoder has reached the end of the stream. */

	FLAC__STREAM_DECODER_OGG_ERROR,
	/**< An error occurred in the underlying Ogg layer.  */

	FLAC__STREAM_DECODER_SEEK_ERROR,
	/**< An error occurred while seeking.  The decoder must be flushed
	 * with FLAC__stream_decoder_flush() or reset with
	 * FLAC__stream_decoder_reset() before decoding can continue.
	 */

	FLAC__STREAM_DECODER_ABORTED,
	/**< The decoder was aborted by the read callback. */

	FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR,
	/**< An error occurred allocating memory.  The decoder is in an invalid
	 * state and can no longer be used.
	 */

	FLAC__STREAM_DECODER_UNINITIALIZED
	/**< The decoder is in the uninitialized state; one of the
	 * FLAC__stream_decoder_init_*() functions must be called before samples
	 * can be processed.
	 */

} FLAC__StreamDecoderState;


/** Possible return values for the FLAC__stream_decoder_init_*() functions.
 */
typedef enum {

	FLAC__STREAM_DECODER_INIT_STATUS_OK = 0,
	/**< Initialization was successful. */

	FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER,
	/**< The library was not compiled with support for the given container
	 * format.
	 */

	FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS,
	/**< A required callback was not supplied. */

	FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR,
	/**< An error occurred allocating memory. */

	FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE,
	/**< fopen() failed in FLAC__stream_decoder_init_file() or
	 * FLAC__stream_decoder_init_ogg_file(). */

	FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED
	/**< FLAC__stream_decoder_init_*() was called when the decoder was
	 * already initialized, usually because
	 * FLAC__stream_decoder_finish() was not called.
	 */

} FLAC__StreamDecoderInitStatus;


/** Return values for the FLAC__StreamDecoder read callback.
 */
typedef enum {

	FLAC__STREAM_DECODER_READ_STATUS_CONTINUE,
	/**< The read was OK and decoding can continue. */

	FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM,
	/**< The read was attempted while at the end of the stream.  Note that
	 * the client must only return this value when the read callback was
	 * called when already at the end of the stream.  Otherwise, if the read
	 * itself moves to the end of the stream, the client should still return
	 * the data and \c FLAC__STREAM_DECODER_READ_STATUS_CONTINUE, and then on
	 * the next read callback it should return
	 * \c FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM with a byte count
	 * of \c 0.
	 */

	FLAC__STREAM_DECODER_READ_STATUS_ABORT
	/**< An unrecoverable error occurred.  The decoder will return from the process call. */

} FLAC__StreamDecoderReadStatus;


/** Possible values passed back to the FLAC__StreamDecoder error callback.
 *  \c FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC is the generic catch-
 *  all.  The rest could be caused by bad sync (false synchronization on
 *  data that is not the start of a frame) or corrupted data.  The error
 *  itself is the decoder's best guess at what happened assuming a correct
 *  sync.  For example \c FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER
 *  could be caused by a correct sync on the start of a frame, but some
 *  data in the frame header was corrupted.  Or it could be the result of
 *  syncing on a point the stream that looked like the starting of a frame
 *  but was not.  \c FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM
 *  could be because the decoder encountered a valid frame made by a future
 *  version of the encoder which it cannot parse, or because of a false
 *  sync making it appear as though an encountered frame was generated by
 *  a future encoder.
 */
typedef enum {

	FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC,
	/**< An error in the stream caused the decoder to lose synchronization. */

	FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER,
	/**< The decoder encountered a corrupted frame header. */

	FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH,
	/**< The frame's data did not match the CRC in the footer. */

	FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM
	/**< The decoder encountered reserved fields in use in the stream. */

	,FLAC__STREAM_DECODER_ERROR_STATUS_CHANGE_HEADER
	,FLAC__STREAM_DECODER_ERROR_STATUS_UNSUPPORTED_STREAM
	,FLAC__STREAM_DECODER_ERROR_STATUS_ASSERT

} FLAC__StreamDecoderErrorStatus;


/***********************************************************************
 *
 * class FLAC__StreamDecoder
 *
 ***********************************************************************/

struct FLAC__StreamDecoderProtected;
struct FLAC__StreamDecoderPrivate;
/** The opaque structure definition for the stream decoder type.
 *  See the \link flac_stream_decoder stream decoder module \endlink
 *  for a detailed description.
 */
typedef struct {
	struct FLAC__StreamDecoderProtected *protected_; /* avoid the C++ keyword 'protected' */
	struct FLAC__StreamDecoderPrivate *private_; /* avoid the C++ keyword 'private' */
} FLAC__StreamDecoder;


/***********************************************************************
 *
 * Class constructor/destructor
 *
 ***********************************************************************/

/** Create a new stream decoder instance.  The instance is created with
 *  default settings; see the individual FLAC__stream_decoder_set_*()
 *  functions for each setting's default.
 *
 * \retval FLAC__StreamDecoder*
 *    \c NULL if there was an error allocating memory, else the new instance.
 */
FLAC_API FLAC__StreamDecoder *FLAC__stream_decoder_new(const flacd_workMemoryInfo* const pWorkMemInfo);


/***********************************************************************
 *
 * Public class method prototypes
 *
 ***********************************************************************/

/** Get the current decoder state.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval FLAC__StreamDecoderState
 *    The current decoder state.
 */
FLAC_API FLAC__StreamDecoderState FLAC__stream_decoder_get_state(const FLAC__StreamDecoder *decoder);

/** Get the current number of channels in the stream being decoded.
 *  Will only be valid after decoding has started and will contain the
 *  value from the most recently decoded frame header.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval unsigned
 *    See above.
 */
FLAC_API unsigned FLAC__stream_decoder_get_channels(const FLAC__StreamDecoder *decoder);

/** Get the current channel assignment in the stream being decoded.
 *  Will only be valid after decoding has started and will contain the
 *  value from the most recently decoded frame header.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval FLAC__ChannelAssignment
 *    See above.
 */
FLAC_API FLAC__ChannelAssignment FLAC__stream_decoder_get_channel_assignment(const FLAC__StreamDecoder *decoder);

/** Get the current sample resolution in the stream being decoded.
 *  Will only be valid after decoding has started and will contain the
 *  value from the most recently decoded frame header.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval unsigned
 *    See above.
 */
FLAC_API unsigned FLAC__stream_decoder_get_bits_per_sample(const FLAC__StreamDecoder *decoder);

/** Get the current sample rate in Hz of the stream being decoded.
 *  Will only be valid after decoding has started and will contain the
 *  value from the most recently decoded frame header.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval unsigned
 *    See above.
 */
FLAC_API unsigned FLAC__stream_decoder_get_sample_rate(const FLAC__StreamDecoder *decoder);

/** Get the current blocksize of the stream being decoded.
 *  Will only be valid after decoding has started and will contain the
 *  value from the most recently decoded frame header.
 *
 * \param  decoder  A decoder instance to query.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval unsigned
 *    See above.
 */
FLAC_API unsigned FLAC__stream_decoder_get_blocksize(const FLAC__StreamDecoder *decoder);

/** Initialize the decoder instance to decode native FLAC streams.
 *
 *  This flavor of initialization sets up the decoder to decode from a
 *  native FLAC stream. I/O is performed via callbacks to the client.
 *  For decoding from a plain file via filename or open FILE*,
 *  FLAC__stream_decoder_init_file() and FLAC__stream_decoder_init_FILE()
 *  provide a simpler interface.
 *
 *  This function should be called after FLAC__stream_decoder_new() and
 *  FLAC__stream_decoder_set_*() but before any of the
 *  FLAC__stream_decoder_process_*() functions.  Will set and return the
 *  decoder state, which will be FLAC__STREAM_DECODER_SEARCH_FOR_METADATA
 *  if initialization succeeded.
 *
 * \param  decoder            An uninitialized decoder instance.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval FLAC__StreamDecoderInitStatus
 *    \c FLAC__STREAM_DECODER_INIT_STATUS_OK if initialization was successful;
 *    see FLAC__StreamDecoderInitStatus for the meanings of other return values.
 */
FLAC_API FLAC__StreamDecoderInitStatus FLAC__stream_decoder_init_stream(
	FLAC__StreamDecoder *decoder
);

/** Flush the stream input.
 *  The decoder's input buffer will be cleared and the state set to
 *  \c FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC.  This will also turn
 *  off MD5 checking.
 *
 * \param  decoder  A decoder instance.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval FLAC__bool
 *    \c true if successful, else \c false if a memory allocation
 *    error occurs (in which case the state will be set to
 *    \c FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR).
 */
FLAC_API FLAC__bool FLAC__stream_decoder_flush(FLAC__StreamDecoder *decoder);

/** Reset the decoding process.
 *  The decoder's input buffer will be cleared and the state set to
 *  \c FLAC__STREAM_DECODER_SEARCH_FOR_METADATA.  This is similar to
 *  FLAC__stream_decoder_finish() except that the settings are
 *  preserved; there is no need to call FLAC__stream_decoder_init_*()
 *  before decoding again.  MD5 checking will be restored to its original
 *  setting.
 *
 *  If the decoder is seekable, or was initialized with
 *  FLAC__stream_decoder_init*_FILE() or FLAC__stream_decoder_init*_file(),
 *  the decoder will also attempt to seek to the beginning of the file.
 *  If this rewind fails, this function will return \c false.  It follows
 *  that FLAC__stream_decoder_reset() cannot be used when decoding from
 *  \c stdin.
 *
 *  If the decoder was initialized with FLAC__stream_encoder_init*_stream()
 *  and is not seekable (i.e. no seek callback was provided or the seek
 *  callback returns \c FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED), it
 *  is the duty of the client to start feeding data from the beginning of
 *  the stream on the next FLAC__stream_decoder_process() or
 *  FLAC__stream_decoder_process_interleaved() call.
 *
 * \param  decoder  A decoder instance.
 * \assert
 *    \code decoder != NULL \endcode
 * \retval FLAC__bool
 *    \c true if successful, else \c false if a memory allocation occurs
 *    (in which case the state will be set to
 *    \c FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR) or a seek error
 *    occurs (the state will be unchanged).
 */
FLAC_API FLAC__bool FLAC__stream_decoder_reset(FLAC__StreamDecoder *decoder);

#if 1
FLAC_API void FLAC__stream_decoder_set_supported_channels(FLAC__StreamDecoder* const decoder, unsigned const input_channels, unsigned const output_channels);
FLAC_API void FLAC__stream_decoder_set_streaminfo(FLAC__StreamDecoder* const decoder, const flacd_decConfigInfo* const pDecConfigInfo);
FLAC_API FLAC__StreamDecoderErrorStatus FLAC__stream_decoder_get_error_state(const FLAC__StreamDecoder* const decoder);
FLAC_API void FLAC__stream_decoder_allocate_rice_contents(FLAC__StreamDecoder* const decoder, unsigned ** const pParameters, unsigned ** const pRaw_bits);
FLAC_API void FLAC__stream_decoder_allocate_residual(FLAC__StreamDecoder* const decoder, FLAC__int32** const pResidual);
FLAC_API void FLAC__stream_decoder_allocate_output(FLAC__StreamDecoder* const decoder, FLAC__int32** const pOutput);
FLAC_API FLAC__bool FLAC__stream_decoder_process_frame_sync(FLAC__StreamDecoder* const decoder);
FLAC_API FLAC__bool FLAC__stream_decoder_process_read_frame(FLAC__StreamDecoder* const decoder, FLAC__bool* const got_a_frame);
#endif
/* \} */

#ifdef __cplusplus
}
#endif

#endif
