/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL) as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   This KMD5 class is based on a C++ implementation of
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" by
   Mordechai T. Abzug,	Copyright (c) 1995.  This implementation
   passes the test-suite as defined by RFC 1321.

   RFC 1321 "MD5 Message-Digest Algorithm" Copyright (C) 1991-1992.
   RSA Data Security, Inc. Created 1991. All rights reserved.

   The encoding and decoding utilities in KCodecs with the exception of
   quoted-printable were ported from the HTTPClient java package by Ronald
   Tschal�r Copyright (C) 1996-1999. The quoted-printable codec as described
   in RFC 2045, section 6.7. is by Rik Hemsley (C) 2001.
*/

#ifndef _KMDBASE_H
#define _KMDBASE_H

#define KBase64 KCodecs

#include <stdio.h> // for FILE
#include <qglobal.h>
#include <qstring.h>

typedef char HASH[16];
typedef char HASHHEX[33];

/**
 * A wrapper class for the most commonly used encoding and
 * decoding algorithms.  Currently there is support for encoding
 * and decoding input using base64, uu and the quoted-printable
 * specifications.
 *
 * @sect Useage:
 *
 * The convienence functions are well suited for encoding
 * and decoding a relatively small input.
 *
 * <PRE>
 * QCString input = "Aladdin:open sesame";
 * QCString result = KCodecs::base64Encode(input);
 * printf ("Result: %c", result.data());
 *
 * Output should be
 * Result: QWxhZGRpbjpvcGVuIHNlc2FtZQ==
 * </PRE>
 *
 * @short A collection of commonly used encoding and decoding algorithms.
 * @author Dawit Alemayehu <adawit@kde.org>
 * @author Rik Hemsley <rik@kde.org>
 */
class KCodecs
{
public:

  /**
   * Encodes the given data using the quoted-printable algorithm.
   * Equivalent to quotedPrintableEncode(in, true).
   *
   * @param in  data to be encoded.
   * @return    quoted-printable encoded data.
   */
  static QCString quotedPrintableEncode(const QByteArray & in);

  /**
   * Encodes the given data using the quoted-printable algorithm.
   *
   * @param in   data to be encoded.
   * @param useCRLF  if true the input data is expected to have
   *                 CRLF line breaks and the output will have CRLF line breaks, too.
   * @return     quoted-printable encoded data.
   */
  static QCString quotedPrintableEncode(const QByteArray & in, bool useCRLF);

  /**
   * Decodes a quoted-printable encoded string.
   * Accepts data with CRLF or standard unix line breaks
   *
   * @param in  the data to be decoded.
   * @return    decoded data.
   */
  static QByteArray quotedPrintableDecode(const QCString & in);

  /**
   * Encodes the given data using the uuencode algorithm.
   *
   * The output is split into lines starting with the number of
   * encoded octets in the line and ending with a newline.  No
   * line is longer than 45 octets (60 characters), excluding the
   * line terminator.
   *
   * @param in   the data to be uuencoded
   * @return     a uuencoded data.
   */
  static QCString uuencode( const QByteArray& in );

  /**
   * Encodes the given data using the uuencode algorithm.
   *
   * Use this function if you want the result of the encoding
   * to be placed in another array and cut down the number of
   * copy opertation that have to be performed in the process.
   *
   * <u>NOTE:</u> the output array is always first reset for
   * sanity and then resized appropriatly.  Hence, any data
   * that is present in the output array will be lost.
   *
   * @param in   the data to be uuencoded.
   * @param out  the container for the uudecoded data.
   */
  static void uuencode( const QByteArray& in, QByteArray& out );

  /**
   * Encodes the given string using the uuencode algorithm.
   *
   * @param str   the string to be uuencoded.
   * @return      a uuencoded string.
   */
  static QCString uuencode( const QCString& str );

  /**
   * Decodes the given data using the uuencode algorithm.
   *
   * Any 'begin' and 'end' lines liks those generated by
   * *nix utilities will automatically be ignored.
   *
   * @param in   the data uuencoded data to be decoded.
   * @return     the decoded data.
   */
  static QCString uudecode( const QByteArray& in );

  /**
   * Decodes the given data using the uudecode algorithm.
   *
   * Use this function if you want the result of the decoding
   * to be placed in another array and cut down the number of
   * copy opertation that have to be performed in the process.
   *
   * Any 'begin' and 'end' lines liks those generated by
   * *nix utilities will automatically be ignored.
   *
   * <u>IMPORTANT:</u> the output array is first reset and then
   * resized appropriately.  Hence, any data that is present in
   * the output array will be lost.
   *
   * @param in   the uuencoded-data to be decoded.
   * @param out  the container for the uudecoded data.
   */
  static void uudecode( const QByteArray& in, QByteArray& out );

