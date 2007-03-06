/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2000,2006 Oracle.  All rights reserved.
 *
 * $Id: TransactionWorker.java,v 1.15 2006/10/30 21:14:10 bostic Exp $
 */

package com.sleepycat.collections;

/**
 * The interface implemented to perform the work within a transaction.
 * To run a transaction, an instance of this interface is passed to the
 * {@link TransactionRunner#run} method.
 *
 * @author Mark Hayes
 */
public interface TransactionWorker {

    /**
     * Perform the work for a single transaction.
     *
     * @see TransactionRunner#run
     */
    void doWork()
        throws Exception;
}
