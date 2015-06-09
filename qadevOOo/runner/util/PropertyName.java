/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
package util;

/**
 * This interfaces describes some key names which are used in <CODE>lib.TestParameters</CODE>.
 */

public interface PropertyName {
    /**
     * parameter name: "AppExecutionCommand"
     * The AppExecutionCmd contains the full qualified<br>
     * command to an Application to be started.
     */
    String APP_EXECUTION_COMMAND = "AppExecutionCommand";
    /**
     * parameter name: "AppKillCommand"
     */
    String APP_KILL_COMMAND = "AppKillCommand";
    /**
     * parameter name: "ConnectionString"
     */
    String CONNECTION_STRING = "ConnectionString";
    String PIPE_CONNECTION_STRING = "PipeConnectionString";
    String USE_PIPE_CONNECTION = "UsePipeConnection";

    /**
     * parameter name: "TestBase"
     * The Testbase to be executed by the runner<br>
     * default is 'java_fat'
     */
    String TEST_BASE = "TestBase";
    /**
     * parameter name: "TestDocumentPath"
     */
    String TEST_DOCUMENT_PATH = "TestDocumentPath";
    /**
     * parameter name: "LoggingIsActive"
     * 'true' is a log should be written, 'false' elsewhere <br>
     * these will be provided by the testcases<br>
     * default is true
     */
    String LOGGING_IS_ACTIVE = "LoggingIsActive";
    /**
     * parameter name: "DebugIsActive"
     */
    String DEBUG_IS_ACTIVE = "DebugIsActive";
    /**
     * parameter name: "OutProduceer"
     * This parameter contains the class used<br>
     * for Logging
     */
    String OUT_PRODUCER = "OutProducer";
    /**
     * parameter name: "ShortWait"
     * Short wait time for the Office: default is 500 milliseconds
     */
    String SHORT_WAIT= "ShortWait";
    /**
     * internal only, no parameter
     * The OfficeProvider contains the full qualified
     * class that provides a connection to StarOffice<br>
     * default is helper.OfficeProvider
     */
    String OFFICE_PROVIDER = "OfficeProvider";
    /**
     * internal only, no parameter
     */
    String OFFICE_WATCHER = "Watcher";
    /**
     * internal only, no parameter
     * This parameter contains the class used<br>
     * for Logging
     */
    String LOG_WRITER = "LogWriter";
    /**
     * parameter name: "TimeOut"<p>
     * time out given in milli seconds
     * This parameter contains the timeout used<br>
     * by the watcher
     */
    String TIME_OUT = "TimeOut";
    /**
     * parameter name: "ThreadTimeOut"
     * This parameter contains the timeout used<br>
     * by the complex tests
     */
    String THREAD_TIME_OUT = "ThreadTimeOut";
    /**
     * parameter name: "OfficeCloseTimeOut"
     * This parameter contains the time which the office could use to close for
     * itself before it is destroyed. Default is 15000 ms
     */
    String OFFICE_CLOSE_TIME_OUT = "OfficeCloseTimeOut";
    /**
     * parameter name: "OperatingSystem"
     */
    String OPERATING_SYSTEM = "OperatingSystem";
    /**
     * parameter name: "AutoRestart"
     * If this parameter is <CODE>true</CODE> the <CODE>OfficeProvider</CODE> tries
     * to get the URL to the binary of the office and to fill the
     * <CODE>AppExecutionCommand</CODE> with useful content if needed.
     * Default is false.
     */
    String AUTO_RESTART = "AutoRestart";
    /**
     * parameter name: "NewOfficeInstance"
     */
    String NEW_OFFICE_INSTANCE = "NewOfficeInstance";

    /**
     * parameter name: "SRC_ROOT"<p>
     * path to the source root of OpenOffice.org
     */
    String SRC_ROOT = "SRC_ROOT";
    /**
     * parameter name: "Version"<p>
     * the name of the version to test
     */
    String VERSION = "Version";

    /**
     * parameter name "Shell"<p>
     * Path to a shell.
     * This shell is used to run some commands outside of Java
     * example: /bin/tcsh c:\\myShell\\myShell.exe
     */
    String SHELL = "Shell";
    /**
     * parameter name "Cygwin"<p>
     * If Cygwin is set to TRUE it indicates if the runner runs in a Cygwin
     * environment
     */
    String CYGWIN = "Cygwin";
    /**
     * parameter name: "NoCwsAttach"<p>
     * If this parameter is set to "true" , a status of CWS-UnoAPI-Tests was not attached to EIS<p>
     * @see complex.unoapi.CheckModuleAPI
     */
    String NO_CWS_ATTACH = "NoCwsAttach";
    /**
     * internal only, no parameter
     */
    String WNTMSCI = "wntmsci";
    /**
     * internal only, no parameter
     */
    String UNXLNGI = "unxlngi";
    /**
     * internal only, no parameter
     */
    String UNXSOLS = "unxsols";
    /**
     * internal only, no parameter
     */
    String UNXSOLI = "unxsoli";
    /**
     * internal only, no parameter
     */
    String UNXMACXI = "unxmacxi";

    /**
     * can be used to dont backup the user layer, faster office start/stop but less secure default is to backup the user layer
     */
    String DONT_BACKUP_USERLAYER = "DontBackupUserLayer";
}