  /**
   * Decodes a uuencoded string.
   *
   * @param str   the string to be decoded.
   * @return      a uudecoded string.
   */
  static QCString uudecode( const QCString& str );

  /**
   * Encodes the given data using the base64 algorithm.
   *
   * The boolean argument determines if the encoded data is
   * going to be restricted to 76 characters or less per line
   * as specified by RFC 2045.  If @p insertLFs is true, then
   * there will be 76 characters or less per line.
   *
   * @param in         the data to be encoded.
   * @param insertLFs  limit the number of characters per line.
   * @return           a base64 encoded data.
   */
  static QCString base64Encode( const QByteArray& in, bool insertLFs );

  /**
   * @deprecated.
   *
   * Use @ref base64Encode(const QByteArray&, bool)
   * with the boolean argument set to false.
   */
   // BC: Merge in KDE 3.x with the above function by
   // defaulting boolean argument to false.
  static QCString base64Encode( const QByteArray& in );

  /**
   * Encodes the given data using the base64 algorithm.
   *
   * Use this function whenever you are dealing with very
   * large data or a stream of data.  The boolean argument
   * determines if the encoded data is going to be restricted
   * to 76 characters or less per line as specified by RFC 2045.
   * If @p insertLFs is true, then there will be 76 characters or
   * less per line.
   *
   * <u>NOTE:</u> the output array is always first reset for
   * sanity and then resized appropriatly.  Hence, any data
   * that is present in the output array will be lost.
   *
   * @param in        the data to be encoded using base64.
   * @param insertLFs limit the number of characters per line.
   * @param out       the container for the encoded data.
   */
  static void base64Encode( const QByteArray& in, QByteArray& out,
                            bool insertLFs );

  /**
   * @deprecated.
   *
   * Use @ref base64Encode(const QByteArray&, QByteArray&, bool)
   * with the boolean argument set to false.
   */
   // BC: Merge in KDE 3.x with the above function by
   // defaulting boolean argument to false.
  static void base64Encode( const QByteArray& in, QByteArray& out );

  /**
   * Encodes the given string using the base64 algorithm.
   *
   * The boolean argument determines if the encoded data is
   * going to be restricted to 76 characters or less per line
   * as specified by RFC 2045.  If @p insertLFs is true, then
   * there will be 76 characters or less per line.
   *
   * @param str       the string to be encoded.
   * @param insertLFs limit the number of characters per line.
   * @return          the decoded string.
   */
  static QCString base64Encode( const QCString& str, bool insertLFs );

  /**
   * @deprecated.
   *
   * Use @ref base64Encode(const QCString&, bool)
   * with the boolean argument set to false.
   */
   // BC: Merge in KDE 3.x with the above function by
   // defaulting boolean argument to false.
  static QCString base64Encode( const QCString& str );

  /**
   * Decodes the given data that was encoded using the
   * base64 algorithm.
   *
   * @param in   the base64-encoded data to be decoded.
   * @return     the decoded data.
   */
  static QCString base64Decode( const QByteArray& in );

  /**
   * Decodes the given data that was encoded with the base64
   * algorithm.
   *
   * Use this function if you want the result of the decoding
   * to be placed in another array and cut down the number of
   * copy opertation that have to be performed in the process.
   *
   * <u>IMPORTANT:</u> the output array is first reset and then
   * resized appropriately.  Hence, any data that is present in
   * the output array will be lost.
   *
   * @param in   the encoded data to be decoded.
   * @param out  the container for the decoded data.
   */
  static void base64Decode( const QByteArray& in, QByteArray& out );

  /**
   * Decodes a string encoded with the base64 algorithm.
   *
   * @param str  the base64-encoded string.
   * @return     the decoded string.
   */
  static QCString base64Decode( const QCString& str );

  /**
   * Encodes the QString data using the base64 algorithm.
   *
   * <u>NOTE:</u> This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrectly
   * encoded data since the conversion of the QString input to latin-1
   * can and will result in data loss if the input data contains non-
   * latin1 characters.  As such it is highly recommended that you avoid
   * this function unless you are absolutely certain that your input
   * does not contain any non-latin1 character!!
   */
  static QString base64Encode( const QString& str );

