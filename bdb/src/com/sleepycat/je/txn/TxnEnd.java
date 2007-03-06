/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2006 Oracle.  All rights reserved.
 *
 * $Id: TxnEnd.java,v 1.32 2006/10/30 21:14:27 bostic Exp $
 */

package com.sleepycat.je.txn;

import java.nio.ByteBuffer;
import java.sql.Timestamp;

import com.sleepycat.je.log.LogEntryType;
import com.sleepycat.je.log.LogReadable;
import com.sleepycat.je.log.LogUtils;
import com.sleepycat.je.log.LoggableObject;
import com.sleepycat.je.utilint.DbLsn;

/**
 * This class writes out a transaction commit or transaction end record.
 */
public abstract class TxnEnd
    implements LoggableObject, LogReadable {

    protected long id;
    protected Timestamp time;
    private long lastLsn;

    TxnEnd(long id, long lastLsn) {
        this.id = id;
        time = new Timestamp(System.currentTimeMillis());
        this.lastLsn = lastLsn;
    }
    
    /**
     * For constructing from the log
     */
    public TxnEnd() {
        lastLsn = DbLsn.NULL_LSN;
    }

    /*
     * Accessors.
     */
    public long getId() {
        return id;
    }

    long getLastLsn() {
        return lastLsn;
    }

    protected abstract String getTagName();

    /*
     * Log support for writing.
     */

    /**
     * @see LoggableObject#getLogType
     */
    public abstract LogEntryType getLogType();

    /**
     * @see LoggableObject#marshallOutsideWriteLatch
     * Can be marshalled outside the log write latch.
     */
    public boolean marshallOutsideWriteLatch() {
        return true;
    }

    /**
     * @see LoggableObject#countAsObsoleteWhenLogged
     */
    public boolean countAsObsoleteWhenLogged() {
        return false;
    }

    /**
     * @see LoggableObject#postLogWork
     */
    public void postLogWork(long justLoggedLsn) {
    }

    /**
     * @see LoggableObject#getLogSize
     */
    public int getLogSize() {
        return LogUtils.LONG_BYTES +
            LogUtils.getTimestampLogSize() +
            LogUtils.getLongLogSize(); // lastLsn
    }

    /**
     * @see LoggableObject#writeToLog
     */
    public void writeToLog(ByteBuffer logBuffer) {
        LogUtils.writeLong(logBuffer, id);
        LogUtils.writeTimestamp(logBuffer, time);
        LogUtils.writeLong(logBuffer, lastLsn);
    }

    /**
     * @see LogReadable#readFromLog
     */
    public void readFromLog(ByteBuffer logBuffer, byte entryTypeVersion) {
        id = LogUtils.readLong(logBuffer);
        time = LogUtils.readTimestamp(logBuffer);
        lastLsn = LogUtils.readLong(logBuffer);
    }

    /**
     * @see LogReadable#dumpLog
     */
    public void dumpLog(StringBuffer sb, boolean verbose) {
        sb.append("<").append(getTagName());
        sb.append(" id=\"").append(id);
        sb.append("\" time=\"").append(time);
        sb.append("\">");
	sb.append(DbLsn.toString(lastLsn));
        sb.append("</").append(getTagName()).append(">");
    }

    /**
     * @see LogReadable#logEntryIsTransactional
     */
    public boolean logEntryIsTransactional() {
	return true;
    }

    /**
     * @see LogReadable#getTransactionId
     */
    public long getTransactionId() {
	return id;
    }
}
