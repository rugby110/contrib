/*
  This code is part of FCPTools - an FCP-based client library for Freenet

	Developers:
 	- David McNab <david@rebirthing.co.nz>
	- Jay Oliveri <ilnero@gmx.net>
	
  CopyLeft (c) 2001 by David McNab

	Currently maintained by Jay Oliveri <ilnero@gmx.net>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <sys/types.h>

#ifndef WINDOWS
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ezFCPlib.h"


extern int   crSockConnect(hFCP *hfcp);
extern void  crSockDisconnect(hFCP *hfcp);


/*
	Returns 0 on success.
*/
int fcpPut(hFCP *hfcp)
{
	char buf[8193];
	int fd;
	int rc;

	hChunk *chunk;

	if (hfcp->key->chunkCount > 1) {
		_fcpLog(FCP_LOG_DEBUG, "Cannot insert multiple splifile chunks via fcpPut()");
		return -1;
	}

	if (hfcp->key->size > CHUNK_BLOCK_SIZE) {
		_fcpLog(FCP_LOG_DEBUG, "FEC not yet implemented");
		return -1;
	}

  /* connect to Freenet FCP */
  if (crSockConnect(hfcp) != 0)
    return -1;
	
  /* create a put message */
  if (hfcp->key->metadata != NULL) {
		/* Code for inserting with metadata */

  }
  else {
    snprintf(buf, FCPRESP_BUFSIZE,
						 "ClientPut\nURI=%s\nHopsToLive=%x\nDataLength=%x\nData\n",
						 hfcp->key->uri->uri_str,
						 hfcp->htl,
						 hfcp->key->size
						 );
  }

	/* Send fcpID */
	if (send(hfcp->socket, _fcpID, 4, 0) == -1)
		 return -1;
		 
	/* Send ClientPut command */
	if (send(hfcp->socket, buf, strlen(buf), 0) == -1) {
		_fcpLog(FCP_LOG_DEBUG, "Could not send ClientPut command");
		return -1;
	}

	/* Open file we are about to send */
	chunk = hfcp->key->chunks[0];
	if (!(chunk->file = fopen(chunk->filename, "rb"))) {
		_fcpLog(FCP_LOG_DEBUG, "Could not open chunk for reading in order to insert into Freenet");
		return -1;
	}

  /* Now send data */
	fd = fileno(chunk->file);
	while ((rc = read(fd, buf, 8192)) > 0) {

    if (send(hfcp->socket, buf, rc, 0) < 0) {
      _fcpLog(FCP_LOG_DEBUG, "Could not write key data to Freenet");
      return -1;
		}
	}

  fclose(chunk->file);
	chunk->file = 0;
  
  /* expecting a success response */
  rc = _fcpRecvResponse(hfcp);
  
  switch (rc) {
  case FCPRESP_TYPE_SUCCESS:
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: SUCCESS: %s", chunk->filename);
    break;

  case FCPRESP_TYPE_KEYCOLLISION:
    /* either of these are ok */
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: KEYCOLLISION: %s", chunk->filename);
    break;

  case FCPRESP_TYPE_ROUTENOTFOUND:
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: ROUTENOTFOUND: %s", chunk->filename);
    break;

  case FCPRESP_TYPE_FORMATERROR:
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: FORMATERROR: %s", chunk->filename);
    break;

  case FCPRESP_TYPE_FAILED:
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: FAILED: %s", chunk->filename);
    break;

  default:
    _fcpLog(FCP_LOG_DEBUG, "fcpPut: unknown response from node for %s", chunk->filename);
    crSockDisconnect(hfcp);
    return -1;
  }
  
  /* finished with connection */
  crSockDisconnect(hfcp);
  
  if ((rc != FCPRESP_TYPE_SUCCESS) && (rc != FCPRESP_TYPE_KEYCOLLISION))
    return -1;
	else
		return 0;
}