  /**
   * Decodes the encoded QString data using the base64 algorithm.
   *
   * <u>NOTE:</u> This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrectly
   * decoded data since the conversion of the QString input to latin-1
   * can and will result in data loss if the input data contains non-
   * latin1 characters.  As such it is highly recommended that you avoid
   * this function unless you are absolutely certain that your input
   * does not contain any non-latin1 character!!
   */
  static QString base64Decode( const QString& str );

  /**
   * Encodes the QString data using the uuencode algorithm.
   *
   * <u>NOTE:</u> This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrectly
   * encoded data since the conversion of the QString input to latin-1
   * can and will result in data loss if the input data contains non-
   * latin1 characters.  As such it is highly recommended that you avoid
   * this function unless you are absolutely certain that your input
   * does not contain any non-latin1 character!!
   */
  static QString uuencode( const QString& str );

  /**
   * Decodes the QString data using the uuencode algorithm.
   *
   * <u>NOTE:</u> This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrectly
   * decoded data since the conversion of the QString input to latin-1
   * can and will result in data loss if the input data contains non-
   * latin1 characters.  As such it is highly recommended that you avoid
   * this function unless you are absolutely certain that your input
   * does not contain any non-latin1 character!!
   */
  static QString uudecode( const QString& str );

  /**
   * @deprecated. Use @ref base64Encode(const QString&) instead.
   */
  static QString encodeString( const QString& data );

  /**
   * @deprecated. Use @ref base64Decode(const QString&) instead.
   */
  static QString decodeString( const QString& data );

private:
  KCodecs();

private:
  static char UUEncMap[64];
  static char UUDecMap[128];
  static char Base64EncMap[64];
  static char Base64DecMap[128];
  static char hexChars[16];
  static const unsigned int maxQPLineLength;
};

/**
 * Provides an easy to use C++ implementation of RSA's
 * MD5 algorithm.
 *
 * The default constructor is designed to provide much the same
 * functionality as the most commonly used C-implementation while
 * the other three constructors are meant to further simplify the
 * message digest calculations by calculating the result in one
 * single step.  Additionally, you have the ability to obtain the
 * result in either raw (16-bytes) or hexidecimal formats (33-bytes)
 * using @ref rawDigest and @ref hexDigest respectivelly, you can also
 * reuse a single instance to make multiple message digest calculations
 * by simply invoking @reset().
 *
 * @sect Useage:
 *
 * A common useage of this class:
 *
 * <PRE>
 *  HASH rawResult;
 *  HASHHEX hexResult;
 *  QCString test1 = "This is a simple test.";
 *
 *  KMD5 context( test1 );
 *  context.rawDigest( rawResult );
 *  context.hexDigest( hexResult );
 *  printf ( "Raw Digest output: %s", rawResult );
 *  printf ( "Hex Digest output: %s", hexResult );
 * </PRE>
 *
 * To cut down on the unnecessary overhead of creating multiple KMD5 objects,
 * you can simply invoke @ref reset() to resue the same object in making another
 * calculation:
 *
 * <PRE>
 *  context.reset();
 *  context.update( QCString("TWO") );
 *  context.update( QCString("THREE") );
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * NOTE: Invoke @ref reset() deletes the previously calculated message
 * digest value.  Thus, be sure to copy the previous result before you
 * reuse the same object!  Also, if you use one of the convienence
 * constructors you must first invoke reset(), before calling any of the
 * update functions.  Otherwise, the call to @ref update will fail.
 *
 * Here is an of KMD5 useage much along the same lines of how one would
 * use the commonly available C-implementations of the MD5 algorithm:
 *
 * <PRE>
 *  KMD5 context;
 *  context.update(QCString("ONE"));
 *  context.update(QCString("TWO"));
 *  context.update(QCString("THREE"));
 *  context.finalize();
 *  printf ( "Raw Digest output: %s", static_cast<char*>(context.rawDigest()) );
 *  printf ( "Hex Digest output: %s", context.hexDigest() );
 * </PRE>
 *
 * @short An adapted C++ implementation of RSA Data Securities MD5 algorithm.
 * @author Dawit Alemayehu <adawit@kde.org>
 */
class KMD5
{

public:

  /**
   * HEX    hexidecimal representation of the message digest
   * BIN    binary representation of the message digest
   */
  enum DigestType { BIN, HEX };

