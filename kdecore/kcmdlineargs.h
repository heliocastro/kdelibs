/* This file is part of the KDE project
   Copyright (C) 1999 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KCMDLINEARGS_H_
#define _KCMDLINEARGS_H_

#include <qlist.h>
#include <qstring.h>

struct KCmdLineOptions
{
   const char *name;
   const char *description;
   const char *def; // Default
};

class KCmdLineArgsList;
class KApplication;
class KCmdLineParsedArgs;

/**
 *  This is a class for simple access to the command-line arguments.
 *
 *  It takes into account Qt-specific options, KDE-specific options
 *  and application specific options.
 *
 *  You have to give two arguments to the constructor, a number and an
 *  array of char[]. The first represents the number of strings in the
 *  array and the second is the array itself.
 *
 *  @short A class for command-line argument handling.
 *  @author Waldo Bastian
 *  @version 0.0.1
 */
class KCmdLineArgs
{
  friend KApplication;
public:
  // Static functions:
  
  /**
   * Initialization function.
   *
   * This function should be called as very first thing in your application.
   * @param argc as passed to main(...)
   * @param argv as passed to main(...)
   * @param appname The untranslated name of your application. This should
   *                match with argv[0].
   * @param description A shot description of what your application is about.
   * @param version A version and copyright message.
   */
  static void init(int _argc, char **_argv, 
                   const char *_appname, const char *_description,
                   const char *_version);

  /**
   * Add options to your application.
   *
   * You must make sure that all possible options have been added before
   * any class uses the command line arguments. 
   *
   * @param options A list of options which your code supplies
   * @param id A name with which these options can be identified.
   *          
   */
  static void addCmdLineOptions( const KCmdLineOptions *options, const char *name=0, const char *id = 0);

  /**
   * Access parsed arguments
   *
   * This function returns you all command line arguments which your code
   * handles. If unknown command line arguments are encountered the program
   * is aborted and usage information is shown. 
   *
   * @param id The name of the options you are interested in.
   */
  static KCmdLineArgs *parsedArgs(const char *id=0);

  /**
   * Print the usage help to stdout and exit.
   *
   * @param complete If true, print all available options.
   **/
  static void usage(bool complete = false);


  // Member functions:

  /**
   *  Destructor.
   */
  ~KCmdLineArgs();
  

  /**
   *  Read out a string option.
   *  
   *  @param option the name of the option without '-'.
   *
   *  @return The value of the option. If the option was not
   *          present on the cmdline the default is returned.
   */
  const char *getOption(const char *option);

  /**
   *  Read out a boolean option.
   *  
   *  @param option the name of the option without '-' or '-no'.
   *
   *  @return The value of the option. If the option was not
   *          present on the cmdline the default is returned.
   *          If the option is listed as '-no<option>' the
   *          default is true.
   *          If the option is listed as '-<option>' the
   *          default is false.
   */
  bool isSet(const char *option);
  

protected:   
  /**
   * @internal
   *  Constructor.
   */
  KCmdLineArgs( const KCmdLineOptions *_options, const char *_id, const char *_name);

  /**
   * @internal
   *
   * Checks what to do with a single option
   */
  static void findOption(const char *opt, int &i, bool enabled);

  /**
   * @internal
   *
   * Parse all arguments, verify correct syntax and put all arguments
   * where they belong.
   */
  static void parseAllArgs();
  
  /**
   * @internal for KApplication only:
   *
   * Return argc 
   */
  static int *qt_argc();

  /**
   * @internal for KApplication only:
   *
   * Return argv
   */

  static char ***qt_argv();

  /**
   * @internal 
   *
   *  Set a boolean option
   */
  void setOption(const char *option, bool enabled);

  /**
   * @internal
   *
   *  Set a string option
   */
  void setOption(const char *option, const char *value);
  
  const KCmdLineOptions *options;
  const char *name;
  const char *id;
  KCmdLineParsedArgs *parsedArgList;

  static KCmdLineArgsList *argsList; // All options.
  static const char *appname; // Appname
  static const char *description; // Desciption
  static const char *version; // Version
  static int argc; // The original argc
  static char **argv; // The original argv
  static bool parsed; // Whether we have parsed the arguments since calling init
};

#endif

