package org.kde.kjas.server;

import java.io.*;
import java.util.*;
import java.awt.*;

/**
 * Encapsulates the KJAS protocol.
 *
 * <H3>Change Log</H3>
 * <PRE>
 * $Log$
 * Revision 1.4  2000/03/21 03:44:44  rogozin
 *
 * New Java support has been merged.
 *
 * Revision 1.3  2000/03/08 01:04:56  rogozin
 *
 * Java console implemented
 *
 * Revision 1.2  2000/01/27 23:41:57  rogozin
 * All applet parameters are passed to KJAS now
 * Next step - make use of them.
 *
 * Revision 1.1.1.1  1999/07/22 17:28:08  rich
 * This is a current snapshot of my work on adding Java support
 * to KDE. Applets now work!
 *
 * </PRE>
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
public class KJASProtocolHandler
{
   BufferedReader commands;
   PrintStream signals;
   KJASAppletRunner runner;
   
public KJASProtocolHandler( InputStream commands,
                               OutputStream signals,
                               KJASAppletRunner runner,
                               String password )
   {
      this.commands = new BufferedReader( new InputStreamReader( commands ) );
      this.runner = runner;
   }

   public void commandLoop()
      throws IOException
   {
      String line = commands.readLine();
      while ( line != null ) {
          if(Main.debug)
              System.out.println("KJAS: Processing command >" + line + "<"); 
          processCommand( line );
          if(Main.debug)
              System.out.println("KJAS: Processing done\n");
          line = commands.readLine();
      }
   }
    
   public void processCommand( String command )
      throws IllegalArgumentException
   {
      // Sanity checks
      if ( command == null )
         throw new IllegalArgumentException( "processCommand() received null" );
      if ( command.length() > 1024 )
         throw new IllegalArgumentException( "processCommand() received suspiciously large command string (ignoring)" );

      int index = command.indexOf( '!' );
      String commandName;
      String args;

      if ( index != -1) {
         commandName = command.substring( 0, index );
         args = command.substring( index + 1, command.length() );
      }
      else {
         commandName = command;
         args = "";
      }
      
      if ( commandName.equals( "createContext" ) ) {
         createContext( args );
      }
      else if ( commandName.equals( "destroyContext" ) ) {
         destroyContext( args );
      }
      else if ( commandName.equals( "createApplet" ) ) {
         createApplet( args );
      }
      else if ( commandName.equals( "createJARApplet" ) ) {
         //         createJARApplet( args );
      }
      else if ( commandName.equals( "destroyApplet" ) ) {
         destroyApplet( args );
      }
      else if ( commandName.equals( "showApplet" ) ) {
         showApplet( args );
      }
      else if ( commandName.equals( "startApplet" ) ) {
         startApplet( args );
      }
      else if ( commandName.equals( "stopApplet" ) ) {
         stopApplet( args );
      }
      else if ( commandName.equals( "setParameter" ) ) {
         setParameter( args );
      }
      else if ( commandName.equals( "quit" ) ) {
         System.exit(0);
      }
      else {
         throw new IllegalArgumentException( "Unknown command: " + commandName );
      }
   }

   public void createContext( String args )
      throws IllegalArgumentException
   {
       String idStr = args.trim();
       runner.createContext( idStr );
   }

   public void destroyContext( String args )
      throws IllegalArgumentException
   {
       String idStr = args.trim();
       runner.destroyContext( idStr );
   }

   public void createApplet( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();
         String name = tok.nextToken();
         String className = tok.nextToken();
         String baseURL = tok.nextToken();
         String codeBase = tok.nextToken();
         if(codeBase.equals("null"))
           codeBase = null;
         String archive = tok.nextToken();
         if(archive.equals("null"))
           archive = null;
         String widthStr = tok.nextToken();
         String heightStr = tok.nextToken();

         int width = Integer.parseInt( widthStr );
         int height = Integer.parseInt( heightStr );

         runner.createApplet( contextIdStr, appletIdStr, name, className, baseURL, 
                              codeBase, archive, new Dimension(width, height) );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "createApplet(): Wrong number of args" );
      }
   }

   public void destroyApplet( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();

         runner.destroyApplet( contextIdStr, appletIdStr );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "destroyApplet(): Wrong number of args" );
      }
   }

   public void showApplet( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();
         String title = tok.nextToken();

         runner.showApplet( contextIdStr, appletIdStr, title );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "showApplet(): Wrong number of args" );
      }
   }

   public void startApplet( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();

         runner.startApplet( contextIdStr, appletIdStr );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "startApplet(): Wrong number of args" );
      }
   }

   public void stopApplet( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();

         runner.stopApplet( contextIdStr, appletIdStr );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "stopApplet(): Wrong number of args" );
      }
   }

   public void setParameter( String args )
      throws IllegalArgumentException
   {
      try {
         String buf = args.trim();
         StringTokenizer tok = new StringTokenizer( buf, "!" );

         String contextIdStr = tok.nextToken();
         String appletIdStr = tok.nextToken();
         String name = tok.nextToken();
         String value = tok.nextToken();

         runner.setParameter( contextIdStr, appletIdStr, name, value );
      }
      catch ( NoSuchElementException nsee ) {
         throw new IllegalArgumentException( "stopApplet(): Wrong number of args" );
      }
   }

   public static void main( String[] args )
   {
      try {
         KJASAppletRunner runner = new KJASAppletRunner();
         KJASProtocolHandler handler = new KJASProtocolHandler( System.in, System.out,
                                                                runner, "friend" );
         handler.processCommand( "createContext!0" );
         handler.processCommand( "createApplet!0!0!fred!Lake.class!http://127.0.0.1/applets/" );
         handler.processCommand( "setParameter!0!0!image!logo.gif" );
         handler.processCommand( "showApplet!0!0!unique_title_one" );
         System.err.println( "Entering commandLoop()" );
         handler.commandLoop();
      }
      catch ( Exception e ) {
         System.err.println( "Error: " + e );
         e.printStackTrace();
      }
   }
}