  /**
   * ERR_NONE                   no error occured. [default]
   * ERR_ALREADY_FINALIZED      @ref finalize() has already been invoked.
   * ERR_NOT_YET_FINALIZED      @ref hexDigest() or @ref rawDigest() invoked before @ref finalize().
   * ERR_CANNOT_READ_FILE       indicates a problem while trying to read the given file.
   * ERR_CANNOT_CLOSE_FILE      indicates a problem while trying to close the given file.
   */
  enum ErrorType { ERR_NONE, ERR_ALREADY_FINALIZED, ERR_NOT_YET_FINALIZED,
                   ERR_CANNOT_READ_FILE, ERR_CANNOT_CLOSE_FILE };

  /**
   * Default constructor that only performs initialization.
   * Unlike the other constructors
   */
  KMD5();

  /**
   * Constructor that initializes, computes, and finalizes
   * the message digest for the given string.
   *
   * NOTE: This is a convience constructor.  It is provided to
   * allow compatiability with the C implementation of this digest.
   */
  KMD5(Q_UINT8 * in);

  /**
   * Constructor that initializes, computes, and finalizes
   * the message digest for the given file.
   *
   * NOTE: This is a convience constructor.  As such it does
   * not allow the update of the message after it has been
   * invoked.  If you need to update the message after creating
   * the constructor,
   */
  KMD5(FILE *file);

  /**
   * Same as above except it accepts a QByteArray as its argument.
   */
  KMD5(const QByteArray& a );


  /**
   * Same as above except it accepts a QCString as its argument.
   */
  KMD5(const QCString& in);

  /**
   * @deprcated.  Use @ref KMD5(const QCString& in) instead!
   *
   * <u>IMPORTANT:</u> This constructor has been depricated and
   * will be removed in future release.  This is done to avoid
   * loss of data from misuse of the function since it first
   * converts the given data into Latin-1.  Additionally, this
   * conversion is very slow!
   */
  KMD5(const QString& in);

  /**
   * Updates the message to be digested.
   *
   * @param input  message to be added to the digest (QByteArray).
   */
  void update (const QByteArray& in );

  /**
   * Same as above except it accepts a pointer to FILE.
   *
   * NOTE that the file must have been already opened.  If you
   * want the file to be automatically closed, set @p closeFile
   * to TRUE.
   *
   * @param file       a pointer to FILE as returned by calls like f{d,re}open
   * @param closeFile  if true closes the file using fclose.
   */
  void update (FILE *file, bool closeFile = false );

  /**
   * Updates the message to be digested.
   *
   * @param input  message to be added to digest (unsigned char*)
   * @param len    the length of the given message.
   */
  void update (Q_UINT8 * in, int len = -1 );

  /**
   * Same as above except it accepts a QCString as its argument.
   */
  void update ( const QCString& in );

  /**
   * Same as above except it accepts a QString as its argument.
   *
   * <u>IMPORTANT:</u> This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrect
   * digest caclculation since the conversion of the QString input to
   * latin-1 can and will result in data loss if the input data contains
   * non-latin1 characters.  As such it is highly recommended that you
   * avoid this function unless you are absolutely certain that your
   * input does not contain any non-latin1 character!!
   */
  void update ( const QString& in );

  /**
   * Finalizes the message digest calculation.
   *
   * If you used the default constructor, you must invoke this function
   * before you can obtain the calculated digest value.
   */
  void finalize();

  /**
   * Compares the message digest supplied messaged digest @p msg_digest
   * with the one calculated for the input QCString @p input.
   *
   * @em NOTE: Calling this function will reset any previously calcualted
   * digests.  If you want to verify your token with the current digest
   * value, use @ref verify( const char*, DigestType ) instead.
   *
   * @param input       the message to be added to the digest value
   * @param msg_digest  the digest to compare the result against
   * @param type        the format of the result for comparison (binary or hexidecimal).
   *
   * @return true if the digests match, otherwise false.
   */
  bool verify( const QCString& in, const char * msg_digest,
               DigestType type = HEX );

  /**
   * Same as above except it takes a QString argument as its input.
   *
   * @em IMPORTANT: This function is ONLY provided for convenience
   * and backward compatability!  Using it can result in an incorrect
   * verification since the conversion of the QString input to latin-1
   * can and will result in data loss if the input data contains non-
   * latin1 characters.  As such it is highly recommended that you
   * avoid this function unless you are absolutely certain that your
   * input does not contain any non-latin1 character!!
   */
  bool verify( const QString& in, const char * msg_digest,
               DigestType type = HEX );

