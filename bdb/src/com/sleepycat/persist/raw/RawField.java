/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2002,2006 Oracle.  All rights reserved.
 *
 * $Id: RawField.java,v 1.8 2006/10/30 21:14:33 bostic Exp $
 */

package com.sleepycat.persist.raw;

/**
 * The definition of a field in a {@link RawType}.
 *
 * <p>{@code RawField} objects are thread-safe.  Multiple threads may safely
 * call the methods of a shared {@code RawField} object.</p>
 *
 * @author Mark Hayes
 */
public interface RawField {

    /**
     * Returns the name of the field.
     */
    String getName();

    /**
     * Returns the type of the field, without expanding parameterized types,
     * or null if the type is an interface type or the Object class.
     */
    RawType getType();
}