  /**
   * Same as above except it takes a pointer to a FILE as its input.
   *
   * @em NOTE: Calling this function will reset any previously
   * calcualted digests.  If you want to verify your token with the
   * current digest value, use @ref verify(const char*, DigestType)
   * instead.
   */
  bool verify( FILE* f, const char * msg_digest, DigestType type = HEX );

  /**
   * Compares the given string with with the current message digest.
   *
   * Unlike the other verification functions this one does not reset
   * the calculated message digest if one is already present.  Rather
   * it simply compares the given digest value against the calculated
   * one.
   *
   * @em NOTE: This function will return false if there was an error
   * calculating the message digest as well as when the verification
   * fails. You can use @ref hasErrored() to determine which is the case.
   *
   * @param msg_digest  the digest to compare the result against
   * @param type        the format of the result for comparison (binary
   *                    or hexidecimal).
   *
   * @return true if the digests match, otherwise false.
   */
  bool verify( const char* msg_digest, DigestType type = HEX );

  /**
   *  Re-initializes internal paramters.
   *
   * Note that calling this function will reset all internal variables
   * and hence any calculated message digest.  Invoke this function only
   * when you reuse the same object to perform another message digest
   * calculation.
   */
  void reset();

  /**
   * Returns the raw 16-byte binary value of the message
   * digest.
   *
   * NOTE: you are responsible for making a copy of this
   * string.
   *
   * @return the raw represenation of the digest or NULL
   *         if there was error calculating the digest.
   */
  Q_UINT8* rawDigest ();

  /**
   * Fills the given array with the binary representation of the
   * message digest.
   *
   * Use this method if you do not want to worry about making
   * copy of the digest once you obtain it.
   *
   * @param bin an array of 16 characters ( char[16] )
   */
  void rawDigest( HASH bin );

  /**
   * Returns the value of the calculated message digest in
   * a hexcidecimal representation.
   *
   * The 32-byte hexidecimal value is terminated by a NULL
   * character to form a properly terminated string.  Also
   * note that that if
   *
   * NOTE: You are responsible for making a copy of
   * this string!
   *
   * @return the hex represenation of the digest or NULL if
   *         there was error calculating the digest.
   */
  char * hexDigest ();

  /**
   * Fills the given array with the hexcidecimal representation of
   * the message digest.
   *
   * Use this method if you do not want to worry about making
   * copy of the digest once you obtain it.  Also note that this
   * method appends a NULL charater to the end of the array to
   * form a properly terminated string.  This is the reason why
   * the hexDigest is 33 characters long.
   *
   * @param bin an array of 33 characters ( char[33] )
   */
  void hexDigest( HASHHEX hex );

  /**
   * Indicates whether the message digest calculation failed or
   * succeeded.  Use @ref error() to determine the error type.
   *
   * @return false if errors are present, otherwise true
   */
  bool hasErrored() const { return (m_error != ERR_NONE); }

  /**
   * Returns the type of error that occurred.
   *
   * @return the error type. See @ref ErrorType for details.
   */
  int error() const { return m_error; }

protected:

  /**
   *  Initializer called by all constructors
   */
  void init();

  /**
   *  Performs the real update work.  Note
   *  that length is implied to be 64.
   */
  void transform( Q_UINT8 * buffer );

  /**
   * Returns true if the current message digest matches @p msg_digest.
   */
  bool isDigestMatch( const char * msg_digest, DigestType type );

private:

  void encode( Q_UINT8 *output, Q_UINT32 *in, Q_UINT32 len );
  void decode( Q_UINT32 *output, Q_UINT8 *in, Q_UINT32 len );

  Q_UINT32 rotate_left( Q_UINT32 x, Q_UINT32 n );
  Q_UINT32 F( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 G( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 H( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  Q_UINT32 I( Q_UINT32 x, Q_UINT32 y, Q_UINT32 z );
  void FF( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d, Q_UINT32 x,
               Q_UINT32  s, Q_UINT32 ac );
  void GG( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d, Q_UINT32 x,
                Q_UINT32 s, Q_UINT32 ac );
  void HH( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d, Q_UINT32 x,
                Q_UINT32 s, Q_UINT32 ac );
  void II( Q_UINT32& a, Q_UINT32 b, Q_UINT32 c, Q_UINT32 d, Q_UINT32 x,
             Q_UINT32 s, Q_UINT32 ac );

private:
  Q_UINT32 m_state[4];
  Q_UINT32 m_count[2];
  Q_UINT8 m_buffer[64];
  Q_UINT8 m_digest[16];
  ErrorType m_error;
  bool m_finalized;

  struct KMD5Private;
  KMD5Private* d;
};
#endif
